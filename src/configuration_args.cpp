#include <cstdlib>
#include <cstdint>

#include <iostream>

#include <boost/program_options.hpp>

#include "configuration_args.hpp"

namespace po = boost::program_options;

void validate(boost::any& v,
              const std::vector<std::string>& values,
              Configuration::Priority,
              int)
{
  po::validators::check_first_occurrence(v);

  std::string const&s = po::validators::get_single_string(values);

  if (s == "tag" || s == "recv") {
    v = boost::any(Configuration::Priority(s));
  } else {
    throw po::validation_error(po::validation_error::invalid_option_value);
  }
}

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
     po::value<int64_t>(&limit)->default_value(INT64_MAX),
     "When to stop the simulation.")
    ("verbose,v",
     po::bool_switch(&verbose)->default_value(false),
     "Be verbose")
    ;

  po::options_description model("Model options");
  model.add_options()
    ("latency,L",
     po::value<int>(&L)->default_value(1),
     "Network latency")
    ("overhead,o",
     po::value<int>(&o)->default_value(1),
     "CPU overhead")
    ("gap,g",
     po::value<int>(&g)->default_value(2),
     "Send/Recv gap")
    ("processors,P",
     po::value<int>(&P)->default_value(8),
     "Number of processors")
    ("prio",
     po::value<Configuration::Priority>(&priority)->default_value(Configuration::Priority("recv")),
     "How to schedule tasks on a CPU. Option 'recv' prefers receives "
     "over sends. Option 'tag' looks at the tag first.")
    ("parallelism",
     po::value<int>(&parallelism)->default_value(1),
     "Parallelism level per node."
     " Level 2 allows simulteneous sending and receiving."
     " By LogP should be capped by ceil(L/g), but not in our case.")
    ;

  desc.add(model);

  po::options_description faults("Fault injector options");
  faults.add_options()
    ("faults",
     po::value<std::string>(&fault_injector)->default_value("none"),
     "Type of fault injector")
    ("fault-count,F",
     po::value<int>(&F)->default_value(0),
     "Number of faults")
    ;

  desc.add(faults);

  po::options_description collectives("Collectives options");
  collectives.add_options()
    ("coll",
     po::value<std::string>(&collective)->default_value("binary_bcast"),
     "Type of collective to model")
    ("karity,k",
     po::value<int>(&k)->default_value(2),
     "K-arity of the tree")
    ("seed",
     po::value<unsigned>(&seed)->default_value(0),
     "Seed for the entropy source. Value 0 tells the simulator to generate "
     "seed on its own.")
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
