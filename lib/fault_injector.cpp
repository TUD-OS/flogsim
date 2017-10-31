#include <algorithm>
#include <ctime>        // std::time
#include <stdexcept>
#include <iterator>

#include "fault_injector.hpp"
#include "task.hpp"

std::unique_ptr<FaultInjector> FaultInjector::create(const Configuration &conf)
{
  if (conf.fault_injector == "none") {
    return std::make_unique<NoFaults>(conf);
  } else if (conf.fault_injector == "uniform") {
    return std::make_unique<UniformFaults>(conf);
  } else {
    throw std::invalid_argument("Fault injector does not exist:" +
                                conf.fault_injector);
  }
}

UniformFaults::UniformFaults(const Configuration &conf)
  : FaultInjector(conf), P(conf.P), F(conf.F)
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

  if (conf.verbose) {
    std::cout << "Failed nodes: ";
    for (auto i : failed_nodes)
      std::cout << i;
    std::cout << std::endl;
  }
}

void UniformFaults::print(std::ostream &os) const
{
  std::copy(failed_nodes.begin(), failed_nodes.end(),
            std::ostream_iterator<int>(os, " "));
}

Fault UniformFaults::failure(std::shared_ptr<Task> task)
{
  if (dynamic_cast<RecvStartTask*>(task.get()) != nullptr) {
    if (std::find(failed_nodes.begin(), failed_nodes.end(),
                  task->receiver()) != failed_nodes.end()) {
      if (conf.verbose) {
        std::cout << "Drop receive task " << *task << std::endl;
      }
      return Fault::FAILURE;
    }
  } else if (dynamic_cast<SendStartTask*>(task.get()) != nullptr) {
    if (std::find(failed_nodes.begin(), failed_nodes.end(),
                  task->sender()) != failed_nodes.end()) {
      if (conf.verbose) {
        std::cout << "Drop send task " << *task << std::endl;
      }
      return Fault::SKIP;
    }
  }
  return Fault::OK;
}
