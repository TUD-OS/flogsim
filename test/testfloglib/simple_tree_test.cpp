#include "globals.hpp"

#include "test_wrappers.hpp"
#include "kary_tree.hpp"

namespace
{

TEST_P(PhaseTest, Runtime)
{
  do_test(
    [](ReachedNodes& rn)
    {
      return std::make_unique<KAryTreePhase<true>>(rn);
    });
}

RunParams KaryTreeTests[] = {
  RunParams().LogP(1, 1, 1, 4 ).k(3).runtime(5),
  RunParams().LogP(1, 1, 1, 13).k(3).runtime(10),
  RunParams().LogP(1, 1, 1, 5 ).k(4).runtime(6),
  RunParams().LogP(1, 1, 1, 4).k(3).failed({0}).runtime(0).unreach(3),
  RunParams().LogP(1, 1, 1, 4).k(3).failed({1}).runtime(5),
};


INSTANTIATE_TEST_CASE_P(KaryTreeTests,
                        PhaseTest,
                        ::testing::ValuesIn(KaryTreeTests));
}
