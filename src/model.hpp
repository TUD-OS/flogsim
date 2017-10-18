#pragma once

#include <ostream>

#include "time.hpp"

namespace LogP
{

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

  static const Model &get();
};

}
