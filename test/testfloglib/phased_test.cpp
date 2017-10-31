#include "gtest/gtest.h"

// #include "../collectives/phased_checked_correctedtree_bcast.hpp"

namespace
{

// static CollectiveRegistrator<PhasedCheckedCorrectedTreeBroadcast> pcctb("phased_checked_correctedtree_bcast");
TEST(FactorialTest, Negative) {
  // This test is named "Negative", and belongs to the "FactorialTest"
  // test case.
  EXPECT_EQ(1, -5);

  // <TechnicalDetails>
  //
  // EXPECT_EQ(expected, actual) is the same as
  //
  //   EXPECT_TRUE((expected) == (actual))
  //
  // except that it will print both the expected value and the actual
  // value when the assertion fails.  This is very helpful for
  // debugging.  Therefore in this case EXPECT_EQ is preferred.
  //
  // On the other hand, EXPECT_TRUE accepts any Boolean expression,
  // and is thus more general.
  //
  // </TechnicalDetails>
}

}