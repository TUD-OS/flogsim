#include "task_queue.hpp"
#include "task.hpp"

void TaskQueue::schedule(std::shared_ptr<Task> task)
{
  queue.emplace(task->start(), task);
}
