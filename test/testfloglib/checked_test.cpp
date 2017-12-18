#include "globals.hpp"

#include "test_wrappers.hpp"
#include "kary_tree.hpp"
#include "exclusive_phase.hpp"
#include "combiner_phase.hpp"

#include "correction_phase.hpp"

namespace
{

typedef PhaseTest CheckedCorrectedKaryTreeBroadcast;
TEST_P(CheckedCorrectedKaryTreeBroadcast, Runtime)
{
  do_test(
    [](ReachedNodes& rn)
    {
      auto phases = CombinerPhase::Phases(rn).
        add_phase<KAryTreePhase<true>>().
        add_phase<CheckedCorrectionPhase<false>>();

      return std::make_unique<CombinerPhase>(std::move(phases));
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
