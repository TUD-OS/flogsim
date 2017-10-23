#pragma once

struct Configuration
{
  std::string log_prefix;
  bool verbose;

  // Model parameters
  int L, o, g, P;

  static void parse_args(int argc, char *argv[]);

  static const Configuration &get();
private:
  bool initialized = false;
};
