#include <algorithm>
#include <random>
#include <chrono>
#include <ctime>        // std::time
#include <stdexcept>
#include <iterator>
#include <regex>
#include <sstream>

#include "fault_injector.hpp"
#include "task.hpp"
#include "configuration.hpp"
#include "globals.hpp"

// FaultInjector

std::unique_ptr<FaultInjector> FaultInjector::create()
{
  auto &conf = Globals::get().conf();
  if (NoFaults::match(conf.fault_injector)) {
    return std::make_unique<NoFaults>();
  } else if (UniformFaults::match(conf.fault_injector)) {
    return std::make_unique<UniformFaults>();
  } else if (ExplicitListFaults::match(conf.fault_injector)) {
    return std::make_unique<ExplicitListFaults>();
  } else {
    throw std::invalid_argument("Fault injector does not exist:" +
                                conf.fault_injector);
  }
}

// ExplicitListFaults

ExplicitListFaults::ExplicitListFaults()
{
  if (Globals::get().conf().F) {
    throw std::invalid_argument("Don't provide F in case of explicit"
                                " list of faulty nodes.");
  }

  auto &conf = Globals::get().conf();

  std::stringstream ss(conf.fault_injector);

  int node_id;
  while (ss >> node_id) {
    char ch;
    ss >> ch;
    failed_nodes.push_back(node_id);
  }
  F = failed_nodes.size();
}

bool ExplicitListFaults::match(const std::string &fault_injector)
{
  std::regex list_regex("^\\d+(,\\d+)*$");
  return std::regex_match(fault_injector, list_regex);
}

// ListFaults

ListFaults::ListFaults()
  : P(Globals::get().model().P)
{}

ListFaults::ListFaults(const std::vector<int> &failed_nodes)
  : P(Globals::get().model().P),
    F(failed_nodes.size()),
    failed_nodes(failed_nodes)
{
  assert(*std::max_element(failed_nodes.begin(), failed_nodes.end()) < P);
}

void ListFaults::print(std::ostream &os) const
{
  if (failed_nodes.empty()) {
    os << "none";
    return;
  }

  for (unsigned i = 0; i < failed_nodes.size() - 1; i++) {
    os << failed_nodes[i] << ",";
  }

  os << failed_nodes[failed_nodes.size() - 1];
}

Fault ListFaults::failure(Task *task)
{
  auto &conf = Globals::get().conf();
  if ((dynamic_cast<RecvStartTask*>(task) != nullptr) ||
      (dynamic_cast<InitTask*>(task) != nullptr)) {
    if (std::find(failed_nodes.begin(), failed_nodes.end(),
                  task->receiver()) != failed_nodes.end()) {
      if (conf.verbose) {
        std::cout << "Drop receive task " << *task << std::endl;
      }
      return Fault::FAILURE;
    }
  } else if ((dynamic_cast<SendStartTask*>(task) != nullptr) ||
             (dynamic_cast<FinishTask*>(task) != nullptr)) {
    if (std::find(failed_nodes.begin(), failed_nodes.end(),
                  task->sender()) != failed_nodes.end()) {
      if (conf.verbose) {
        std::cout << "Drop " << task->type() << *task << std::endl;
      }
      return Fault::SKIP;
    }
  }
  return Fault::OK;
}

// UniformFaults

UniformFaults::UniformFaults()
{
  F = Globals::get().conf().F;
  // Probably not the most efficient way, but the easiest way to
  // achive uniformity
  failed_nodes.reserve(P);
  for (int i = 0; i < P; i++) {
    failed_nodes.push_back(i);
  }

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

  std::shuffle(failed_nodes.begin(), failed_nodes.end(),
               std::default_random_engine(seed));

  failed_nodes.resize(F);

  if (Globals::get().conf().verbose) {
    std::cout << "Failed nodes: ";
    for (auto i : failed_nodes)
      std::cout << i;
    std::cout << std::endl;
  }
}
