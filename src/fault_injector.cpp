#include <algorithm>
#include <ctime>        // std::time
#include <stdexcept>

#include "fault_injector.hpp"
#include "task.hpp"

std::unique_ptr<FaultInjector> FaultInjector::create()
{
  auto &conf = Configuration::get();

  if (conf.fault_injector == "none") {
    return std::make_unique<NoFaults>();
  } else if (conf.fault_injector == "uniform") {
    return std::make_unique<UniformFaults>(conf.F);
  } else {
    throw std::invalid_argument("Fault injector does not exist:" +
                                conf.fault_injector);
  }
}

UniformFaults::UniformFaults(int F)
  : P(Configuration::get().P), F(F)
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

  if (Configuration::get().verbose) {
    std::cout << "Failed nodes: ";
    for (auto i : failed_nodes)
      std::cout << i;
    std::cout << std::endl;
  }
}

bool UniformFaults::failure(std::shared_ptr<Task> task)
{
  if (dynamic_cast<LogP::RecvStartTask*>(task.get())) {
    if (std::find(failed_nodes.begin(), failed_nodes.end(),
                  task->receiver()) != failed_nodes.end()) {
      if (Configuration::get().verbose) {
        std::cout << "Drop task " << *task << std::endl;
      }
      return true;
    }
  }
  // if (std::find(failed_nodes.begin(), failed_nodes.end(),
  //               task->sender()) != failed_nodes.end()) {
  //   return true;
  // }
  return false;
}
