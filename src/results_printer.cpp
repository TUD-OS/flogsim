#include <iostream>

#include "globals.hpp"
#include "task.hpp"
#include "fault_injector.hpp"
#include "timeline.hpp"
#include "results_printer.hpp"

std::unique_ptr<ResultsPrinter> ResultsPrinter::create()
{
  auto &conf = Globals::get().conf();

  if (conf.results_format == "table") {
    return std::make_unique<TablePrinter>();
  } else {
    throw std::invalid_argument("Desired format does not exist:" +
                                conf.results_format);
  }
}

void TablePrinter::results(Timeline &timeline, FaultInjector &faults)
{
  auto &conf = Globals::get().conf();
  auto &entropy = Globals::get().entropy();

  std::cout << "TotalRuntime," << timeline.get_total_time() << std::endl;

  auto [failed, finished, unreached] = timeline.node_stat();
  std::cout << "FailedNodes," << failed << std::endl
            << "FinishedNodes," << finished << std::endl
            << "UnreachedNodes," << unreached << std::endl
            << "MsgTask," << MsgTask::issued() << std::endl
            << "FaultInjectorSeed," << entropy.seed << std::endl
            << "FailedNodeList," << faults << std::endl;

  if (conf.verbose) {
    std::cout << "ReschRecvStartTask," << RecvStartTask::reschedules() << std::endl
              << "ReschRecvEndTask," << RecvEndTask::reschedules() << std::endl
              << "ReschSendStartTask," << SendStartTask::reschedules() << std::endl
              << "ReschSendEndTask," << SendEndTask::reschedules() << std::endl
              << "ReschIdleTask," << IdleTask::reschedules() << std::endl;
  }
}
