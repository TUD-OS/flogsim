#pragma once

#include <memory>
#include <queue>
#include <cassert>

#include "time.hpp"
#include "timeline.hpp"

#include <boost/heap/fibonacci_heap.hpp>

class Collective;

class Task;

class TaskQueue
{
  struct QueueItem
  {
    Time time;
    std::shared_ptr<Task> task;

    QueueItem() = default;

    QueueItem(const QueueItem &other)
    {
      time = other.time;
      task = other.task;
    }

    QueueItem(Time time, std::shared_ptr<Task> task) :
      time(time), task(task)
    {}

    // Take time with the minimum value. By default priority_queue
    // puts the maximum on the top.
    bool operator<(const QueueItem &other) const
    {
      return time > other.time;
    }
  };

  std::priority_queue<QueueItem> queue;

  Time current_time;

  void progress(Time absolute)
  {
    assert(!(absolute < current_time));
    current_time = absolute;
  }
public:

  TaskQueue() :
    queue()
  {}

  Time now() const
  {
    return current_time;
  }

  std::shared_ptr<Task> pop()
  {
    auto item = queue.top();
    queue.pop();
    progress(item.time);
    return item.task;
  }

  bool empty() const
  {
    return queue.empty();
  }

  void schedule(std::shared_ptr<Task> task);

  void run(Collective &coll, Timeline &timeline);
};
