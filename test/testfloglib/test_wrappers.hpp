#pragma once

#include <functional>

#include "gtest/gtest.h"
#include "fault_injector.hpp"

#include "collective.hpp"
#include "phase.hpp"

class PhaseTest
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
  // List of failed nodes
  std::vector<int> param_failed;
  // Factory method for creating collectives
  std::function<std::unique_ptr<Phase>(ReachedNodes&)> create_coll;

  std::string make_command(const Configuration &conf, const FaultInjector &fi)
  {
    std::stringstream ss;

    ss << "./flogsim --k " << conf.k
       << " --L " << conf.L
       << " --o " << conf.o
       << " --g " << conf.g
       << " --P " << conf.P
       << " --faults " << fi
       << " --coll " << "whatever"
       << " --prio " << conf.priority;
    return ss.str();
  }

public:
  auto &init()
  {
    param_L = 1;
    param_o = 1;
    param_g = 1;
    param_P = 1;
    param_k = 1;
    expect_runtime = 0;
    expect_unreach = 0;
    param_failed = decltype(param_failed)();
    return *this;
  }

  auto &L(int L)
  {
    param_L = L;
    return *this;
  }

  auto &o(int o)
  {
    param_o = o;
    return *this;
  }

  auto &g(int g)
  {
    param_g = g;
    return *this;
  }

  auto &P(int P)
  {
    param_P = P;
    return *this;
  }

  auto &LogP(int _L, int _o, int _g, int _P)
  {
    return L(_L).o(_o).g(_g).P(_P);
  }

  auto &k(int k)
  {
    param_k = k;
    return *this;
  }

  auto &failed(std::initializer_list<int> fl)
  {
    param_failed = fl;
    return *this;
  }

  auto &runtime(int t)
  {
    expect_runtime = t;
    return *this;
  }

  auto &unreach(int u)
  {
    expect_unreach = u;
    return *this;
  }

  PhaseTest(auto create_coll)
    : create_coll(create_coll)
  {
    init();
  }

  void operator()(int linenum)
  {
    auto conf = Configuration(this->param_k,
                              this->expect_runtime + 10).
      LogP(this->param_L,
           this->param_o,
           this->param_g,
           this->param_P).faults("none");
    auto model = Model(conf);

    Globals::set({&conf, &model});

    auto coll = Collective({0}, std::make_unique<ListFaults>(this->param_failed));

    auto timeline = coll.run(this->create_coll(coll.reached_nodes));

    auto cmd = this->make_command(conf, *coll.faults.get());
    std::stringstream info;
    info << conf << std::endl
         << "Test case from line: " << linenum << std::endl
         << "Suggested command:" << std::endl
         << cmd;
    auto info_str = info.str();

    EXPECT_EQ(timeline.get_total_time(),
              Time(this->expect_runtime)) << info_str;

    auto [failed, finished, unreached] = timeline.node_stat();
    int fault_count = coll.faults->fault_count();
    auto expect_finished = (this->param_P - fault_count -
                            this->expect_unreach);
    EXPECT_EQ(failed, fault_count) << info_str;
    EXPECT_EQ(finished, expect_finished) << info_str;
    EXPECT_EQ(unreached, this->expect_unreach) << info_str;
  }

};

#define CALL(t, x)                              \
  ({                                            \
    t.init().x(__LINE__);                       \
  })
