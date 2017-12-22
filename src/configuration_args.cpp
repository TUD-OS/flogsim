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
     po::value<std::string>(&log_prefix)->value_name("PREFIX"),
     "Where to store the logs. Adds suffixes '.model.csv' and '.trace.csv' to the output files.")
    ("results-format",
     po::value<std::string>()->value_name("NAME")->default_value("tables"),
     "Format to print out runtime results. Allowed values: tables, csv, csv-id.")
    ("id",
     po::value<std::string>(&id)->value_name("ID"),
     "Id of the experiment. Used together with 'csv-id' format.")
    ("time_limit",
     po::value<int64_t>(&limit)->value_name("TIME"),
     "When to stop the simulation.")
    ("verbose,v",
     po::bool_switch(&verbose)->default_value(false),
     "Be verbose")
    ;

  po::options_description model("Model options");
  model.add_options()
    ("latency,L",
     po::value<int>(&L)->default_value(1)->value_name("TIME"),
     "Network latency")
    ("overhead,o",
     po::value<int>(&o)->default_value(1)->value_name("TIME"),
     "CPU overhead")
    ("gap,g",
     po::value<int>(&g)->default_value(2)->value_name("TIME"),
     "Send/Recv gap")
    ("processors,P",
     po::value<int>(&P)->default_value(8)->value_name("NUM"),
     "Number of processors")
    ("prio",
     po::value<Configuration::Priority>(&priority)->
     default_value(Configuration::Priority("recv"))->value_name("NAME"),
     "How to schedule tasks on a CPU. Option 'recv' prefers receives "
     "over sends. Option 'tag' looks at the tag first.")
    ("parallelism",
     po::value<int>(&parallelism)->default_value(1)->value_name("NUM"),
     "Parallelism level per node."
     " Level 2 allows simulteneous sending and receiving."
     " By LogP should be capped by ceil(L/g), but not in our case.")
    ;

  desc.add(model);

  po::options_description faults("Fault injector options");
  faults.add_options()
    ("faults",
     po::value<std::string>(&fault_injector)->
     default_value("none")->value_name("NAME"),
     "Type of fault injector")
    ("fault-count,F",
     po::value<int>(&F)->default_value(0)->value_name("NUM"),
     "Number of faults")
    ;

  desc.add(faults);

  po::options_description collectives("Collectives options");
  collectives.add_options()
    ("coll",
     po::value<std::string>(&collective)->
     default_value("binary_bcast")->value_name("NAME"),
     "Type of collective to model")
    ("karity,k",
     po::value<int>(&k)->default_value(2)->value_name("NUM"),
     "K-arity of the tree")
    ("seed",
     po::value<unsigned>(&seed)->default_value(0)->value_name("NUM"),
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

  if (!args.count("log")) {
    assert(log_prefix == "");
  }

  // Set default value for time limit
  if (!args.count("time_limit")) {
    limit = INT64_MAX;
  }

  // Check specification for results format
  bool need_id = false;
  if (args.count("results-format")) {
    results_format = args["results-format"].as<std::string>();
    if (results_format == "table") {
      // OK
    } else if (results_format == "csv") {
      // OK
    } else if (results_format == "csv-id") {
      if (args.count("id")) {
        // OK
        need_id = true;
      } else {
        std::cerr << "Format 'csv-id' expects the user "
                  << "to provide an experiment id (--id)." << std::endl;
        throw po::validation_error(po::validation_error::invalid_option_value);
      }
    } else {
      std::cerr << "Unknown output format: " << results_format << std::endl;
      throw po::validation_error(po::validation_error::invalid_option_value);
    }
  } else {
    results_format = "table";
  }

  if (args.count("id") && !need_id) {
    std::cerr << "Unexpected parameter --id." << std::endl;
    throw po::validation_error(po::validation_error::invalid_option_value);
  }

  if (args.count("help")) {
    show_help(desc);
    // no return
  }
}
