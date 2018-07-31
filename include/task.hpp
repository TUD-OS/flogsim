#pragma once

#include <iostream>
#include <type_traits>
#include <typeinfo>

#include "timeline.hpp"
#include "collective.hpp"
#include "model.hpp"
#include "sequence.hpp"
#include "tag.hpp"
#include "counter.hpp"

class TaskQueue;

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
  {
    assert(time >= Time(0));
    assert(_sender   >= 0 && _sender < Globals::get().conf().P);
    assert(_receiver >= 0 && _sender < Globals::get().conf().P);
  }
};

enum class TaskPriority : int
{
  RECEIVER    = 3,
  RECEIVE_END = 4,
  NORMAL      = 5,
  INIT        = 6,
  SENDER      = 7,
  SEND_END    = 8,
  IDLE        = 9,
  FINISH      = 10,
};

class Task : public TaskData
{
public:
  virtual TaskPriority task_priority() const { return TaskPriority::NORMAL; }

  static bool is_init(const Task& task)
  {
    return task.task_priority() == TaskPriority::INIT;
  }

  static bool is_receive_start(const Task& task)
  {
    return task.task_priority() == TaskPriority::RECEIVER;
  }

  static bool is_send_start(const Task& task)
  {
    return task.task_priority() == TaskPriority::SENDER;
  }

  static bool is_idle(const Task& task)
  {
    return task.task_priority() == TaskPriority::IDLE;
  }

  static bool is_finish(const Task& task)
  {
    return task.task_priority() == TaskPriority::FINISH;
  }

  Task(const Task &other) = default;
  Task(const TaskData &task_data)
    : TaskData(task_data)
  {}
  virtual ~Task(){}

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
    if (time < other.time) {
      return true;
    } else if (time > other.time) {
      return false;
    }

    auto prio = Globals::get().conf().priority;

    if (prio.get() == Configuration::Priority::TAG) {
      if (tag() < other.tag()) {
        return true;
      } else if (tag() > other.tag()) {
        return false;
      }
    }

    if (task_priority() < other.task_priority()) {
      return true;
    } else if (task_priority() > other.task_priority()) {
      return false;
    }

    return (seq() < other.seq());
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
       << t.type() << ": "
       << t.sender() << " -> " << t.receiver();
    return os;
  }
};

template<typename CHILD>
class TaskCounted : public Task
{
protected:
  static auto &counter()
  {
    static Counter counter;
    return counter;
  }

  static auto &rescheduled()
  {
    static Counter rescheduled;
    return rescheduled;
  }

public:
  TaskCounted(const TaskData &task) :
    Task(task)
  {}

  static auto make_task_attime(auto *task, Time time)
  {
    rescheduled().inc();
    typedef typename std::remove_cv<
      typename std::remove_pointer<decltype(task)>::type>::type task_t;
    return std::make_unique<task_t>(task->seq(), task->tag(), time,
                                    task->sender(), task->receiver());
  }

  template<class ...Args>
  static std::unique_ptr<CHILD> make_from_task(const Task *task, Args... args)
  {
    counter().inc();
    return std::make_unique<CHILD>(task->seq(), task->tag(), args...);
  }

  static std::unique_ptr<CHILD> make_from_task(const Task *task)
  {
    counter().inc();
    return std::make_unique<CHILD>(task);
  }

  template<class ...Args>
  static std::unique_ptr<CHILD> make_new(Args... args)
  {
    counter().inc();
    return std::make_unique<CHILD>(Sequence::next(), args...);
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
    return counter().get();
  }

  static int reschedules()
  {
    return rescheduled().get();
  }
};

class RecvStartTask : public TaskCounted<RecvStartTask>
{
  virtual TaskPriority task_priority() const { return TaskPriority::RECEIVER; }
public:
  RecvStartTask(const RecvStartTask &other) = default;

  RecvStartTask(Sequence seq, Tag tag, Time time, int sender, int receiver) :
    TaskCounted(TaskData{seq, tag, time, sender, receiver})
  {}

  bool execute(Timeline &timeline, TaskQueue &tq) const override final;
};

class RecvEndTask : public TaskCounted<RecvEndTask>
{
  virtual TaskPriority task_priority() const { return TaskPriority::RECEIVE_END; }
public:
  RecvEndTask(const RecvEndTask &other) = default;

  RecvEndTask(Sequence seq, Tag tag, Time time, int sender, int receiver) :
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

class SendStartTask : public TaskCounted<SendStartTask>
{
  virtual TaskPriority task_priority() const { return TaskPriority::SENDER; }
public:
  SendStartTask(const SendStartTask &other) = default;

  SendStartTask(Sequence seq, Tag tag, Time time, int sender, int receiver) :
    TaskCounted(TaskData{seq, tag, time, sender, receiver})
  {
    assert((sender != receiver) && "Sending to oneself is silly");
  }

  bool execute(Timeline &timeline, TaskQueue &tq) const override final;
};

class SendEndTask : public TaskCounted<SendEndTask>
{
  virtual TaskPriority task_priority() const { return TaskPriority::SEND_END; }
public:
  SendEndTask(const SendEndTask &other) = default;

  SendEndTask(Sequence seq, Tag tag, Time time, int sender, int receiver) :
    TaskCounted(TaskData{seq, tag, time, sender, receiver})
  {
  }

  bool execute(Timeline &timeline, TaskQueue &tq) const override final;
};

// This task is delivered, when a node can send once again. One does
// not have to schedule receives. See IdleTracker::deliver_task for
// more detail.
class IdleTask : public TaskCounted<IdleTask>
{
  virtual TaskPriority task_priority() const { return TaskPriority::IDLE; }
public:

  IdleTask(Sequence seq, Tag tag, Time time, int receiver, int sender) :
    TaskCounted(TaskData{seq, tag, time, receiver, sender})
  {
  }

  IdleTask(Sequence seq, Time time, int receiver) :
    IdleTask(seq, Tag::INTERNAL, time, receiver, receiver)
  {
  }

  IdleTask(Sequence seq, int receiver) :
    IdleTask(seq, Time(0), receiver)
  {
  }

  bool execute(Timeline &timeline, TaskQueue &tq) const override final;
};

class FinishTask : public TaskCounted<FinishTask>
{
  virtual TaskPriority task_priority() const { return TaskPriority::FINISH; }
public:

  FinishTask(Sequence seq, int receiver) :
    TaskCounted(TaskData{seq, Tag::INTERNAL, Time::max(), receiver, receiver})
  {
  }

  bool execute(Timeline &timeline, TaskQueue &tq) const override final;
};

class TimerTask : public TaskCounted<TimerTask>
{
public:

  TimerTask(Sequence seq, Tag tag, Time time, int sender) :
    TaskCounted(TaskData{seq, tag, time, sender, sender})
  {
  }

  TimerTask(Sequence seq, Time time, int sender) :
    TaskCounted(TaskData{seq, Tag::INTERNAL, time, sender, sender})
  {
  }

  bool execute(Timeline &timeline, TaskQueue &tq) const override final;
};

class InitTask : public TaskCounted<InitTask>
{
  virtual TaskPriority task_priority() const { return TaskPriority::INIT; }
public:

  InitTask(Sequence seq, Time time, int sender) :
    TaskCounted(TaskData{seq, Tag::INIT, time, sender, sender})
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
