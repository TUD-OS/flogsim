#pragma once

#include <cinttypes>
#include <ostream>

struct Configuration
{
  std::string log_prefix;
  // How to print out simulation results
  std::string results_format;
  // Id of the experiment parameters
  std::string id;
  unsigned repeat;

  bool verbose;
  int64_t limit;

  // Model parameters
  int L, o, g, P;

  // Fault injector parametrs
  std::string fault_injector = "none";
  int F;
  unsigned seed;

  // Collectives parameters
  std::string collective;
  int k;
  int parallelism;
  class Priority
  {
    int _value;
  public:
    enum
    {
      RECV,
      TAG,
    };

    Priority(const std::string &value = "recv")
    {
      if (value == "tag") {
        _value = TAG;
      } else if (value == "recv") {
        _value = RECV;
      } else {
        throw std::runtime_error("Unknow priority provided: " + value);
      }
    }

    friend std::ostream &operator<<(std::ostream &os, const Priority &prio)
    {
      switch (prio.get()) {
        case RECV:
          os << "recv";
          break;
        case TAG:
          os << "tag";
          break;
      }
      return os;
    }

    friend std::istream &operator>>(std::istream &is, Priority &prio)
    {
      std::string s;
      is >> s;
      prio = Priority(s);
      return is;
    }

    int get() const { return _value; }
  };

  Priority priority;

  friend std::ostream &operator<<(std::ostream &os, const Configuration &conf)
  {
    os << "L = " << conf.L << ", "
       << "o = " << conf.o << ", "
       << "g = " << conf.g << ", "
       << "P = " << conf.P << ", "
       << "k = " << conf.k << ", "
       << "limit = " << conf.limit << ", "
       << "F = " << conf.F << ", "
       << "seed = " << conf.seed << ", "
       << "Parallelism = " << conf.parallelism << ", "
       << "Priority = " << conf.priority << ", "
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

  Configuration &set_seed(unsigned seed)
  {
    this->seed = seed;

    return *this;
  }

  Configuration(int k, int parallelism, int64_t limit)
    : log_prefix(), verbose(false), limit(limit),
      L(1), o(1), g(1), P(1),
      F(0),
      seed(0),
      collective(), k(k),
      parallelism(parallelism)
  {}

  Configuration(int k, int64_t limit)
    : Configuration(k, 1, limit)
  {}

  Configuration()
    : Configuration(2, INT64_MAX)
  {}
};
