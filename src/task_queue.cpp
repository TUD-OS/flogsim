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
      printf("[%d] Task %s %" PRIu64 " @ node %d", now(), task->type(), task->start(), task->get_node());
    }
    if (task->execute(timeline, *this)) {
      task->notify(coll, *this);
      if (Configuration::get().verbose) {
        printf("\n");
      }
    } else {
      if (Configuration::get().verbose) {
        printf(" rescheduled\n");
      }
    }
  }
}
