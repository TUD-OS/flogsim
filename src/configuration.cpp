#include <cstdlib>
#include <cstdint>

#include <iostream>

#include <boost/program_options.hpp>

#include "configuration.hpp"

static Configuration &__get()
{
  static Configuration conf;
  return conf;
}

const Configuration &Configuration::get()
{
  auto &conf = __get();
  if (!conf.initialized) {
    std::runtime_error("Initialize configuration first");
  }
  return conf;
}

namespace po = boost::program_options;

void
show_help(const po::options_description& desc)
{
    std::cout << desc << '\n';
    exit( EXIT_SUCCESS );
}

void Configuration::parse_args(int argc, char *argv[])
{
  Configuration temp_conf;

  po::options_description desc("General options");
  desc.add_options()
    ("help", "Show help message")
    ("log",
     po::value<std::string>(&temp_conf.log_prefix)->default_value("log"),
     "Where to store the logs. Adds suffixes '.model.csv' and '.trace.csv' to the output files.")
    ("time_limit",
     po::value<uint64_t>(&temp_conf.limit)->default_value(UINT64_MAX),
     "When to stop the simulation.")
    ("verbose,v",
     po::bool_switch(&temp_conf.verbose)->default_value(false),
     "Be verbose")
    ;

  po::options_description model("Model options");
  model.add_options()
    ("L",
     po::value<int>(&temp_conf.L)->default_value(1),
     "Network latency")
    ("o",
     po::value<int>(&temp_conf.o)->default_value(1),
     "CPU overhead")
    ("g",
     po::value<int>(&temp_conf.g)->default_value(2),
     "Send/Recv gap")
    ("P",
     po::value<int>(&temp_conf.P)->default_value(8),
     "Number of processors")
    ;

  desc.add(model);

  po::options_description faults("Fault injector options");
  faults.add_options()
    ("faults",
     po::value<std::string>(&temp_conf.fault_injector)->default_value("none"),
     "Type of fault injector")
    ("F",
     po::value<int>(&temp_conf.F)->default_value(1),
     "Number of faults")
    ;

  desc.add(faults);

  po::options_description collectives("Collectives options");
  collectives.add_options()
    ("coll",
     po::value<std::string>(&temp_conf.collective)->default_value("binary_bcast"),
     "Type of collective to model")
    ("k",
     po::value<int>(&temp_conf.k)->default_value(2),
     "K-arity of the tree")
    ;

  desc.add(collectives);

  po::variables_map args;

  try {
    po::store(
      po::parse_command_line(argc, argv, desc),
      args);
  } catch (po::error const& e) {
    std::cerr << e.what() << '\n';
    exit(EXIT_FAILURE);
  }
  po::notify(args);

  if (args.count("help")) {
    show_help(desc);
    // no return
  }

  auto &conf = __get();
  conf = temp_conf;
  conf.initialized = true;
}
