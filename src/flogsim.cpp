#include <cstdio>
#include <cassert>

#include <memory>
#include <deque>
#include <queue>
#include <algorithm>

#include "event.hpp"
#include "model.hpp"

Model model{Time(1), Time(1), Time(2), 1023};

struct CpuTimeline
{
  static Time get_last_or_zero(const std::deque<auto> &queue)
  {
    if (queue.empty())
      return Time(0);
    return queue.back().end();
  }

  std::deque<CpuEvent> cpu_events;
  std::deque<SendGap> send_gaps;
  std::deque<RecvGap> recv_gaps;
  FinishEvent finish;

  CpuTimeline() = default;

  void dump() const
  {
    auto dumper = [] (const Event &e) {
      e.dump();
      std::printf(" ");
    };
    std::for_each(cpu_events.begin(), cpu_events.end(), dumper);
    std::printf(",");
    std::for_each(send_gaps.begin(), send_gaps.end(), dumper);
    std::printf(",");
    std::for_each(recv_gaps.begin(), recv_gaps.end(), dumper);
    std::printf(",");
    finish.dump();
  }
};

class Timeline
{
  Time total_time;
public:
  std::vector<CpuTimeline> per_cpu_time;

  Timeline(int nodes) :
    per_cpu_time(nodes)
  {}

  void update_total_time(Time time)
  {
    if (total_time < time) {
      total_time = time;
    }
  }

  void dump() const
  {
    std::printf("CPU, CpuEvent, SendGaps, RecvGaps, Finish\n");
    for(int i = 0; i < per_cpu_time.size(); i++) {
      auto const &cpu = per_cpu_time[i];
      std::printf("%d,", i);
      cpu.dump();
      std::printf("\n");
    }
  }
};

class TaskQueue;
class Collective;

class Task
{
protected:
  Time time;
  int node; // Node where the task is executed
public:

  Task(Time time, int node) :
    time(time), node(node)
  {
  }

  int get_node() const
  {
    return node;
  }

  Time start() const
  {
    return time;
  }
  // Execute a task. If needed, schedule another task and put it into
  // the TaskQueue. Task creates timeline events as side effects.
  // Returns true if the task finished. Return false if task was
  // rescheduled.
  virtual bool execute(Timeline &timeline, TaskQueue &tq) const = 0;

  virtual void notify(Collective &coll, TaskQueue &q) = 0;

  bool operator<(const Task &other) const
  {
    return (time < other.time) ? true : (node < other.node);
  }

  bool operator==(const Task &other) const
  {
    return !(*this < other) && !(other < *this);
  }

  bool operator!=(const Task &other) const
  {
    return !(*this == other);
  }

  bool operator>(const Task &other) const
  {
    return !(*this < other) && (other != *this);
  }

  virtual const char* type() const = 0;
};

class TaskQueue
{
  struct QueueItem
  {
    Time time;
    std::shared_ptr<Task> task;

    QueueItem(Time time, std::shared_ptr<Task> task) :
      time(time), task(task)
    {}

    // Take time with the minimum value. By default priority_queue
    // puts the maximum on the top.
    bool operator<(const QueueItem &other) const
    {
      return time > other.time;
    }
  };

  std::priority_queue<QueueItem> queue;

  Time current_time;

  void progress(Time absolute)
  {
    assert(!(absolute < current_time));
    current_time = absolute;
  }
public:

  Time now() const
  {
    return current_time;
  }

  std::shared_ptr<Task> pop()
  {
    auto item = queue.top();
    queue.pop();
    progress(item.time);
    return item.task;
  }

  bool empty() const
  {
    return queue.empty();
  }

  void schedule(std::shared_ptr<Task> task)
  {
    queue.push(QueueItem(task->start(), task));
  }
};

class SendTask;
class RecvTask;
class MsgTask;
class FinishTask;

class Collective
{
public:
  virtual void populate(TaskQueue &eq) = 0;

  virtual void accept(const SendTask&, TaskQueue&)
  {
  }

  virtual void accept(const RecvTask&, TaskQueue&)
  {
  }

  virtual void accept(const MsgTask&, TaskQueue&)
  {
  }

  virtual void accept(const FinishTask&, TaskQueue&)
  {
  }
};

class RecvTask : public Task
{
public:
  RecvTask(const RecvTask &other) = default;

  RecvTask(Time time, int node) :
    Task(time, node)
  {}

  bool execute(Timeline &timeline, TaskQueue &tq) const override final
  {
    auto &cpu = timeline.per_cpu_time[node];

    // Calculate time for CpuTime
    Time cpu_last = CpuTimeline::get_last_or_zero(cpu.cpu_events);
    Time recv_last = CpuTimeline::get_last_or_zero(cpu.recv_gaps);

    auto start_time = std::max({cpu_last, recv_last, tq.now()});

    if (start_time > tq.now()) {
      tq.schedule(std::make_shared<RecvTask>(start_time, this->node));
      return false;
    }

    RecvGap rg{start_time};
    CpuEvent cpu_event{start_time};

    cpu.recv_gaps.push_back(rg);
    cpu.cpu_events.push_back(cpu_event);

    return true;
  }

  void notify(Collective &coll, TaskQueue &tq) override final
  {
    coll.accept(*this, tq);
  }

  virtual const char* type() const override final
  {
    return "RecvTask";
  }
};

class MsgTask : public Task
{
public:
  MsgTask(Time time, int node) :
    Task(time, node)
  {
  }

  bool execute(Timeline &timeline, TaskQueue &tq) const override final
  {
    // Calculate time when receive task can be scheduled
    auto recv_time = tq.now() + model.L;

    tq.schedule(std::make_shared<RecvTask>(recv_time, node));
    return true;
  }

  void notify(Collective &coll, TaskQueue &tq) override final
  {
    coll.accept(*this, tq);
  }

  virtual const char* type() const override final
  {
    return "MsgTask";
  }
};

class SendTask : public Task
{
  int recv;
public:
  SendTask(const SendTask &other) = default;

  SendTask(Time time, int node, int recv) :
    Task(time, node), recv(recv)
  {}

  bool execute(Timeline &timeline, TaskQueue &tq) const override final
  {
    auto &cpu = timeline.per_cpu_time[node];

    // Calculate time for CpuTime
    Time cpu_last = CpuTimeline::get_last_or_zero(cpu.cpu_events);
    Time send_last = CpuTimeline::get_last_or_zero(cpu.send_gaps);

    auto start_time = std::max({cpu_last, send_last, tq.now()});

    if (start_time > tq.now()) {
      tq.schedule(std::make_shared<SendTask>(start_time, this->node, this->recv));
      return false;
    }

    SendGap sg{start_time};
    CpuEvent cpu_event{start_time};

    cpu.send_gaps.push_back(sg);
    cpu.cpu_events.push_back(cpu_event);

    tq.schedule(std::make_shared<MsgTask>(cpu_event.end(), recv));
    return true;
  }

  void notify(Collective &coll, TaskQueue &tq) override final
  {
    coll.accept(*this, tq);
  }

  virtual const char* type() const override final
  {
    return "SendTask";
  }
};

class FinishTask : public Task
{
public:

  FinishTask(int node) :
    Task(Time::max(), node)
  {
  }

  bool execute(Timeline &timeline, TaskQueue &tq) const override final
  {
    auto &cpu = timeline.per_cpu_time[node];

    // Calculate time for CpuTime
    auto cpu_last =  cpu.cpu_events.back();

    cpu.finish = FinishEvent(cpu_last.end());

    timeline.update_total_time(cpu.finish.end());
    return true;
  }

  void notify(Collective &coll, TaskQueue &tq) override final
  {
    coll.accept(*this, tq);
  }

  virtual const char* type() const override final
  {
    return "FinalTask";
  }
};

class BinaryBroadcast : public Collective
{
  int nodes;
  void post_sends(int sender, TaskQueue &tq)
  {
    for (int i = 1; i <= 2; i++) {
      int recv = 2 * sender + i;
      if (recv < nodes) {
        tq.schedule(std::make_shared<SendTask>(tq.now(), sender, recv));
      }
    }
    tq.schedule(std::make_shared<FinishTask>(sender));
  }
public:
  BinaryBroadcast(int nodes)
    : nodes(nodes)
  {
  }

  virtual void accept(const RecvTask& task, TaskQueue& tq)
  {
    post_sends(task.get_node(), tq);
  }

  void populate(TaskQueue &tq) override
  {
    int root = 0;
    post_sends(root, tq);
  }
};

int main(int argc, char *argv[])
{
  TaskQueue tq;
  Timeline timeline(model.P);

  BinaryBroadcast coll(model.P);
  coll.populate(tq);

  while (!tq.empty()) {
    std::shared_ptr<Task> task = tq.pop();

    printf("[%d] Task %s %" PRIu64 " @ node %d", tq.now(), task->type(), task->start(), task->get_node());
    if (task->execute(timeline, tq)) {
      task->notify(coll, tq);
      printf("\n");
    } else {
      printf(" rescheduled\n");
    }
  }

  timeline.dump();

  return 0;
}
