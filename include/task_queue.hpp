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

  private:
    item_t items;
  };
  Queue queue;

  Time current_time;

  void progress(Time absolute)
  {
    assert(!(absolute < current_time) && "Let's not do the timewarp again");
    current_time = absolute;
  }

  FaultInjector *fault_injector;
public:
  std::vector<bool> has_idle;

  const FaultInjector &faults() const
  {
    return *fault_injector;
  }

  TaskQueue(FaultInjector *fault_injector)
    : queue(),
      fault_injector(std::move(fault_injector)),
      has_idle(Globals::get().model().P)
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
