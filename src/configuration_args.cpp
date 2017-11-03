#include <cstdlib>
#include <cstdint>

#include <iostream>

#include <boost/program_options.hpp>

#include "configuration_args.hpp"

namespace po = boost::program_options;

void
show_help(const po::options_description& desc)
{
    std::cout << desc << '\n';
    exit( EXIT_SUCCESS );
}

ConfigurationArgs::ConfigurationArgs(int argc, char *argv[])
{
  po::options_description desc("General options");
  desc.add_options()
    ("help", "Show help message")
    ("log",
     po::value<std::string>(&log_prefix)->default_value("log"),
     "Where to store the logs. Adds suffixes '.model.csv' and '.trace.csv' to the output files.")
    ("time_limit",
     po::value<int64_t>(&limit)->default_value(UINT64_MAX),
     "When to stop the simulation.")
    ("verbose,v",
     po::bool_switch(&verbose)->default_value(false),
     "Be verbose")
    ;

  po::options_description model("Model options");
  model.add_options()
    ("L",
     po::value<int>(&L)->default_value(1),
     "Network latency")
    ("o",
     po::value<int>(&o)->default_value(1),
     "CPU overhead")
    ("g",
     po::value<int>(&g)->default_value(2),
     "Send/Recv gap")
    ("P",
     po::value<int>(&P)->default_value(8),
     "Number of processors")
    ;

  desc.add(model);

  po::options_description faults("Fault injector options");
  faults.add_options()
    ("faults",
     po::value<std::string>(&fault_injector)->default_value("none"),
     "Type of fault injector")
    ("F",
     po::value<int>(&F)->default_value(1),
     "Number of faults")
    ;

  desc.add(faults);

  po::options_description collectives("Collectives options");
  collectives.add_options()
    ("coll",
     po::value<std::string>(&collective)->default_value("binary_bcast"),
     "Type of collective to model")
    ("k",
     po::value<int>(&k)->default_value(2),
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
}
