#include "task_queue.hpp"
#include "task.hpp"
#include "configuration.hpp"

void TaskQueue::schedule(std::shared_ptr<Task> task)
{
  queue.emplace(task->start(), task);
}

void TaskQueue::run(Collective &coll, Timeline &timeline)
{
  coll.populate(*this);

  while (!empty()) {
    std::shared_ptr<Task> task = pop();

    if (Configuration::get().verbose) {
      std::cout << "NOW=" << now() << " " << *task;
    }
    if (task->execute(timeline, *this)) {
      task->notify(coll, *this);
      if (Configuration::get().verbose) {
        std::cout << std::endl;
      }
    } else {
      if (Configuration::get().verbose) {
        std::cout << " rescheduled" << std::endl;
      }
    }
  }
}
