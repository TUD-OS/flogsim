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
#include "tree_phase.hpp"
#include "correction_phase.hpp"
#include "combiner_phase.hpp"
#include "exclusive_phase.hpp"
#include "gossip_phase.hpp"
#include "globals.hpp"

#include "configuration_args.hpp"

class TaskQueue;
class Collective;

int main(int argc, char *argv[])
{
  try {
    ConfigurationArgs conf(argc, argv);

    Model model(conf);
    Globals::set({&conf, &model});

    auto coll = Collective({0});

    auto &rn = coll.reached_nodes;

    // Here you build a collective
    std::vector<std::unique_ptr<int>> v {int{4}};
    std::vector<std::unique_ptr<Phase>> phases;

    std::unique_ptr<Phase> tree = std::make_unique<KAryTreePhase<true>>(rn);
    phases.push_back(std::make_unique<ExclusivePhase>(rn, std::move(tree)));
    phases.push_back(std::make_unique<OpportunisticCorrectionPhase<true>>(rn));
    phases.push_back(std::make_unique<ExclusivePhase>(rn,
                                                      std::make_unique<GossipPhase>(rn)));
    phases.push_back(std::make_unique<CheckedCorrectionPhase<true>>(rn));

    std::unique_ptr<Phase> full = std::make_unique<CombinerPhase>(coll.reached_nodes, std::move(phases));

    auto timeline = coll.run(std::move(full));

    std::cout << "TotalRuntime," << timeline.get_total_time() << std::endl;

    auto [failed, finished, unreached] = timeline.node_stat();
    std::cout << "FailedNodes," << failed << std::endl
              << "FinishedNodes," << finished << std::endl
              << "UnreachedNodes," << unreached << std::endl
              << "MsgTask," << MsgTask::issued() << std::endl
              << "FailedNodeList," << *coll.faults << std::endl;

    if (conf.verbose) {
      std::cout << "ReschRecvStartTask," << RecvStartTask::reschedules() << std::endl
                << "ReschRecvEndTask," << RecvEndTask::reschedules() << std::endl
                << "ReschSendStartTask," << SendStartTask::reschedules() << std::endl
                << "ReschSendEndTask," << SendEndTask::reschedules() << std::endl
                << "ReschIdleTask," << IdleTask::reschedules() << std::endl;
    }

    auto trace_filename = conf.log_prefix + ".trace.csv";
    std::ofstream trace_log(trace_filename);
    trace_log << timeline;

    auto model_filename = conf.log_prefix + ".model.csv";
    std::ofstream model_log(model_filename);
    model_log << model;
  } catch (const std::exception &e) {
    std::cerr << "Caught an exception: " << e.what() << std::endl;
  }

  return 0;
}
