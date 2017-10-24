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
  typedef std::shared_ptr<Task> queue_item_t;

  struct QueueItemCompare
  {
    bool operator()(const queue_item_t &first, const queue_item_t &second) const
    {
      return second->operator<(*first);
    }
  };
  boost::heap::fibonacci_heap<queue_item_t,
                      boost::heap::compare<QueueItemCompare>> queue;

  Time current_time;

  void progress(Time absolute)
  {
    assert(!(absolute < current_time));
    current_time = absolute;
  }

  std::unique_ptr<FaultInjector> fault_injector;
public:

  TaskQueue(std::unique_ptr<FaultInjector> fault_injector) :
    queue(), fault_injector(std::move(fault_injector))
  {}

  Time now() const
  {
    return current_time;
  }

  std::shared_ptr<Task> pop();

  bool empty() const
  {
    return queue.empty();
  }

  void schedule(std::shared_ptr<Task> task);

  void run(Collective &coll, Timeline &timeline);
};
