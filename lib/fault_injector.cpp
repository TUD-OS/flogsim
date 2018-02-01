#include <algorithm>
#include <random>
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

FaultInjector::FaultInjector()
{
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
  : FaultInjector(), P(Globals::get().model().P)
{}

ListFaults::ListFaults(const std::vector<int> &failed_nodes)
  : P(Globals::get().model().P),
    F(failed_nodes.size()),
    failed_nodes(failed_nodes)
{
  assert(failed_nodes.empty() ||
         (*std::max_element(failed_nodes.begin(), failed_nodes.end()) < P));
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

  switch (task->task_priority()) {
    case TaskPriority::RECEIVER:
    case TaskPriority::INIT:
      if (std::find(failed_nodes.begin(), failed_nodes.end(),
                    task->receiver()) != failed_nodes.end()) {
        if (conf.verbose) {
          std::cout << "Drop receive task " << *task << std::endl;
        }
        return Fault::FAILURE;
      }
      break;
    case TaskPriority::SENDER:
    case TaskPriority::FINISH:
      if (std::find(failed_nodes.begin(), failed_nodes.end(),
                    task->sender()) != failed_nodes.end()) {
        if (conf.verbose) {
          std::cout << "Drop " << task->type() << *task << std::endl;
        }
        return Fault::SKIP;
      }
      break;
    default:
      break;
  }
  return Fault::OK;
}

// UniformFaults

std::set<int>
UniformFaults::parse_robust_nodes(const std::string &robust_nodes_str)
{
  std::set<int> robust_nodes;
  std::stringstream ss(robust_nodes_str);

  int node_id;
  while (ss >> node_id) {
    char ch;
    ss >> ch;
    robust_nodes.insert(node_id);
  }
  return robust_nodes;
}

UniformFaults::UniformFaults()
  : ListFaults(),
    robust_nodes(parse_robust_nodes(Globals::get().conf().robust_nodes))
{
  F = Globals::get().conf().F;

  std::cout << robust_nodes.size() << std::endl;
  if (F + static_cast<int>(robust_nodes.size()) >= P) {
    throw std::invalid_argument("Can't simulate a collective. "
                                "There is at most one alive node.");
  }

  // Probably not the most efficient way, but the easiest way to
  // achive uniformity
  failed_nodes.reserve(P);
  for (int i = 0; i < P; i++) {
    if (robust_nodes.find(i) != robust_nodes.end()) {
      // The node is considered to be too robust to fail
      continue;
    }
    failed_nodes.push_back(i);
  }

  std::shuffle(failed_nodes.begin(), failed_nodes.end(),
               Globals::get().entropy().generator);

  failed_nodes.resize(F);

  if (Globals::get().conf().verbose) {
    std::cout << "Failed nodes: ";
    for (auto i : failed_nodes)
      std::cout << i;
    std::cout << std::endl;
  }
}
