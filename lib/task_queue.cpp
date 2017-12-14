#include "task_queue.hpp"
#include "task.hpp"
#include "configuration.hpp"
#include "globals.hpp"

void TaskQueue::schedule(std::unique_ptr<Task> task)
{
  if (dynamic_cast<IdleTask *>(task.get()) != nullptr) {
    if (!idle.pending[task->sender()]) {
      idle.count ++;
    }
    // Mark that a node wants to post an idle task
    idle.pending[task->sender()] = true;
    return;
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
  auto &conf = Globals::get().conf();
  while (!empty() || idle.is_pending()) {
    if (conf.verbose) {
      std::cout << "Heap state:\n\t";
      std::cout << queue << std::endl;
    }

    auto remember_now = now();
    if (is_next_timestamp(remember_now)) {
      idle.deliver_tasks(*this, timeline);
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
  assert(!queue.empty() && "Queue is empty, but there is an attempt to pop.");
  auto item = queue.pop();

  if (dynamic_cast<FinishTask *>(item.get()) == nullptr) {
    progress(item->start());
  }
  return std::move(item);
}

void TaskQueue::IdleTracker::deliver_tasks(TaskQueue &tq, Timeline &tl)
{
  for (unsigned node = 0; node < pending.size(); node++) {
    if (!was_idle[node] || !pending[node]) {
      // In this timestamp the core wasn't idling
      continue;
    }

    Time est = tl.per_cpu_time[node].cpu_events.earliest_start_time();
    if (est > tq.current_time) {
      // Something is running right now
      continue;
    }

    tq.queue.emplace_back(IdleTask::make_new(tq.now(), node));
    if (pending[node]) {
      count --;
    }
    pending[node] = false;
  }
}

void TaskQueue::IdleTracker::prepare_next_timestamp()
{
  std::fill(was_idle.begin(), was_idle.end(), threads);
}
