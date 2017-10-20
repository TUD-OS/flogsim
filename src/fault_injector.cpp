#include <algorithm>
#include <ctime>        // std::time

#include "fault_injector.hpp"
#include "configuration.hpp"

UniformFaults::UniformFaults(int P, int F)
  : P(P), F(F)
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
  if (dynamic_cast<LogP::RecvTask*>(task.get())) {
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
