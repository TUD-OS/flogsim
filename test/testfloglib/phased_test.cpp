#include "globals.hpp"

#include "test_wrappers.hpp"

#define ALGORITHM PhasedCheckedCorrectedTreeBroadcast
namespace
{

TEST(ALGORITHM, NoFaultTest_k_is_2)
{

  // // All is 1
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 1).k(2).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 3).k(2).runtime(8));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 4).k(2).runtime(10));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 5).k(2).runtime(11));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 7).k(2).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 8).k(2).runtime(13));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 13).k(2).runtime(15));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 15).k(2).runtime(16));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 16).k(2).runtime(16));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 21).k(2).runtime(18));

  // // L = 2
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 1, 1).k(2).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 1, 3).k(2).runtime(11));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 1, 4).k(2).runtime(14));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 1, 5).k(2).runtime(15));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 1, 7).k(2).runtime(16));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 1, 8).k(2).runtime(18));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 1, 13).k(2).runtime(20));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 1, 15).k(2).runtime(21));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 1, 16).k(2).runtime(22));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 1, 21).k(2).runtime(24));

  // // o = 2
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 1, 1).k(2).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 1, 3).k(2).runtime(15));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 1, 4).k(2).runtime(18));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 1, 5).k(2).runtime(20));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 1, 7).k(2).runtime(22));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 1, 8).k(2).runtime(23));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 1, 13).k(2).runtime(27));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 1, 15).k(2).runtime(29));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 1, 16).k(2).runtime(29));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 1, 21).k(2).runtime(32));

  // // g = 2
  // // Where should the phase end if g > 1 and o != g?
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 2, 1).k(2).runtime(0));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 2, 3).k(2).runtime(0));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 2, 4).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 2, 5).k(2).runtime(0));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 2, 7).k(2).runtime(0));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 2, 8).k(2).runtime(0));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 2, 13).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 2, 15).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 2, 16).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 2, 21).k(2).runtime(12));

  // // L = 2, o = 2
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 2, 1, 1).k(2).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 2, 1, 3).k(2).runtime(16));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 2, 1, 4).k(2).runtime(20));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 2, 1, 5).k(2).runtime(22));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 2, 1, 7).k(2).runtime(24));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 2, 1, 8).k(2).runtime(26));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 2, 1, 13).k(2).runtime(30));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 2, 1, 15).k(2).runtime(32));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 2, 1, 16).k(2).runtime(32));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 2, 1, 21).k(2).runtime(36));

  // // L = 2, g = 2
  // // Where should the phase end if g > 1 and o != g?
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 2, 1).k(2).runtime(0));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 2, 3).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 2, 4).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 2, 5).k(2).runtime(0));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 2, 7).k(2).runtime(0));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 2, 8).k(2).runtime(0));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 2, 13).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 2, 15).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 2, 16).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 2, 21).k(2).runtime(12));

  // // o = 2, g = 2
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 2, 1).k(2).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 2, 3).k(2).runtime(15));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 2, 4).k(2).runtime(18));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 2, 5).k(2).runtime(20));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 2, 7).k(2).runtime(22));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 2, 8).k(2).runtime(23));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 2, 13).k(2).runtime(27));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 2, 15).k(2).runtime(29));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 2, 16).k(2).runtime(29));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 2, 21).k(2).runtime(32));

  // // L = 3, o = 2
  // // CALL(NoFaultTest<ALGORITHM>().LogP(3, 2, 1, 1).k(2).runtime(0));
  // // It shouldn't send to the same node on different rings
  // // CALL(NoFaultTest<ALGORITHM>().LogP(3, 2, 1, 3).k(2).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 2, 1, 4).k(2).runtime(26));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 2, 1, 5).k(2).runtime(28));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 2, 1, 7).k(2).runtime(30));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 2, 1, 8).k(2).runtime(33));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 2, 1, 13).k(2).runtime(37));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 2, 1, 15).k(2).runtime(39));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 2, 1, 16).k(2).runtime(40));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 2, 1, 21).k(2).runtime(44));

  // // L = 3, g = 2
  // // CALL(NoFaultTest<ALGORITHM>().LogP(3, 1, 2, 1).k(2).runtime(0));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(3, 1, 2, 3).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(3, 1, 2, 4).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(3, 1, 2, 5).k(2).runtime(0));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(3, 1, 2, 7).k(2).runtime(0));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(3, 1, 2, 8).k(2).runtime(0));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(3, 1, 2, 13).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(3, 1, 2, 15).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(3, 1, 2, 16).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(3, 1, 2, 21).k(2).runtime(12));

  // // o = 3, L = 2
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 3, 1, 1).k(2).runtime(0));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 3, 1, 3).k(2).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 3, 1, 4).k(2).runtime(28));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 3, 1, 5).k(2).runtime(31));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 3, 1, 7).k(2).runtime(34));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 3, 1, 8).k(2).runtime(36));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 3, 1, 13).k(2).runtime(42));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 3, 1, 15).k(2).runtime(45));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 3, 1, 16).k(2).runtime(45));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 3, 1, 21).k(2).runtime(50));

  // // o = 3, g = 2
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 3, 2, 1).k(2).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 3, 2, 3).k(2).runtime(22));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 3, 2, 4).k(2).runtime(26));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 3, 2, 5).k(2).runtime(29));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 3, 2, 7).k(2).runtime(32));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 3, 2, 8).k(2).runtime(33));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 3, 2, 13).k(2).runtime(39));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 3, 2, 15).k(2).runtime(42));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 3, 2, 16).k(2).runtime(42));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 3, 2, 21).k(2).runtime(46));

  // // g = 3, L = 2
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 3, 1).k(2).runtime(0));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 3, 3).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 3, 4).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 3, 5).k(2).runtime(0));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 3, 7).k(2).runtime(0));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 3, 8).k(2).runtime(0));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 3, 13).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 3, 15).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 3, 16).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 3, 21).k(2).runtime(12));

  // // g = 3, o = 2
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 3, 1).k(2).runtime(0));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 3, 3).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 3, 4).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 3, 5).k(2).runtime(0));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 3, 7).k(2).runtime(0));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 3, 8).k(2).runtime(0));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 3, 13).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 3, 15).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 3, 16).k(2).runtime(12));
  // // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 3, 21).k(2).runtime(12));

  // <TechnicalDetails>
  //
  // Test which check functionality of fault fault-free case with
  // k-ary tree and k = 2.
  //
  // </TechnicalDetails>
}

TEST(ALGORITHM, NoFaultTest_k_is_3)
{

  // All is 1
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 1).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 3).k(3).runtime(8));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 4).k(3).runtime(10));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 5).k(3).runtime(11));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 7).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 8).k(3).runtime(13));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 13).k(3).runtime(15));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 15).k(3).runtime(16));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 16).k(3).runtime(16));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 1, 21).k(3).runtime(18));

  // L = 2
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 1, 1).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 1, 3).k(3).runtime(11));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 1, 4).k(3).runtime(14));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 1, 5).k(3).runtime(15));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 1, 7).k(3).runtime(16));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 1, 8).k(3).runtime(18));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 1, 13).k(3).runtime(20));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 1, 15).k(3).runtime(21));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 1, 16).k(3).runtime(22));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 1, 21).k(3).runtime(24));

  // o = 2
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 1, 1).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 1, 3).k(3).runtime(15));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 1, 4).k(3).runtime(18));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 1, 5).k(3).runtime(20));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 1, 7).k(3).runtime(22));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 1, 8).k(3).runtime(23));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 1, 13).k(3).runtime(27));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 1, 15).k(3).runtime(29));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 1, 16).k(3).runtime(29));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 1, 21).k(3).runtime(32));

  // g = 2
  // Where should the phase end if g > 1 and o != g?
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 2, 1).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 2, 3).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 2, 4).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 2, 5).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 2, 7).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 2, 8).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 2, 13).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 2, 15).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 2, 16).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 1, 2, 21).k(3).runtime(12));

  // L = 2, o = 2
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 2, 1, 1).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 2, 1, 3).k(3).runtime(16));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 2, 1, 4).k(3).runtime(20));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 2, 1, 5).k(3).runtime(22));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 2, 1, 7).k(3).runtime(24));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 2, 1, 8).k(3).runtime(26));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 2, 1, 13).k(3).runtime(30));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 2, 1, 15).k(3).runtime(32));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 2, 1, 16).k(3).runtime(32));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 2, 1, 21).k(3).runtime(36));

  // L = 2, g = 2
  // Where should the phase end if g > 1 and o != g?
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 2, 1).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 2, 3).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 2, 4).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 2, 5).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 2, 7).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 2, 8).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 2, 13).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 2, 15).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 2, 16).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 2, 21).k(3).runtime(12));

  // o = 2, g = 2
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 2, 1).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 2, 3).k(3).runtime(15));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 2, 4).k(3).runtime(18));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 2, 5).k(3).runtime(20));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 2, 7).k(3).runtime(22));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 2, 8).k(3).runtime(23));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 2, 13).k(3).runtime(27));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 2, 15).k(3).runtime(29));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 2, 16).k(3).runtime(29));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 2, 21).k(3).runtime(32));

  // L = 3, o = 2
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 2, 1, 1).k(3).runtime(0));
  // It shouldn't send to the same node on different rings
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 2, 1, 3).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 2, 1, 4).k(3).runtime(26));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 2, 1, 5).k(3).runtime(28));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 2, 1, 7).k(3).runtime(30));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 2, 1, 8).k(3).runtime(33));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 2, 1, 13).k(3).runtime(37));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 2, 1, 15).k(3).runtime(39));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 2, 1, 16).k(3).runtime(40));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 2, 1, 21).k(3).runtime(44));

  // L = 3, g = 2
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 1, 2, 1).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 1, 2, 3).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 1, 2, 4).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 1, 2, 5).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 1, 2, 7).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 1, 2, 8).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 1, 2, 13).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 1, 2, 15).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 1, 2, 16).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(3, 1, 2, 21).k(3).runtime(12));

  // o = 3, L = 2
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 3, 1, 1).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 3, 1, 3).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 3, 1, 4).k(3).runtime(28));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 3, 1, 5).k(3).runtime(31));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 3, 1, 7).k(3).runtime(34));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 3, 1, 8).k(3).runtime(36));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 3, 1, 13).k(3).runtime(42));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 3, 1, 15).k(3).runtime(45));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 3, 1, 16).k(3).runtime(45));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 3, 1, 21).k(3).runtime(50));

  // o = 3, g = 2
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 3, 2, 1).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 3, 2, 3).k(3).runtime(22));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 3, 2, 4).k(3).runtime(26));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 3, 2, 5).k(3).runtime(29));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 3, 2, 7).k(3).runtime(32));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 3, 2, 8).k(3).runtime(33));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 3, 2, 13).k(3).runtime(39));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 3, 2, 15).k(3).runtime(42));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 3, 2, 16).k(3).runtime(42));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 3, 2, 21).k(3).runtime(46));

  // g = 3, L = 2
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 3, 1).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 3, 3).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 3, 4).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 3, 5).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 3, 7).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 3, 8).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 3, 13).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 3, 15).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 3, 16).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(2, 1, 3, 21).k(3).runtime(12));

  // g = 3, o = 2
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 3, 1).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 3, 3).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 3, 4).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 3, 5).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 3, 7).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 3, 8).k(3).runtime(0));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 3, 13).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 3, 15).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 3, 16).k(3).runtime(12));
  // CALL(NoFaultTest<ALGORITHM>().LogP(1, 2, 3, 21).k(3).runtime(12));

  // <TechnicalDetails>
  //
  // Test which check functionality of fault fault-free case with
  // k-ary tree and k = 3.
  //
  // </TechnicalDetails>
}

TEST(ALGORITHM, FaultTest_k_is_2)
{
  // CALL(FaultTest<ALGORITHM>().LogP(1, 1, 1, 7).k(2).
  //      failed({4}).runtime(16));
  // CALL(FaultTest<ALGORITHM>().LogP(2, 1, 1, 15).k(2).
  //      failed({5, 12}).runtime(26));
}

}
