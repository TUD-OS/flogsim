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
  int _sender; // Node where the task is executed
  int _receiver; // Destination node. Can be the same as sender
public:

  Task(Time time, int sender, int receiver) :
    time(time), _sender(sender), _receiver(receiver)
  {
  }

  int sender() const { return _sender; }
  int receiver() const { return _receiver; }

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
    return (time < other.time) ? true : (sender() < other.sender());
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

  friend std::ostream &operator<<(std::ostream &os, const Task &t)
  {
    os << "Task[@ " << t.time << "] "
       << t.type() << " "
       << t.sender() << " -> " << t.receiver();
    return os;
  }
};

namespace LogP
{

class RecvTask : public Task
{
public:
  RecvTask(const RecvTask &other) = default;

  RecvTask(Time time, int sender, int receiver) :
    Task(time, sender, receiver)
  {}

  bool execute(Timeline &timeline, TaskQueue &tq) const override final;

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
  MsgTask(Time time, int sender, int receiver) :
    Task(time, sender, receiver)
  {
  }

  bool execute(Timeline &timeline, TaskQueue &tq) const override final;

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
public:
  SendTask(const SendTask &other) = default;

  SendTask(Time time, int sender, int receiver) :
    Task(time, sender, receiver)
  {
  }

  bool execute(Timeline &timeline, TaskQueue &tq) const override final;

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

  FinishTask(int sender) :
    Task(Time::max(), sender, sender)
  {
  }

  bool execute(Timeline &timeline, TaskQueue &tq) const override final;

  void notify(Collective &coll, TaskQueue &tq) override final
  {
    coll.accept(*this, tq);
  }

  virtual const char* type() const override final
  {
    return "FinalTask";
  }
};

class FailureTask : public Task
{
public:

  FailureTask(const std::shared_ptr<Task> task) :
    Task(task->start(), task->sender(), task->receiver())
  {
  }

  bool execute(Timeline &timeline, TaskQueue &tq) const override final;

  void notify(Collective &coll, TaskQueue &tq) override final
  {
    coll.accept(*this, tq);
  }

  virtual const char* type() const override final
  {
    return "FailureTask";
  }
};

}
