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

std::string ResultsPrinter::metric_value(const std::string &key)
{
  auto &metrics = Globals::get().metrics();

  auto result = std::find_if(metrics.cbegin(), metrics.cend(), [&](const auto &arg) {
      return arg.first == key;
    });
  // Key not found -> return empty string
  if (result == metrics.cend()) {
    return "";
  }

  std::stringstream ss;
  ss << result->second;
  return ss.str();
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
  } else if (conf.results_format == "csv-columns") {
    return std::make_unique<CsvColumnsPrinter>();
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

std::vector<std::string> parse_header(const std::string &header_unparsed)
{
  std::vector<std::string> result;
  std::string token;
  std::string delimeter = ",";
  std::istringstream ss(header_unparsed);
  while (std::getline(ss, token, ',')) {
    result.push_back(token);
  }

  return result;
}

CsvColumnsPrinter::CsvColumnsPrinter() :
  header(parse_header(Globals::get().conf().header))
{
  if (header.size() < 1) {
    throw std::runtime_error("Header is empty or parsed incorrectly");
  }
}

void CsvColumnsPrinter::intro()
{
  for (unsigned i = 0; i < header.size(); i++) {
    if (i > 0) {
      std::cout << ",";
    }

    std::cout << header[i];
  }

  std::cout << std::endl;
}

void CsvColumnsPrinter::results(Timeline &timeline, FaultInjector &)
{
  auto &conf = Globals::get().conf();
  auto &model = Globals::get().model();
  auto &entropy = Globals::get().entropy();

  auto [failed, finished, unreached] = timeline.node_stat();

  for (unsigned i = 0; i < header.size(); i++) {
    if (i > 0) {
      std::cout << ",";
    }

    if (header[i] == "L") {
      std::cout << model.L;
    } else if (header[i] == "o") {
      std::cout << model.o;
    } else if (header[i] == "g") {
      std::cout << model.g;
    } else if (header[i] == "P") {
      std::cout << model.P;
    } else if (header[i] == "k") {
      std::cout << conf.k;
    } else if (header[i] == "d") {
      std::cout << conf.d;
    } else if (header[i] == "COLL") {
      std::cout << conf.collective;
    } else if (header[i] == "parallel") {
      std::cout << conf.parallelism;
    } else if (header[i] == "prio") {
      std::cout << conf.priority;
    } else if (header[i] == "F") {
      std::cout << conf.F;
    } else if (header[i] == "FailedNodes") {
      std::cout << failed;
    } else if (header[i] == "FinishedNodes") {
      std::cout << finished;
    } else if (header[i] == "UnreachedNodes") {
      std::cout << unreached;
    } else if (header[i] == "MsgTask") {
      std::cout << MsgTask::issued();
    } else if (header[i] == "FaultInjectorSeed") {
      std::cout << entropy.get_seed();
    } else if (header[i] == "ReschRecvStartTask") {
      std::cout << RecvStartTask::reschedules();
    } else if (header[i] == "ReschRecvEndTask") {
      std::cout << RecvEndTask::reschedules();
    } else if (header[i] == "ReschSendStartTask") {
      std::cout << SendStartTask::reschedules();
    } else if (header[i] == "ReschSendEndTask") {
      std::cout << SendEndTask::reschedules();
    } else if (header[i] == "ReschIdleTask") {
      std::cout << IdleTask::reschedules();
    } else {
      // Maybe it is an algorithm specific known metric
      std::cout << metric_value(header[i]);
    }
  }

  std::cout << std::endl;
}
