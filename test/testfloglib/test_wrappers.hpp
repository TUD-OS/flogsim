#pragma once

#include "gtest/gtest.h"

template<class T>
class Test
{
protected:
  int param_L, param_o, param_g;
  int param_P;
  // Arity
  int param_k;
  // Expected runtime
  int expect_runtime;
  // Expected number of unreached nodes
  int expect_unreach;
public:

  T &L(int L)
  {
    param_L = L;
    return *static_cast<T*>(this);
  }
  T &o(int o)
  {
    param_o = o;
    return *static_cast<T*>(this);
  }
  T &g(int g)
  {
    param_g = g;
    return *static_cast<T*>(this);
  }

  T &P(int P)
  {
    param_P = P;
    return *static_cast<T*>(this);
  }

  auto &LogP(int _L, int _o, int _g, int _P)
  {
    return L(_L).o(_o).g(_g).P(_P);
  }

  T &k(int k)
  {
    param_k = k;
    return *static_cast<T*>(this);
  }
  T &runtime(int t)
  {
    expect_runtime = t;
    return *static_cast<T*>(this);
  }
  T &unreach(int u)
  {
    expect_unreach = u;
    return *static_cast<T*>(this);
  }

  virtual void operator()() = 0;

  Test()
    : param_L(1),
      param_o(1),
      param_g(1),
      param_P(1),
      param_k(1),
      expect_runtime(0),
      expect_unreach(0)
  {}

};

template<class ALG>
class NoFaultTest : public Test<NoFaultTest<ALG>>
{
public:

  void operator()() override final
  {
    auto conf = Configuration(this->param_k,
                              this->expect_runtime + 1).
      LogP(this->param_L,
           this->param_o,
           this->param_g,
           this->param_P).faults("none");
    auto model = Model(conf);

    Globals::set({&conf, &model});

    ALG coll;

    NoFaults faults;
    TaskQueue tq{&faults};
    Timeline timeline;

    tq.run(coll, timeline);

    EXPECT_EQ(timeline.get_total_time(),
              Time(this->expect_runtime)) << conf;

    auto [failed, finished, unreached] = timeline.node_stat();
    EXPECT_EQ(failed, 0) << conf;
    EXPECT_EQ(finished, this->param_P) << conf;
    EXPECT_EQ(unreached, 0) << conf;
  }
};

template<class ALG>
class UniformFaultTest : public Test<UniformFaultTest<ALG>>
{
  // List of failed nodes
  std::vector<int> param_failed;
public:
  UniformFaultTest &failed(std::initializer_list<int> failed)
  {
    param_failed = std::vector<int>(failed);
    return *this;
  }

  void operator()() override
  {
    auto conf = Configuration(this->param_k,
                              this->expect_runtime + 10).
      LogP(this->param_L,
           this->param_o,
           this->param_g,
           this->param_P).
      faults("uniform", param_failed.size());
    auto model = Model(conf);

    Globals::set({&conf, &model});

    ALG coll;

    UniformFaults fi(param_failed);
    TaskQueue tq(&fi);
    Timeline timeline;

    tq.run(coll, timeline);

    EXPECT_EQ(timeline.get_total_time(),
              Time(this->expect_runtime)) << conf;

    auto [failed, finished, unreached] = timeline.node_stat();
    auto expect_finished = this->param_P - conf.F - this->expect_unreach;
    EXPECT_EQ(failed, conf.F) << conf;
    EXPECT_EQ(finished, expect_finished) << conf;
    EXPECT_EQ(unreached, this->expect_unreach) << conf;
  }
};
