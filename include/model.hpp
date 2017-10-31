#pragma once

#include <ostream>

#include "time.hpp"
#include "configuration.hpp"

struct Model
{
  Time L, o, g;
  int P;

  friend std::ostream &operator<<(std::ostream& os, const Model& m)
  {
    os << "L, o, g, P\n"
       << m.L << ","
       << m.o << ","
       << m.g << ","
       << m.P << std::endl;
    return os;
  }

  Model(const Configuration &conf)
    : L(Time(conf.L)),
      o(Time(conf.o)),
      g(Time(conf.g)),
      P(conf.P)
  {}
};
