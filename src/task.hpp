#pragma once

#include "timeline.hpp"
#include "collective.hpp"
#include "model.hpp"
#include "sequence.hpp"

#include <iostream>
#include <type_traits>
#include <typeinfo>

class TaskQueue;

struct Tag : public Integer<Tag>
{
  int tag;

  int get() const { return tag; }

  explicit Tag(int tag)
    : tag(tag)
  {}
  Tag(const Tag &other) = default;
};

struct TaskData
{
protected:
  Sequence _seq;
  Tag _tag;
  Time time;
  int _sender; // Node where the task is executed
  int _receiver; // Destination node. Can be the same as sender

public:

  TaskData(Sequence seq, Tag tag, Time time, int sender, int receiver)
    : _seq(seq),
      _tag(tag),
      time(time),
      _sender(sender),
      _receiver(receiver)
  {}
};

class Task : public TaskData
{
public:

  Task(const Task &other) = default;
  Task(const TaskData &task_data)
    : TaskData(task_data)
  {}

  Sequence seq() const { return _seq; }
  Tag tag() const { return _tag; }
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
    return (time < other.time);
    return (time < other.time) ?
      true : ((time == other.time) && (seq() < other.seq()));
  }

  bool operator>(const Task &other) const
  {
    return (other < *this);
  }

  bool operator==(const Task &other) const
  {
    return !(*this < other) && !(other < *this);
  }

  bool operator!=(const Task &other) const
  {
    return !(*this == other);
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

template<typename CHILD>
class TaskCounted : public Task
{
protected:
  static int &get_counter()
  {
    static int counter;
    return counter;
  }

public:
  TaskCounted(const TaskData &task) :
    Task(task)
  {}

  static auto make_task_attime(auto *task, Time time)
  {
    typedef typename std::remove_cv<
      typename std::remove_pointer<typeof(task)>::type>::type task_t;
    return std::make_shared<task_t>(task->seq(), task->tag(), time,
                                    task->sender(), task->receiver());
  }

  template<class ...Args>
  static std::shared_ptr<CHILD> make_from_task(const Task *task, Args... args)
  {
    get_counter() ++;
    return std::make_shared<CHILD>(task->seq(), task->tag(), args...);
  }

  static std::shared_ptr<CHILD> make_from_task(const Task *task)
  {
    get_counter() ++;
    return std::make_shared<CHILD>(task);
  }

  template<class ...Args>
  static auto make_new(Args... args)
  {
    get_counter() ++;
    return std::make_shared<CHILD>(Sequence::next(), args...);
  }

  void notify(Collective &coll, TaskQueue &tq) override final
  {
    coll.accept(*static_cast<CHILD *>(this), tq);
  }

  virtual const char* type() const override final
  {
    return typeid(CHILD).name();
  }

  static int issued()
  {
    return get_counter();
  }
};

namespace LogP
{

class RecvTask : public TaskCounted<RecvTask>
{
public:
  RecvTask(const RecvTask &other) = default;

  RecvTask(Sequence seq, Tag tag, Time time, int sender, int receiver) :
    TaskCounted(TaskData{seq, tag, time, sender, receiver})
  {}

  bool execute(Timeline &timeline, TaskQueue &tq) const override final;
};

class MsgTask : public TaskCounted<MsgTask>
{
public:
  MsgTask(Sequence seq, Tag tag, Time time, int sender, int receiver) :
    TaskCounted(TaskData{seq, tag, time, sender, receiver})
  {
  }

  bool execute(Timeline &timeline, TaskQueue &tq) const override final;
};

class SendTask : public TaskCounted<SendTask>
{
public:
  SendTask(const SendTask &other) = default;

  SendTask(Sequence seq, Tag tag, Time time, int sender, int receiver) :
    TaskCounted(TaskData{seq, tag, time, sender, receiver})
  {
  }

  bool execute(Timeline &timeline, TaskQueue &tq) const override final;
};

class FinishTask : public TaskCounted<FinishTask>
{
public:

  FinishTask(Sequence seq, int sender) :
    TaskCounted(TaskData{seq, Tag(0), Time::max(), sender, sender})
  {
  }

  bool execute(Timeline &timeline, TaskQueue &tq) const override final;
};

class FailureTask : public TaskCounted<FailureTask>
{
public:

  FailureTask(const Task *task) :
    TaskCounted(*task)
  {
  }

  bool execute(Timeline &timeline, TaskQueue &tq) const override final;
};

}
