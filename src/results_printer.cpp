#include <iostream>

#include "globals.hpp"
#include "task.hpp"
#include "fault_injector.hpp"
#include "timeline.hpp"
#include "results_printer.hpp"


void ResultsPrinter::print_header()
{
  auto &metrics = Globals::get().metrics();

  for (auto it = metrics.cbegin(); it != metrics.cend(); it++) {
    std::cout << "," << it->first;
  }
}

void ResultsPrinter::print_metrics()
{
  auto &metrics = Globals::get().metrics();

  for (auto it = metrics.cbegin(); it != metrics.cend(); it++) {
    std::cout << "," << it->second;
  }
}

std::unique_ptr<ResultsPrinter> ResultsPrinter::create()
{
  auto &conf = Globals::get().conf();

  if (conf.results_format == "table") {
    return std::make_unique<TablePrinter>();
  } else if (conf.results_format == "csv") {
    return std::make_unique<CsvPrinter>();
  } else if (conf.results_format == "csv-id") {
    return std::make_unique<CsvIdPrinter>();
  } else {
    throw std::invalid_argument("Desired format does not exist:" +
                                conf.results_format);
  }
}

void TablePrinter::print_metrics()
{
  auto &metrics = Globals::get().metrics();

  for (auto it = metrics.cbegin(); it != metrics.cend(); it++) {
    std::cout << it->first
              << "," << it->second
              << std::endl;
  }
}

void TablePrinter::results(Timeline &timeline, FaultInjector &faults)
{
  auto &conf = Globals::get().conf();
  auto &entropy = Globals::get().entropy();

  auto [failed, finished, unreached] = timeline.node_stat();
  std::cout << "FailedNodes," << failed << std::endl
            << "FinishedNodes," << finished << std::endl
            << "UnreachedNodes," << unreached << std::endl
            << "MsgTask," << MsgTask::issued() << std::endl
            << "FaultInjectorSeed," << entropy.get_seed() << std::endl
            << "FailedNodeList," << faults << std::endl;

  print_metrics();

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
  std::cout << "L,o,g,P,k,d,COLL,parallel,prio,F,"
            << "FailedNodes,FinishedNodes,UnreachedNodes,"
            << "MsgTask,FaultInjectorSeed";

  print_header();

  if (conf.verbose) {
    std::cout << ",ReschRecvStartTask,ReschRecvEndTask,"
              << "ReschSendStartTask,ReschSendEndTask,ReschIdleTask";
  }

  std::cout << std::endl;
}

void CsvPrinter::results(Timeline &timeline, FaultInjector &)
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
            << conf.d << ","
            << conf.collective << ","
            << conf.parallelism << ","
            << conf.priority << ","
            << conf.F << ","
            << failed << ","
            << finished << ","
            << unreached << ","
            << MsgTask::issued() << ","
            << entropy.get_seed();

  print_metrics();

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

void CsvIdPrinter::intro()
{
  auto &conf = Globals::get().conf();
  std::cout << "id,"
            << "FailedNodes,FinishedNodes,UnreachedNodes,"
            << "MsgTask,FaultInjectorSeed";

  print_header();

  if (conf.verbose) {
    std::cout << ",ReschRecvStartTask,ReschRecvEndTask,"
              << "ReschSendStartTask,ReschSendEndTask,ReschIdleTask";
  }

  std::cout << std::endl;
}

void CsvIdPrinter::results(Timeline &timeline, FaultInjector &)
{
  auto &conf = Globals::get().conf();
  auto &entropy = Globals::get().entropy();

  auto [failed, finished, unreached] = timeline.node_stat();

  std::cout << conf.id << ","
            << failed << ","
            << finished << ","
            << unreached << ","
            << MsgTask::issued() << ","
            << entropy.get_seed();

  print_metrics();

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
