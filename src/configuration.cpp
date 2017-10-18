#include <cstdlib>
#include <iostream>

#include <boost/program_options.hpp>

#include "configuration.hpp"

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

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "Show help message")
    ("log,l",
     po::value<std::string>(&temp_conf.log_prefix)->default_value("log"),
     "Where to store the logs. Adds suffixes '.model.csv' and '.trace.csv' to the output files.")
    ("verbose,v",
     po::bool_switch(&temp_conf.verbose)->default_value(false),
     "Be verbose")
    ;

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

  auto &conf = const_cast<Configuration&>(Configuration::get());
  conf = temp_conf;
}

const Configuration &Configuration::get()
{
  static Configuration conf;
  return conf;
}
