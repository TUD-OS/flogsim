#include "task_queue.hpp"
#include "task.hpp"
#include "configuration.hpp"

void TaskQueue::schedule(std::shared_ptr<Task> task)
{
  if (!fault_injector.failure(task)) {
    queue.emplace(task->start(), task);
  } else {
    queue.emplace(task->start(), std::make_shared<LogP::FailureTask>(task));
  }
}

void TaskQueue::run(Collective &coll, Timeline &timeline)
{
  coll.populate(*this);

  while (!empty()) {
    std::shared_ptr<Task> task = pop();

    std::stringstream ss;
    if (Configuration::get().verbose) {
      ss << "NOW=" << now() << " " << *task;
    }
    if (task->execute(timeline, *this)) {
      task->notify(coll, *this);
      if (Configuration::get().verbose) {
        std::cout << ss.str() << std::endl;
      }
    } else {
      if (Configuration::get().verbose) {
        std::cout << ss.str() << " rescheduled" << std::endl;
      }
    }
  }
}
