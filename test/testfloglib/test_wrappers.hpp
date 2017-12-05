#pragma once

#include "gtest/gtest.h"

template<template<class> class T, class ALG>
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

  std::string make_command(const Configuration &conf, const FaultInjector &fi)
  {
    std::stringstream ss;

    ss << "./flogsim --k " << conf.k
       << " --L " << conf.L
       << " --o " << conf.o
       << " --g " << conf.g
       << " --P " << conf.P
       << " --faults " << fi
       << " --coll " << ALG::name;
    return ss.str();
  }
public:

  auto &L(int L)
  {
    param_L = L;
    return *static_cast<T<ALG>*>(this);
  }

  auto &o(int o)
  {
    param_o = o;
    return *static_cast<T<ALG>*>(this);
  }

  auto &g(int g)
  {
    param_g = g;
    return *static_cast<T<ALG>*>(this);
  }

  auto &P(int P)
  {
    param_P = P;
    return *static_cast<T<ALG>*>(this);
  }

  auto &LogP(int _L, int _o, int _g, int _P)
  {
    return L(_L).o(_o).g(_g).P(_P);
  }

  auto &k(int k)
  {
    param_k = k;
    return *static_cast<T<ALG>*>(this);
  }

  auto &runtime(int t)
  {
    expect_runtime = t;
    return *static_cast<T<ALG>*>(this);
  }

  auto &unreach(int u)
  {
    expect_unreach = u;
    return *static_cast<T<ALG>*>(this);
  }

  virtual void operator()(int) = 0;

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
class NoFaultTest : public Test<NoFaultTest, ALG>
{
public:

  void operator()(int linenum) override final
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

    auto cmd = this->make_command(conf, faults);
    std::stringstream info;
    info << conf << std::endl
         << "Test case from line: " << linenum << std::endl
         << "Suggested command:" << std::endl
         << cmd;
    auto info_str = info.str();

    EXPECT_EQ(timeline.get_total_time(),
              Time(this->expect_runtime)) << info_str;

    auto [failed, finished, unreached] = timeline.node_stat();
    EXPECT_EQ(failed, 0) << info_str;
    EXPECT_EQ(finished, this->param_P) << info_str;
    EXPECT_EQ(unreached, 0) << info_str;
  }
};

template<class ALG>
class FaultTest : public Test<FaultTest, ALG>
{
  // List of failed nodes
  std::vector<int> param_failed;
public:
  FaultTest &failed(std::initializer_list<int> failed)
  {
    param_failed = std::vector<int>(failed);
    return *this;
  }

  void operator()(int linenum) override final
  {
    auto conf = Configuration(this->param_k,
                              this->expect_runtime + 10).
      LogP(this->param_L,
           this->param_o,
           this->param_g,
           this->param_P);
    auto model = Model(conf);


    Globals::set({&conf, &model});

    ALG coll;

    ListFaults fi(param_failed);
    TaskQueue tq(&fi);
    Timeline timeline;


    tq.run(coll, timeline);

    auto cmd = this->make_command(conf, fi);
    std::stringstream info;
    info << conf << fi << std::endl
         << "Test case from line: " << linenum << std::endl
         << "Suggested command:" << std::endl
         << cmd;
    auto info_str = info.str();

    EXPECT_EQ(timeline.get_total_time(),
              Time(this->expect_runtime)) << info_str;

    auto [failed, finished, unreached] = timeline.node_stat();
    auto expect_finished = (this->param_P - fi.fault_count() -
                            this->expect_unreach);
    EXPECT_EQ(failed, fi.fault_count()) << info_str;
    EXPECT_EQ(finished, expect_finished) << info_str;
    EXPECT_EQ(unreached, this->expect_unreach) << info_str;
  }
};

#define CALL(x)                                 \
  ({                                            \
    x(__LINE__);                                \
  })
