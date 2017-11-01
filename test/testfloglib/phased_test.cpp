#include "gtest/gtest.h"

#include "phased_checked_correctedtree_bcast.hpp"
#include "globals.hpp"

namespace
{

TEST(PhasedCheckedCorrectedTree, Functional)
{

  {
    auto conf = Configuration(2, 100).LogP(1, 1, 1, 7).faults("none");
    auto model = Model(conf);

    Globals::set({&conf, &model});

    PhasedCheckedCorrectedTreeBroadcast coll;

    NoFaults faults;
    TaskQueue tq{&faults};
    Timeline timeline;

    tq.run(coll, timeline);

    EXPECT_EQ(timeline.get_total_time(), Time(12)) << conf;

    auto [failed, finished, unreached] = timeline.node_stat();
    EXPECT_EQ(failed, 0) << conf;
    EXPECT_EQ(finished, 7) << conf;
    EXPECT_EQ(unreached, 0) << conf;
  }

  {
    auto conf = Configuration(2, 100).LogP(1, 1, 1, 7);
    auto model = Model(conf);

    Globals::set({&conf, &model});

    PhasedCheckedCorrectedTreeBroadcast coll;

    UniformFaults faults({4});
    TaskQueue tq{&faults};
    Timeline timeline;

    tq.run(coll, timeline);

    EXPECT_EQ(timeline.get_total_time(), Time(15)) << conf;

    auto [failed, finished, unreached] = timeline.node_stat();
    EXPECT_EQ(failed, 1) << conf;
    EXPECT_EQ(finished, 6) << conf;
    EXPECT_EQ(unreached, 0) << conf;
  }
  // <TechnicalDetails>
  //
  // High level test which checks how the runtime and that all the
  // nodes which should finish are finished.
  //
  // </TechnicalDetails>
}

}
