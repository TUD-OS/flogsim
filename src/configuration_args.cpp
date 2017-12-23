#include <cstdlib>
#include <cstdint>

#include <iostream>

#include <boost/program_options.hpp>

#include "configuration_args.hpp"
#include "collective_registry.hpp"

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

namespace
{

void
show_help(const po::options_description& desc)
{
    std::cout << desc << '\n';
    exit( EXIT_SUCCESS );
}

po::options_description create_options_description(ConfigurationArgs &conf)
{
  po::options_description desc("General options");
  desc.add_options()
    ("help", "Show help message")
    ("log",
     po::value<std::string>(&conf.log_prefix)->value_name("PREFIX"),
     "Where to store the logs. Adds suffixes '.model.csv' and '.trace.csv' to the output files.")
    ("results-format",
     po::value<std::string>()->value_name("NAME")->default_value("table"),
     "Format to print out runtime results. Allowed values: table, csv, csv-id.")
    ("id",
     po::value<std::string>(&conf.id)->value_name("ID"),
     "Id of the experiment. Used together with 'csv-id' format.")
    ("repeat,r",
     po::value<unsigned>(&conf.repeat)->value_name("NUM")->default_value(1),
     "How many times we should repeat the experiment.")
    ("no-header",
     po::value<bool>(&conf.no_header)->implicit_value(true),
     "Should the printer omit the header. This option is relevant if header exists.")
    ("time_limit",
     po::value<int64_t>(&conf.limit)->value_name("TIME"),
     "When to stop the simulation.")
    ("verbose,v",
     po::bool_switch(&conf.verbose)->default_value(false),
     "Be verbose")
    ;

  po::options_description model("Model options");
  model.add_options()
    ("latency,L",
     po::value<int>(&conf.L)->default_value(1)->value_name("TIME"),
     "Network latency")
    ("overhead,o",
     po::value<int>(&conf.o)->default_value(1)->value_name("TIME"),
     "CPU overhead")
    ("gap,g",
     po::value<int>(&conf.g)->default_value(2)->value_name("TIME"),
     "Send/Recv gap")
    ("processors,P",
     po::value<int>(&conf.P)->default_value(8)->value_name("NUM"),
     "Number of processors")
    ("prio",
     po::value<Configuration::Priority>(&conf.priority)->
     default_value(Configuration::Priority("recv"))->value_name("NAME"),
     "How to schedule tasks on a CPU. Option 'recv' prefers receives "
     "over sends. Option 'tag' looks at the tag first.")
    ("parallelism",
     po::value<int>(&conf.parallelism)->default_value(1)->value_name("NUM"),
     "Parallelism level per node."
     " Level 2 allows simulteneous sending and receiving."
     " By LogP should be capped by ceil(L/g), but not in our case.")
    ;

  desc.add(model);

  po::options_description faults("Fault injector options");
  faults.add_options()
    ("faults",
     po::value<std::string>(&conf.fault_injector)->
     default_value("none")->value_name("NAME"),
     "Type of fault injector")
    ("fault-count,F",
     po::value<int>(&conf.F)->default_value(0)->value_name("NUM"),
     "Number of faults")
    ;

  desc.add(faults);

  po::options_description collectives("Collectives options");
  collectives.add_options()
    ("coll",
     po::value<std::string>(&conf.collective)->value_name("NAME"),
     "Type of collective to model")
    ("karity,k",
     po::value<int>(&conf.k)->default_value(2)->value_name("NUM"),
     "K-arity of the tree")
    ("seed",
     po::value<unsigned>(&conf.seed)->default_value(0)->value_name("NUM"),
     "Seed for the entropy source. Value 0 tells the simulator to generate "
     "seed on its own.")
    ;

  desc.add(collectives);
  return desc;
}

void validate_options(ConfigurationArgs &conf, po::variables_map &args)
{
  // Check collective parameter
  const auto coll_list = CollectiveRegistry::list();
  if (!args.count("coll")) {
    std::cerr << "Missing --coll parameter." << std::endl;
    throw po::validation_error(po::validation_error::invalid_option_value);
  } else if (conf.collective == "list") {
    std::cout << "List of collectives:" << std::endl;
    for (const auto &coll : coll_list) {
      std::cout << "  " << coll << std::endl;
    }
    exit( EXIT_SUCCESS );
    // no return
  } else {
    auto search_result = std::find(coll_list.cbegin(), coll_list.cend(),
                                   conf.collective);
    if (search_result == coll_list.end()) {
      std::cerr << "Unknown collective specified in --coll parameter." << std::endl;
      throw po::validation_error(po::validation_error::invalid_option_value);
    }
  }

  // Set default value for time limit
  if (!args.count("time_limit")) {
    conf.limit = INT64_MAX;
  }

  // Check specification for results format
  bool need_id = false;
  if (args.count("results-format")) {
    conf.results_format = args["results-format"].as<std::string>();
    if (conf.results_format == "table") {
      // OK
    } else if (conf.results_format == "csv") {
      // OK
    } else if (conf.results_format == "csv-id") {
      if (args.count("id")) {
        // OK
        need_id = true;
      } else {
        std::cerr << "Format 'csv-id' expects the user "
                  << "to provide an experiment id (--id)." << std::endl;
        throw po::validation_error(po::validation_error::invalid_option_value);
      }
    } else {
      std::cerr << "Unknown output format: "
                << conf.results_format << std::endl;
      throw po::validation_error(po::validation_error::invalid_option_value);
    }
  } else {
    conf.results_format = "table";
  }

  if (args.count("id") && !need_id) {
    std::cerr << "Unexpected parameter --id." << std::endl;
    throw po::validation_error(po::validation_error::invalid_option_value);
  }
}

}

ConfigurationArgs::ConfigurationArgs(int argc, char *argv[])
{
  auto desc = create_options_description(*this);

  po::variables_map args;

  try {
    po::store(po::parse_command_line(argc, argv, desc), args);

    po::notify(args);

    validate_options(*this, args);
  } catch (po::error const& e) {
    std::cerr << "Exit reason: " << e.what() << '\n';
    show_help(desc);
  }

  if (args.count("help")) {
    show_help(desc);
    // no return
  }
}
