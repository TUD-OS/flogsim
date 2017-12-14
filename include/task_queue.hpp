#pragma once

#include <memory>
#include <queue>
#include <cassert>

#include "time.hpp"
#include "timeline.hpp"
#include "fault_injector.hpp"
#include "task.hpp"

#include <boost/heap/fibonacci_heap.hpp>

class Collective;
class FaultInjector;

class TaskQueue
{
  struct Queue
  {
    typedef std::unique_ptr<Task> queue_item_t;
    typedef std::vector<queue_item_t> item_t;

    static bool queue_item_compare(const queue_item_t &first, const queue_item_t &second)
    {
      return (*second) < (*first);
    }

    bool empty() const
    {
      return items.empty();
    }

    template<class... Args>
    void emplace_back(Args&&... args)
    {
      items.emplace_back(std::forward<Args>(args)...);
      std::push_heap(items.begin(), items.end(), queue_item_compare);
    }

    auto pop()
    {
      std::pop_heap(items.begin(), items.end(), queue_item_compare);
      std::unique_ptr<Task> item = std::move(items.back());
      items.pop_back();
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
      return *items.front().get();
    }
  private:
    item_t items;
  };
  Queue queue;

  Time current_time;

  bool is_next_timestamp(Time time) const
  {
    if (queue.empty()) {
      return true;
    }
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

  FaultInjector *fault_injector;
public:
  void mark_nonidle(int node)
  {
    idle.mark_nonidle(node);
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
