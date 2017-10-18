#pragma once

#include "time.hpp"

namespace LogP
{

struct Model
{
  Time L, o, g;
  int P;

  void dump()
  {
    std::printf("L, o, g, P\n");
    L.dump(); std::printf(",");
    o.dump(); std::printf(",");
    g.dump(); std::printf(",");
    std::printf("%d\n", P);
  }

  static const Model &get();
};

}
