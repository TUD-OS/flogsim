#include "globals.hpp"

#include "test_wrappers.hpp"

#include "phase/gossip.hpp"
#include "phase/exclusive.hpp"
#include "phase/combiner.hpp"
#include "phase/correction.hpp"

namespace
{

typedef PhaseTest CheckedCorrectedGossipTests;
TEST_P(CheckedCorrectedGossipTests, Runtime)
{
  do_test(
    [](ReachedNodes& rn)
    {
      auto phases = Combiner::Phases(rn).
        add_phase<Gossip>().
        add_phase<CheckedCorrection<true>>();

      return std::make_unique<Combiner>(std::move(phases));
    });
}

RunParams tests[] = {
  RunParams().LogP(1, 1, 1, 7).k(3).failed({4}).runtime(20),
};


INSTANTIATE_TEST_CASE_P(Failed,
                        CheckedCorrectedGossipTests,
                        ::testing::ValuesIn(tests));
}
