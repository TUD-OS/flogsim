#include "globals.hpp"

#include "test_wrappers.hpp"

#include "phase/optimal_tree.hpp"
#include "phase/combiner.hpp"
#include "phase/correction.hpp"

namespace
{

typedef PhaseTest CheckedCorrectedOptimalTreeBroadcast;
TEST_P(CheckedCorrectedOptimalTreeBroadcast, Runtime)
{
  do_test(
    [](ReachedNodes& rn)
    {
      auto phases = Combiner::Phases(rn).
        add_phase<OptimalTree>().
        add_phase<CheckedCorrection<false>>();

      return std::make_unique<Combiner>(std::move(phases));
    });
}

RunParams tests[] = {
  RunParams().LogP(1, 1, 1, 15).parallel(2).runtime(16).failed({1, 3}),
};


INSTANTIATE_TEST_CASE_P(All,
                        CheckedCorrectedOptimalTreeBroadcast,
                        ::testing::ValuesIn(tests));

// <TechnicalDetails>
//
// High level test which checks how the runtime and that all the
// nodes which should finish are finished.
//
// </TechnicalDetails>
}
