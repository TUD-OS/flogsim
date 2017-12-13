#include "globals.hpp"

#include "test_wrappers.hpp"

#define ALGORITHM CheckedCorrectedTreeBroadcast
namespace
{

TEST(ALGORITHM, Functional)
{

  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 3).k(2).runtime(9));   //  4
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 7).k(2).runtime(14));  //  9
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 15).k(2).runtime(20)); // 14

  // CALL(FaultTest<ALGORITHM>().LogP(1, 1, 1, 3).k(2).
  //      failed({0}).runtime(0).unreach(2));
  // CALL(FaultTest<ALGORITHM>().LogP(1, 1, 1, 7).k(2).
  //      failed({0}).runtime(0).unreach(6));
  // CALL(FaultTest<ALGORITHM>().LogP(1, 1, 1, 15).k(2).
  //      failed({0}).runtime(0).unreach(14));

  // CALL(FaultTest<ALGORITHM>().LogP(1, 1, 1, 3).k(2).
  //      failed({1}).runtime(9).unreach(0));                        // 3
  // CALL(FaultTest<ALGORITHM>().LogP(1, 1, 1, 7).k(2).
  //      failed({1}).runtime(15).unreach(0));                       // 9
  // CALL(FaultTest<ALGORITHM>().LogP(1, 1, 1, 15).k(2).
  //      failed({1}).runtime(20).unreach(0));                       // 16

  // <TechnicalDetails>
  //
  // High level test which checks how the runtime and that all the
  // nodes which should finish are finished.
  //
  // </TechnicalDetails>
}

}
