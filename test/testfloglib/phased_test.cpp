#include "globals.hpp"

#include "test_wrappers.hpp"

#include "phase/kary_tree.hpp"
#include "phase/exclusive.hpp"
#include "phase/combiner.hpp"
#include "phase/correction.hpp"

namespace
{

typedef PhaseTest PhasedCheckedCorrectedKaryTreeBroadcast;
TEST_P(PhasedCheckedCorrectedKaryTreeBroadcast, Runtime)
{
  do_test(
    [](ReachedNodes& rn)
    {
      auto phases = Combiner::Phases(rn).
        add_phase<Exclusive>(std::make_unique<KAryTree<true>>(rn)).
        add_phase<CheckedCorrection<true>>();

      return std::make_unique<Combiner>(std::move(phases));
    });
}

RunParams tests_k_2[] = {
  // All is 1
  RunParams().LogP(1, 1, 1, 3).k(2).runtime(8),
  RunParams().LogP(1, 1, 1, 4).k(2).runtime(10),
  RunParams().LogP(1, 1, 1, 5).k(2).runtime(11),
  RunParams().LogP(1, 1, 1, 7).k(2).runtime(12),
  RunParams().LogP(1, 1, 1, 8).k(2).runtime(13),
  RunParams().LogP(1, 1, 1, 13).k(2).runtime(15),
  RunParams().LogP(1, 1, 1, 15).k(2).runtime(16),
  RunParams().LogP(1, 1, 1, 16).k(2).runtime(16),
  RunParams().LogP(1, 1, 1, 21).k(2).runtime(18),
  // L = 2
  RunParams().LogP(2, 1, 1, 3).k(2).runtime(11),
  RunParams().LogP(2, 1, 1, 4).k(2).runtime(14),
  RunParams().LogP(2, 1, 1, 5).k(2).runtime(15),
  RunParams().LogP(2, 1, 1, 7).k(2).runtime(16),
  RunParams().LogP(2, 1, 1, 8).k(2).runtime(18),
  RunParams().LogP(2, 1, 1, 13).k(2).runtime(20),
  RunParams().LogP(2, 1, 1, 15).k(2).runtime(21),
  RunParams().LogP(2, 1, 1, 16).k(2).runtime(22),
  RunParams().LogP(2, 1, 1, 21).k(2).runtime(24),
  // o = 2
  RunParams().LogP(1, 2, 1, 3).k(2).runtime(15),
  RunParams().LogP(1, 2, 1, 4).k(2).runtime(18),
  RunParams().LogP(1, 2, 1, 5).k(2).runtime(20),
  RunParams().LogP(1, 2, 1, 7).k(2).runtime(22),
  RunParams().LogP(1, 2, 1, 8).k(2).runtime(23),
  RunParams().LogP(1, 2, 1, 13).k(2).runtime(27),
  RunParams().LogP(1, 2, 1, 15).k(2).runtime(29),
  RunParams().LogP(1, 2, 1, 16).k(2).runtime(29),
  RunParams().LogP(1, 2, 1, 21).k(2).runtime(32),

  // g = 2
  // Where should the phase end if g > 1 and o != g?
  RunParams().LogP(1, 1, 2, 3).k(2).runtime(99),
  // // Where should the phase end if g > 1 and o != g?
  RunParams().LogP(2, 1, 2, 5).k(2).runtime(99),
  // It shouldn't send to the same node on different rings
  RunParams().LogP(3, 2, 1, 3).k(2).runtime(99),

  // L = 3, o = 2
  RunParams().LogP(3, 2, 1, 4).k(2).runtime(26),
  RunParams().LogP(3, 2, 1, 5).k(2).runtime(28),
  RunParams().LogP(3, 2, 1, 7).k(2).runtime(30),
  RunParams().LogP(3, 2, 1, 8).k(2).runtime(33),
  RunParams().LogP(3, 2, 1, 13).k(2).runtime(37),
  RunParams().LogP(3, 2, 1, 15).k(2).runtime(39),
  RunParams().LogP(3, 2, 1, 16).k(2).runtime(40),
  RunParams().LogP(3, 2, 1, 21).k(2).runtime(44),
};


INSTANTIATE_TEST_CASE_P(K_2,
                        PhasedCheckedCorrectedKaryTreeBroadcast,
                        ::testing::ValuesIn(tests_k_2));

RunParams tests_k_3[] = {
  // L is 2
  RunParams().LogP(2, 1, 1, 3).k(3).runtime(11),
  RunParams().LogP(2, 1, 1, 4).k(3).runtime(12),
  RunParams().LogP(2, 1, 1, 5).k(3).runtime(14),
  RunParams().LogP(2, 1, 1, 21).k(3).runtime(21),
  RunParams().LogP(1, 3, 2, 5).k(3).runtime(26),
};

INSTANTIATE_TEST_CASE_P(K_3,
                        PhasedCheckedCorrectedKaryTreeBroadcast,
                        ::testing::ValuesIn(tests_k_3));


RunParams tests_faults[] = {
  RunParams().LogP(1, 1, 1, 7).k(2).failed({4}).runtime(16),
  RunParams().LogP(2, 1, 1, 15).k(2).failed({5, 12}).runtime(26),
  RunParams().LogP(2, 1, 1, 15).k(2).failed({0, 1}).runtime(0).unreach(13).limit(99),
};

INSTANTIATE_TEST_CASE_P(Faults,
                        PhasedCheckedCorrectedKaryTreeBroadcast,
                        ::testing::ValuesIn(tests_faults));

}

// <TechnicalDetails>
//
// Test which check functionality of fault fault-free case with
// k-ary tree.
//
// </TechnicalDetails>
// }

