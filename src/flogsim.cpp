#include <cstdio>
#include <cassert>

#include <memory>
#include <iostream>
#include <fstream>

#include "event.hpp"
#include "model.hpp"
#include "task.hpp"
#include "task_queue.hpp"
#include "configuration.hpp"
#include "fault_injector.hpp"

class TaskQueue;
class Collective;

using LogP::Model;

int main(int argc, char *argv[])
{
  Configuration::parse_args(argc, argv);

  auto fault_injector = FaultInjector::create();
  auto coll = CollectiveRegistry::create(Configuration::get().collective);
  TaskQueue tq{std::move(fault_injector)};
  Timeline timeline(Model::get().P);

  tq.run(*coll, timeline);

  std::cout << "TotalRuntime," << timeline.get_total_time() << std::endl;

  auto [failed, finished, unreached] = timeline.node_stat();
  std::cout << "FailedNodes," << failed << std::endl
            << "FinishedNodes," << finished << std::endl
            << "UnreachedNodes," << unreached << std::endl
            << "MsgTask," << MsgTask::issued() << std::endl
            << "FailedNodeList," << tq.faults() <<std::endl;

  auto trace_filename = Configuration::get().log_prefix + ".trace.csv";
  std::ofstream trace_log(trace_filename);
  trace_log << timeline;

  auto model_filename = Configuration::get().log_prefix + ".model.csv";
  std::ofstream model_log(model_filename);
  model_log << LogP::Model::get();

  return 0;
}
