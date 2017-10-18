#pragma once

#include "timeline.hpp"
#include "task_queue.hpp"
#include "collective.hpp"
#include "model.hpp"

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

namespace LogP
{

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
    auto recv_time = tq.now() + LogP::Model::get().L;

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

}
