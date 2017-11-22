#include "gtest/gtest.h"

#include "phased_checked_correctedtree_bcast.hpp"
#include "globals.hpp"

#define ALGORITHM PhasedCheckedCorrectedTreeBroadcast
#include "Wrappers.hpp"

namespace
{

TEST(ALGORITHM, Functional)
{

  MAKE_NOFAULT_TESTCASE(1,1,1,7,2, 12)

  MAKE_UNIFORMFAULT_TESTCASE(1,1,1,7,2,{4}, 16,0)

  // <TechnicalDetails>
  //
  // High level test which checks how the runtime and that all the
  // nodes which should finish are finished.
  //
  // </TechnicalDetails>
}

}
