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
  } else if (conf.results_format == "csv") {
    return std::make_unique<CsvPrinter>();
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

void CsvPrinter::intro()
{
  auto &conf = Globals::get().conf();
  std::cout << "L,o,g,P,k,COLL,parallel,prio,F,"
            << "TotalRuntime,FailedNodes,FinishedNodes,UnreachedNodes,"
            << "MsgTask,FaultInjectorSeed";

  if (conf.verbose) {
    std::cout << ",ReschRecvStartTask,ReschRecvEndTask,"
              << "ReschSendStartTask,ReschSendEndTask,ReschIdleTask";
  }

  std::cout << std::endl;
}

void CsvPrinter::results(Timeline &timeline, FaultInjector &faults)
{
  auto &conf = Globals::get().conf();
  auto &model = Globals::get().model();
  auto &entropy = Globals::get().entropy();

  auto [failed, finished, unreached] = timeline.node_stat();

  std::cout << model.L << ","
            << model.o << ","
            << model.g << ","
            << model.P << ","
            << conf.k << ","
            << conf.collective << ","
            << conf.parallelism << ","
            << conf.priority << ","
            << conf.F << ","
            << timeline.get_total_time() << ","
            << failed << ","
            << finished << ","
            << unreached << ","
            << MsgTask::issued() << ","
            << entropy.seed;

  if (conf.verbose) {
    std::cout << ","
              << RecvStartTask::reschedules() << ","
              << RecvEndTask::reschedules() << ","
              << SendStartTask::reschedules() << ","
              << SendEndTask::reschedules() << ","
              << IdleTask::reschedules();
  }

  std::cout << std::endl;
}
