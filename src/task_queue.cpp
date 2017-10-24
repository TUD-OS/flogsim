#include "task_queue.hpp"
#include "task.hpp"
#include "configuration.hpp"

void TaskQueue::schedule(std::shared_ptr<Task> task)
{
  if (!fault_injector->failure(task)) {
    queue.emplace(task);
  } else {
    queue.emplace(Task::make_from_task<LogP::FailureTask>(task.get()));
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


std::shared_ptr<Task> TaskQueue::pop()
{
  auto item = queue.top();
  queue.pop();
  progress(item->start());
  return item;
}
