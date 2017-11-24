#include "simple_tree_bcast.hpp"
#include "globals.hpp"

#include "test_wrappers.hpp"

#define ALGORITHM SimpleTreeBroadcast
namespace
{

TEST(ALGORITHM, Functional)
{

  NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 4 ).k(3).runtime(5)();
  NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 13).k(3).runtime(10)();
  NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 5 ).k(4).runtime(6)();

  UniformFaultTest<ALGORITHM>().LogP(1, 1, 1, 4).k(3).
    failed({0}).runtime(0).unreach(3)();
  UniformFaultTest<ALGORITHM>().LogP(1, 1, 1, 4).k(3).
    failed({1}).runtime(5)();

}

}
