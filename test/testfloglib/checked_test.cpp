#include "globals.hpp"

#include "test_wrappers.hpp"

#include "phase/kary_tree.hpp"
#include "phase/exclusive.hpp"
#include "phase/combiner.hpp"
#include "phase/correction.hpp"

namespace
{

typedef PhaseTest CheckedCorrectedKaryTreeBroadcast;
TEST_P(CheckedCorrectedKaryTreeBroadcast, Runtime)
{
  do_test(
    [](ReachedNodes& rn)
    {
      auto phases = Combiner::Phases(rn).
        add_phase<KAryTree<true>>().
        add_phase<CheckedCorrection<false>>();

      return std::make_unique<Combiner>(std::move(phases));
    });
}

RunParams tests[] = {
  RunParams().LogP(1, 1, 1, 3).k(2).runtime(9),
  RunParams().LogP(1, 1, 1, 7).k(2).runtime(14),
  RunParams().LogP(1, 1, 1, 15).k(2).runtime(20),
  RunParams().LogP(1, 1, 1, 3).k(2).failed({0}).runtime(0).unreach(2),
  RunParams().LogP(1, 1, 1, 7).k(2).failed({0}).runtime(0).unreach(6),
  RunParams().LogP(1, 1, 1, 15).k(2).failed({0}).runtime(0).unreach(14),
  RunParams().LogP(1, 1, 1, 3).k(2).failed({1}).runtime(9).unreach(0),
  RunParams().LogP(1, 1, 1, 7).k(2).failed({1}).runtime(15).unreach(0),
  RunParams().LogP(1, 1, 1, 15).k(2).failed({1}).runtime(20).unreach(0),
};


INSTANTIATE_TEST_CASE_P(All,
                        CheckedCorrectedKaryTreeBroadcast,
                        ::testing::ValuesIn(tests));

// <TechnicalDetails>
//
// High level test which checks how the runtime and that all the
// nodes which should finish are finished.
//
// </TechnicalDetails>
}
