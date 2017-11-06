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
  typedef std::unique_ptr<Task> queue_item_t;

  static bool queue_item_compare(const queue_item_t &first, const queue_item_t &second)
  {
    return (*second) < (*first);
  }

  typedef std::vector<queue_item_t> queue_t;

  queue_t queue;

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
