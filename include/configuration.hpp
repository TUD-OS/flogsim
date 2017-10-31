#pragma once

#include <cinttypes>

struct Configuration
{
  std::string log_prefix;
  bool verbose;
  uint64_t limit;

  // Model parameters
  int L, o, g, P;

  // Fault injector parametrs
  std::string fault_injector;
  int F;

  // Collectives parameters
  std::string collective;
  int k;
};
