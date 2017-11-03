#include "task_queue.hpp"
#include "task.hpp"
#include "configuration.hpp"
#include "globals.hpp"

void TaskQueue::schedule(std::shared_ptr<Task> task)
{
  auto result = fault_injector->failure(task);
  if (result == Fault::OK) {
    queue.emplace(task);
  } else if (result == Fault::FAILURE) {
    queue.emplace(FailureTask::make_from_task(task.get()));
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

void TaskQueue::cancel_pending_sends(int node, Tag tag)
{
  std::deque<queue_t::handle_type> handles;
  for (auto it = queue.begin(); it != queue.end(); it++) {
    SendStartTask *task = dynamic_cast<SendStartTask *>(it->get());
    if (task == nullptr) {
      continue;
    }

    if ((task->tag() == tag) && (task->sender() == node)) {
      handles.push_back(queue_t::s_handle_from_iterator(it));
    }
  }

  for (auto &h : handles) {
    queue.erase(h);
  }
}

bool TaskQueue::now_idle(int node)
{
  auto cur_time = now();
  auto res = std::find_if(
    queue.ordered_begin(), queue.ordered_end(),
    [&](const auto &task)
    {
      if ((dynamic_cast<SendStartTask*>(task.get()) == nullptr) &&
          (dynamic_cast<RecvStartTask*>(task.get()) == nullptr)){
        return false;
      }

      if (task->sender() == node) {
        return true;
      }

      if (task->start() != cur_time) {
        return false;
      }
    });

  return ((res == queue.ordered_end()) ||
          ((*res)->start() != cur_time));
}

std::shared_ptr<Task> TaskQueue::pop()
{
  auto item = queue.top();
  queue.pop();
  progress(item->start());
  return item;
}
