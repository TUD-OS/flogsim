#pragma once

#include <memory>
#include <queue>
#include <cassert>

#include "time.hpp"
#include "timeline.hpp"
#include "fault_injector.hpp"
#include "task.hpp"

#include <boost/heap/pairing_heap.hpp>

class Collective;
class FaultInjector;

class TaskQueue
{
  struct Queue
  {
    typedef Task* queue_item_t;

    struct Compare
    {
      bool operator()(const queue_item_t &first, const queue_item_t &second) const
      {
        return (*second) < (*first);
      }
    };

    typedef boost::heap::pairing_heap<queue_item_t,
                                      boost::heap::compare<Compare>> queue_t;

    bool empty() const
    {
      return items.empty();
    }

    void emplace_back(std::unique_ptr<Task> task)
    {
      Task *ptr = task.release();
      items.push(ptr);
    }

    auto pop()
    {
      std::unique_ptr<Task> item(items.top());
      items.pop();
      return std::move(item);
    }

    friend std::ostream &operator<<(std::ostream &os, const Queue &q)
    {
      // This is slow, but verbose is supposed to be used only for
      // small sizes anyway
      for (auto &task : q.items) {
        os << *task << "\n\t";
      }
      return os;
    }

    const auto &top() const
    {
      return *items.top();
    }

    Queue()
    {
      // items.reserve(Globals::get().model().P);
    }

    ~Queue()
    {
    }
  private:
    queue_t items;
  };

  Queue queue;

  Time current_time;

  bool is_next_timestamp(Time time) const
  {
    if (queue.empty()) {
      return true;
    }
    // return time != queue.top().start();
    return time != queue.top().start();
  }

  struct IdleTracker
  {
    // The idea of this class is to be proxy for IdleTasks. The
    // problem with putting IdleTasks on the queue as they are is huge
    // number of rescheduling events they generate. This slows down
    // overall progress a lot.
    //
    // So we created a proxy class which tracks requests for IdleTask
    // creation and puts them onto the queue exactly before the
    // progress would move forward in time.
    void deliver_tasks(TaskQueue &, Timeline &);
    void prepare_next_timestamp();


    bool is_pending()
    {
      return count > 0;
    }
    void mark_nonidle(int node)
    {
      was_idle[node]--;
    }

    int threads;
    int count;
    // Count how many threads were actually idling
    std::vector<int> was_idle;
    // Pending idle task
    std::vector<bool> pending;

    IdleTracker(int P, int threads)
      : threads(threads),
        count(0),
        was_idle(P, threads),
        pending(P)
    {}
  };

  IdleTracker idle;

  void progress(Time absolute)
  {
    assert(!(absolute < current_time) && "Let's not do the timewarp again");
    if (absolute != current_time) {
      idle.prepare_next_timestamp();
    }
    current_time = absolute;
  }

  Timeline *timeline_ptr;
  FaultInjector *fault_injector;
public:
  void mark_nonidle(int node)
  {
    idle.mark_nonidle(node);
  }

  const Timeline &timeline() const
  {
    return *timeline_ptr;
  }

  const FaultInjector &faults() const
  {
    return *fault_injector;
  }

  TaskQueue(FaultInjector *fault_injector)
    : queue(),
      idle(Globals::get().model().P, Globals::get().model().parallelism),
      fault_injector(fault_injector)
  {}

  Time now() const
  {
    return current_time;
  }

  std::unique_ptr<Task> pop();

  bool empty() const
  {
    return queue.empty();
  }

  void schedule(std::unique_ptr<Task> task);

  void run(Collective &coll, Timeline &timeline);
};
