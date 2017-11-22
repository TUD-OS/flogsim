/*                                  arity \   / expected runtime           */
/*                            L  o  g  P  |   |                            */
/*                            |  |  |  |  |   |                            */
#define MAKE_NOFAULT_TESTCASE(L, o, g, P, k, TIME)                          \
{                                                                           \
    auto conf = Configuration(k, TIME + 1).LogP(L, o, g, P).faults("none"); \
    auto model = Model(conf);                                               \
                                                                            \
    Globals::set({&conf, &model});                                          \
                                                                            \
    ALGORITHM coll;                                                         \
                                                                            \
    NoFaults faults;                                                        \
    TaskQueue tq{&faults};                                                  \
    Timeline timeline;                                                      \
                                                                            \
    tq.run(coll, timeline);                                                 \
                                                                            \
    EXPECT_EQ(timeline.get_total_time(), Time(TIME)) << conf;               \
                                                                            \
    auto [failed, finished, unreached] = timeline.node_stat();              \
    EXPECT_EQ(failed, 0) << conf;                                           \
    EXPECT_EQ(finished, P) << conf;                                         \
    EXPECT_EQ(unreached, 0) << conf;                                        \
}
/*                                  arity
 *                                       \     -> list of failed nodes
 *                            L  o  g  P  \   |   -> expected runtime
 *                             \  \  \  \  \  |  |          -> unreached
 *                              \  \  \  \  \  \  \        | (non-failed)
 *                               \  \  \  \  \  \  \______  \___ nodes
 *                                \  \  \  \  \  \        \     \        */
#define MAKE_UNIFORMFAULT_TESTCASE(L, o, g, P, k, FAILERS, TIME, UNREACH)  \
{                                                                          \
    std::vector<int> f = FAILERS ;                                         \
    auto conf = Configuration(k, TIME + 10).                               \
      LogP(L, o, g, P).faults("uniform", f.size());                        \
    auto model = Model(conf);                                              \
                                                                           \
    Globals::set({&conf, &model});                                         \
                                                                           \
    ALGORITHM coll;                                                        \
                                                                           \
    UniformFaults fi(f);                                                   \
    TaskQueue tq(&fi);                                                     \
    Timeline timeline;                                                     \
                                                                           \
    tq.run(coll, timeline);                                                \
                                                                           \
    EXPECT_EQ(timeline.get_total_time(), Time(TIME)) << conf;              \
                                                                           \
    auto [failed, finished, unreached] = timeline.node_stat();             \
    auto expect_finished = P - conf.F - UNREACH;                           \
    EXPECT_EQ(failed, conf.F) << conf;                                     \
    EXPECT_EQ(finished, expect_finished) << conf;                          \
    EXPECT_EQ(unreached, UNREACH) << conf;                                 \
}
