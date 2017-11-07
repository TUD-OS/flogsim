#include "task_queue.hpp"
#include "task.hpp"
#include "configuration.hpp"
#include "globals.hpp"

void TaskQueue::schedule(std::unique_ptr<Task> task)
{
  if (dynamic_cast<IdleTask *>(task.get()) != nullptr) {
    // Ensure at most one Idle task per sender
    if (!has_idle[task->sender()]) {
      has_idle[task->sender()] = true;
    } else {
      return;
    }
  }

  auto result = fault_injector->failure(task.get());
  if (result == Fault::OK) {
    queue.emplace_back(std::move(task));
  } else if (result == Fault::FAILURE) {
    queue.emplace_back(FailureTask::make_from_task(task.get()));
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

  auto &conf = Globals::get().conf();
  while (!empty()) {
    if (conf.verbose) {
      std::cout << "Heap state:\n\t";
      std::cout << queue << std::endl;
    }
    std::unique_ptr<Task> task = pop();

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

std::unique_ptr<Task> TaskQueue::pop()
{
  auto item = queue.pop();

  if (dynamic_cast<FinishTask *>(item.get()) == nullptr) {
    progress(item->start());
  }
  return std::move(item);
}
