#include <cstdio>
#include <cassert>
#include <exception>

#include <memory>
#include <iostream>
#include <fstream>

#include "event.hpp"
#include "model.hpp"
#include "task.hpp"
#include "task_queue.hpp"
#include "globals.hpp"

#include "configuration_args.hpp"
#include "results_printer.hpp"

class TaskQueue;
class Collective;

int main(int argc, char *argv[])
{
  ConfigurationArgs conf(argc, argv);
  Model model(conf);
  Entropy entropy(conf);

  Globals::set({&conf, &model, &entropy});

  auto printer = ResultsPrinter::create();

  printer->intro();

  for (unsigned i = 0; i < conf.repeat; i++) {
    entropy.reset_seed(conf.seed);
    Counter::reset_counters();

    Timeline timeline;
    auto faults = FaultInjector::create();

    try {
      auto coll = Collective({0}, faults.get());

      coll.run(timeline, CollectiveRegistry::create(coll.reached_nodes));

      printer->results(timeline, *faults.get());
    } catch (const std::exception &e) {
      std::cerr << "Caught an exception: " << e.what() << std::endl;
      std::cerr << "Initial environment: " << conf << std::endl;
      std::cerr << "Faults: " << *faults << std::endl;
    }

    if (!conf.log_prefix.empty()) {
      std::string iter("");
      if (conf.repeat > 1) {
        iter = "." + std::to_string(i);
      }

      auto trace_filename = conf.log_prefix + iter + ".trace.csv";
      std::ofstream trace_log(trace_filename);
      trace_log << timeline;

      auto model_filename = conf.log_prefix + ".model.csv";
      std::ofstream model_log(model_filename);
      model_log << model;
    }
  }

  return 0;
}
