#include <cstdio>
#include <cassert>

#include <memory>
#include <iostream>

#include "event.hpp"
#include "model.hpp"
#include "task.hpp"
#include "configuration.hpp"

class TaskQueue;
class Collective;

using LogP::Model;

class BinaryBroadcast : public Collective
{
  int nodes;
  void post_sends(int sender, TaskQueue &tq)
  {
    for (int i = 1; i <= 2; i++) {
      int recv = 2 * sender + i;
      if (recv < nodes) {
        tq.schedule(std::make_shared<LogP::SendTask>(tq.now(), sender, recv));
      }
    }
    tq.schedule(std::make_shared<LogP::FinishTask>(sender));
  }
public:
  BinaryBroadcast(int nodes)
    : nodes(nodes)
  {
  }

  virtual void accept(const LogP::RecvTask& task, TaskQueue& tq)
  {
    post_sends(task.get_node(), tq);
  }

  void populate(TaskQueue &tq) override
  {
    int root = 0;
    post_sends(root, tq);
  }
};

int main(int argc, char *argv[])
{
  Configuration::parse_args(argc, argv);

  TaskQueue tq{};
  Timeline timeline(Model::get().P);

  BinaryBroadcast coll(Model::get().P);
  coll.populate(tq);

  while (!tq.empty()) {
    std::shared_ptr<Task> task = tq.pop();

    if (Configuration::get().verbose) {
      printf("[%d] Task %s %" PRIu64 " @ node %d", tq.now(), task->type(), task->start(), task->get_node());
    }
    if (task->execute(timeline, tq)) {
      task->notify(coll, tq);
      if (Configuration::get().verbose) {
        printf("\n");
      }
    } else {
      if (Configuration::get().verbose) {
        printf(" rescheduled\n");
      }
    }
  }

  std::cout << timeline << std::endl;
  std::cout << LogP::Model::get() << std::endl;
  return 0;
}
