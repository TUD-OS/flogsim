#include "gtest/gtest.h"

#include "simple_tree_bcast.hpp"
#include "globals.hpp"

namespace
{

TEST(SimpleTree, Functional)
{

  { //                  arity \   / time limit
    //                        |   |        L  o  g  P
    //                        |   |        |  |  |  |
    auto conf = Configuration(3, 100).LogP(1, 1, 1, 13).faults("none");
    auto model = Model(conf);

    Globals::set({&conf, &model});

    SimpleTreeBroadcast coll;

    UniformFaults faults({});
    TaskQueue tq{&faults};
    Timeline timeline;

    tq.run(coll, timeline);

    EXPECT_EQ(timeline.get_total_time(), Time(10)) << conf;

    auto [failed, finished, unreached] = timeline.node_stat();
    EXPECT_EQ(failed, Globals::get().conf().F) << conf;
    EXPECT_EQ(finished, Globals::get().conf().P) << conf;
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
