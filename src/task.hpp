#pragma once

#include "timeline.hpp"
#include "task_queue.hpp"
#include "collective.hpp"
#include "model.hpp"

#include <iostream>

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
  int recv;
public:
  int sender() const { return node; }
  int receiver() const { return recv; }

  RecvTask(const RecvTask &other) = default;

  RecvTask(Time time, int send, int recv) :
    Task(time, send), recv(recv)
  {}

  bool execute(Timeline &timeline, TaskQueue &tq) const override final
  {
    auto &cpu = timeline.per_cpu_time[receiver()];

    // Calculate time for CpuTime
    Time cpu_last = cpu.cpu_events.get_last_or_zero();
    Time recv_last = cpu.recv_gaps.get_last_or_zero();

    auto start_time = std::max({cpu_last, recv_last, tq.now()});

    if (start_time > tq.now()) {
      tq.schedule(std::make_shared<RecvTask>(start_time, sender(), receiver()));
      return false;
    }

    RecvGap rg{start_time, sender()};
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
  int recv;
public:
  int sender() const { return node; }
  int receiver() const { return recv; }

  MsgTask(Time time, int send, int recv) :
    Task(time, send), recv(recv)
  {
  }

  bool execute(Timeline &timeline, TaskQueue &tq) const override final
  {
    // Calculate time when receive task can be scheduled
    auto recv_time = tq.now() + LogP::Model::get().L;

    tq.schedule(std::make_shared<RecvTask>(recv_time, sender(), receiver()));
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
  {
    assert(time < Time(50));
  }

  bool execute(Timeline &timeline, TaskQueue &tq) const override final
  {
    auto &cpu = timeline.per_cpu_time[node];

    // Calculate time for CpuTime
    Time cpu_last = cpu.cpu_events.get_last_or_zero();
    Time send_last = cpu.send_gaps.get_last_or_zero();

    auto start_time = std::max({cpu_last, send_last, tq.now()});

    if (start_time > tq.now()) {
      tq.schedule(std::make_shared<SendTask>(start_time, node, recv));
      return false;
    }

    SendGap sg{start_time};
    CpuEvent cpu_event{start_time};

    cpu.send_gaps.push_back(sg);
    cpu.cpu_events.push_back(cpu_event);

    tq.schedule(std::make_shared<MsgTask>(cpu_event.end(), node, recv));
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

    cpu.finish.push_back(FinishEvent(cpu_last.end()));

    timeline.update_total_time(cpu.finish.back().end());
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
