#pragma once

#include <cinttypes>
#include <ostream>

struct Configuration
{
  std::string log_prefix;
  bool verbose;
  int64_t limit;

  // Model parameters
  int L, o, g, P;

  // Fault injector parametrs
  std::string fault_injector;
  int F;

  // Collectives parameters
  std::string collective;
  int k;

  friend std::ostream &operator<<(std::ostream &os, const Configuration &conf)
  {
    os << "L = " << conf.L << ", "
       << "o = " << conf.o << ", "
       << "g = " << conf.g << ", "
       << "P = " << conf.P << ", "
       << "k = " << conf.k << ", "
       << "limit = " << conf.limit << ", "
       << "F = " << conf.F << ", "
       << "fault_injector = " << conf.fault_injector;
    return os;
  }

  // Builder methods
  Configuration &LogP(int L, int o, int g, int P)
  {
    this->L = L;
    this->o = o;
    this->g = g;
    this->P = P;

    return *this;
  }

  Configuration &faults(const std::string &fault_injector, int F = 0)
  {
    this->fault_injector = fault_injector;
    this->F = F;

    return *this;
  }

  Configuration(int k, int64_t limit)
    : log_prefix(), verbose(false), limit(limit), collective(), k(k)
  {}

  Configuration()
    : Configuration(2, INT64_MAX)
  {}
};
