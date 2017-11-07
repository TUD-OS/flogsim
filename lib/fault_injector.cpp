#include <algorithm>
#include <ctime>        // std::time
#include <stdexcept>
#include <iterator>

#include "fault_injector.hpp"
#include "task.hpp"
#include "configuration.hpp"
#include "globals.hpp"

std::unique_ptr<FaultInjector> FaultInjector::create()
{
  auto &conf = Globals::get().conf();
  if (conf.fault_injector == "none") {
    return std::make_unique<NoFaults>();
  } else if (conf.fault_injector == "uniform") {
    return std::make_unique<UniformFaults>();
  } else {
    throw std::invalid_argument("Fault injector does not exist:" +
                                conf.fault_injector);
  }
}

UniformFaults::UniformFaults()
  : P(Globals::get().model().P),
    F(Globals::get().conf().F)
{
  std::srand(unsigned(std::time(0)));

  // Probably not the most efficient way, but the easiest way to
  // achive uniformity
  failed_nodes.reserve(P);
  for (int i = 0; i < P; i++) {
    failed_nodes.push_back(i);
  }

  std::random_shuffle(failed_nodes.begin(), failed_nodes.end(),
                      [&] (int i) {
                        return std::rand() % i;
                      });

  failed_nodes.resize(F);

  if (Globals::get().conf().verbose) {
    std::cout << "Failed nodes: ";
    for (auto i : failed_nodes)
      std::cout << i;
    std::cout << std::endl;
  }
}

UniformFaults::UniformFaults(const std::vector<int> &failed_nodes)
  : P(Globals::get().model().P),
    F(failed_nodes.size()),
    failed_nodes(failed_nodes)
{
  assert(*std::max_element(failed_nodes.begin(), failed_nodes.end()) < P);
}

void UniformFaults::print(std::ostream &os) const
{
  std::copy(failed_nodes.begin(), failed_nodes.end(),
            std::ostream_iterator<int>(os, " "));
}

Fault UniformFaults::failure(Task *task)
{
  auto &conf = Globals::get().conf();

  if (std::find(failed_nodes.begin(), failed_nodes.end(),
                task->sender()) != failed_nodes.end()) {
    if (dynamic_cast<RecvStartTask*>(task) != nullptr) {
      if (conf.verbose) {
        std::cout << "Drop receive task " << *task << std::endl;
      }
      return Fault::FAILURE;
    }
    else {
      if (conf.verbose) {
        std::cout << "Drop " << task->type() << *task << std::endl;
      }
      return Fault::SKIP;
    }
  }
  return Fault::OK;
}
