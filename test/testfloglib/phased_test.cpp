#include "phased_checked_correctedtree_bcast.hpp"
#include "globals.hpp"

#include "test_wrappers.hpp"

#define ALGORITHM PhasedCheckedCorrectedTreeBroadcast
namespace
{

TEST(ALGORITHM, Functional)
{

  CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 7).k(2).runtime(12));

  CALL(FaultTest<ALGORITHM>().LogP(1, 1, 1, 7).k(2).
       failed({4}).runtime(16));
  CALL(FaultTest<ALGORITHM>().LogP(2, 1, 1, 15).k(2).
       failed({5, 12}).runtime(26));

  // <TechnicalDetails>
  //
  // High level test which checks how the runtime and that all the
  // nodes which should finish are finished.
  //
  // </TechnicalDetails>
}

}
