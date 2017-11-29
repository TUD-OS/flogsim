#include "checked_correctedtree_bcast.hpp"
#include "globals.hpp"

#include "test_wrappers.hpp"

#define ALGORITHM CheckedCorrectedTreeBroadcast
namespace
{

TEST(ALGORITHM, Functional)
{

  NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 3).k(2).runtime(8)();   //  4
  NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 7).k(2).runtime(14)();  //  9
  NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 15).k(2).runtime(20)(); // 14

  FaultTest<ALGORITHM>().LogP(1, 1, 1, 3).k(2).
    failed({0}).runtime(0).unreach(2)();
  FaultTest<ALGORITHM>().LogP(1, 1, 1, 7).k(2).
    failed({0}).runtime(0).unreach(6)();
  FaultTest<ALGORITHM>().LogP(1, 1, 1, 15).k(2).
    failed({0}).runtime(0).unreach(14)();

  FaultTest<ALGORITHM>().LogP(1, 1, 1, 3).k(2).
    failed({1}).runtime(6).unreach(0)();                        // 3
  FaultTest<ALGORITHM>().LogP(1, 1, 1, 7).k(2).
    failed({1}).runtime(15).unreach(0)();                       // 9
//   FaultTest<ALGORITHM>().LogP(1, 1, 1, 15).k(2).
//     failed({1}).runtime(23???).unreach(0)();                       // 16

  // <TechnicalDetails>
  //
  // High level test which checks how the runtime and that all the
  // nodes which should finish are finished.
  //
  // </TechnicalDetails>
}

}
