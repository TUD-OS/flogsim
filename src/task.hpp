#pragma once

#include "timeline.hpp"
#include "collective.hpp"
#include "model.hpp"
#include "task_queue.hpp"
#include "sequence.hpp"

#include <iostream>
#include <type_traits>

class TaskQueue;

class Task
{
protected:
  Sequence _seq;
  Time time;
  int _sender; // Node where the task is executed
  int _receiver; // Destination node. Can be the same as sender
public:

  Task(Sequence seq, Time time, int sender, int receiver)
    : _seq(seq),
      time(time),
      _sender(sender),
      _receiver(receiver)
  {}


  Task(const Task &other) = default;

  static auto make_task_attime(auto *task, Time time)
  {
    typedef typename std::remove_cv<
      typename std::remove_pointer<typeof(task)>::type>::type task_t;
    return std::make_shared<task_t>(task->seq(), time, task->sender(), task->receiver());
  }

  template<typename T, class ...Args>
  static std::shared_ptr<T> make_from_task(const Task *task, Args... args)
  {
    return std::make_shared<T>(task->seq(), args...);
  }

  template<typename T>
  static std::shared_ptr<T> make_from_task(const Task *task)
  {
    return std::make_shared<T>(task);
  }

  template<typename T, class ...Args>
  static auto make_new(Args... args)
  {
    return std::make_shared<T>(Sequence::next(), args...);
  }

  Sequence seq() const { return _seq; }
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

  RecvTask(Sequence seq, Time time, int sender, int receiver) :
    Task(seq, time, sender, receiver)
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
  MsgTask(Sequence seq, Time time, int sender, int receiver) :
    Task(seq, time, sender, receiver)
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

  SendTask(Sequence seq, Time time, int sender, int receiver) :
    Task(seq, time, sender, receiver)
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

  FinishTask(Sequence seq, int sender) :
    Task(seq, Time::max(), sender, sender)
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

  FailureTask(const Task *task) :
    Task(task->seq(), task->start(), task->sender(), task->receiver())
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
