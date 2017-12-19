#include "globals.hpp"

#include "test_wrappers.hpp"

#include "phase/kary_tree.hpp"

namespace
{

typedef PhaseTest KaryTreeTests;
TEST_P(KaryTreeTests, Runtime)
{
  do_test(
    [](ReachedNodes& rn)
    {
      return std::make_unique<KAryTree<true>>(rn);
    });
}

RunParams tests[] = {
  RunParams().LogP(1, 1, 1, 4 ).k(3).runtime(5),
  RunParams().LogP(1, 1, 1, 13).k(3).runtime(10),
  RunParams().LogP(1, 1, 1, 5 ).k(4).runtime(6),
  RunParams().LogP(1, 1, 1, 4).k(3).failed({0}).runtime(0).unreach(3),
  RunParams().LogP(1, 1, 1, 4).k(3).failed({1}).runtime(5),
};


INSTANTIATE_TEST_CASE_P(All,
                        KaryTreeTests,
                        ::testing::ValuesIn(tests));
}
