#pragma once

struct Configuration
{
  std::string log_prefix;
  bool verbose;

  // Model parameters
  int L, o, g, P;

  // Fault injector parametrs
  std::string fault_injector;
  int F;

  static void parse_args(int argc, char *argv[]);

  static const Configuration &get();
private:
  bool initialized = false;
};
