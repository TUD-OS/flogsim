#include "task_queue.hpp"
#include "task.hpp"
#include "configuration.hpp"

void TaskQueue::schedule(std::shared_ptr<Task> task)
{
  if (!fault_injector->failure(task)) {
    queue.emplace(task);
  } else {
    queue.emplace(LogP::FailureTask::make_from_task(task.get()));
  }
}

std::ostream &operator<<(std::ostream &os, std::shared_ptr<Task> t)
{
  os << *t;
  return os;
}

void TaskQueue::run(Collective &coll, Timeline &timeline)
{
  coll.populate(*this);

  auto &conf = Configuration::get();
  while (!empty()) {
    if (conf.verbose) {
      std::cout << "Heap state:\n\t";
      std::copy(queue.ordered_begin(), queue.ordered_end(),
                std::ostream_iterator<std::shared_ptr<Task>>(std::cout, "\n\t"));
      std::cout << std::endl;
    }
    std::shared_ptr<Task> task = pop();

    std::stringstream ss;
    if (conf.verbose) {
      ss << "NOW=" << now() << " " << *task;
    }
    if (task->execute(timeline, *this)) {
      task->notify(coll, *this);
      if (conf.verbose) {
        std::cout << ss.str() << std::endl;
      }
    } else {
      if (conf.verbose) {
        std::cout << ss.str() << " rescheduled" << std::endl;
      }
    }

    if (now() >= Time(conf.limit)) {
      std::cout << "Time limit exceeded. Now stop." << std::endl;
      break;
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
