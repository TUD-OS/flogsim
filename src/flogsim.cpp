#include <cstdio>
#include <cassert>
#include <exception>

#include <memory>
#include <iostream>
#include <fstream>

#include "event.hpp"
#include "model.hpp"
#include "task.hpp"
#include "task_queue.hpp"
#include "globals.hpp"

#include "configuration_args.hpp"

class TaskQueue;
class Collective;

int main(int argc, char *argv[])
{
  ConfigurationArgs conf(argc, argv);
  Model model(conf);
  Globals::set({&conf, &model});
  auto faults = FaultInjector::create();
  Timeline timeline;

  try {
    auto coll = Collective({0}, faults.get());

    coll.run(timeline, CollectiveRegistry::create(coll.reached_nodes));

    std::cout << "TotalRuntime," << timeline.get_total_time() << std::endl;

    auto [failed, finished, unreached] = timeline.node_stat();
    std::cout << "FailedNodes," << failed << std::endl
              << "FinishedNodes," << finished << std::endl
              << "UnreachedNodes," << unreached << std::endl
              << "MsgTask," << MsgTask::issued() << std::endl
              << "FaultInjectorSeed," << coll.faults->seed() << std::endl
              << "FailedNodeList," << *coll.faults << std::endl;

    if (conf.verbose) {
      std::cout << "ReschRecvStartTask," << RecvStartTask::reschedules() << std::endl
                << "ReschRecvEndTask," << RecvEndTask::reschedules() << std::endl
                << "ReschSendStartTask," << SendStartTask::reschedules() << std::endl
                << "ReschSendEndTask," << SendEndTask::reschedules() << std::endl
                << "ReschIdleTask," << IdleTask::reschedules() << std::endl;
    }

  } catch (const std::exception &e) {
    std::cerr << "Caught an exception: " << e.what() << std::endl;
    std::cerr << "Initial environment: " << conf << std::endl;
    std::cerr << "Faults: " << *faults << std::endl;
  }

  auto trace_filename = conf.log_prefix + ".trace.csv";
  std::ofstream trace_log(trace_filename);
  trace_log << timeline;

  auto model_filename = conf.log_prefix + ".model.csv";
  std::ofstream model_log(model_filename);
  model_log << model;

  return 0;
}
