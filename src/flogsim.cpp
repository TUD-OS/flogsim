#include <cstdio>
#include <cassert>

#include <memory>
#include <iostream>
#include <fstream>

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
    post_sends(task.receiver(), tq);
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

  auto fault_injector = FaultInjector::create();
  TaskQueue tq{std::move(fault_injector)};
  Timeline timeline(Model::get().P);

  BinaryBroadcast coll(Model::get().P);

  tq.run(coll, timeline);

  std::cout << "TotalRuntime," << timeline.get_total_time() << std::endl;

  auto [failed, finished, unreached] = timeline.node_stat();
  std::cout << "FailedNodes," << failed << std::endl
            << "FinishedNodes," << finished << std::endl
            << "UnreachedNodes," << unreached << std::endl;

  auto trace_filename = Configuration::get().log_prefix + ".trace.csv";
  std::ofstream trace_log(trace_filename);
  trace_log << timeline;

  auto model_filename = Configuration::get().log_prefix + ".model.csv";
  std::ofstream model_log(model_filename);
  model_log << LogP::Model::get();

  return 0;
}
