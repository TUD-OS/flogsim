#pragma once

#include <functional>
#include <fstream>

#include <experimental/filesystem>

#include "gtest/gtest.h"
#include "fault_injector.hpp"

#include "collective.hpp"
#include "phase.hpp"

using create_coll_t = std::function<std::unique_ptr<Phase>(ReachedNodes&)>;

struct RunParams
{
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
  create_coll_t create_coll;

  friend std::ostream &operator<<(std::ostream &os, const RunParams &rp)
  {
    os << "./flogsim --k " << rp.param_k
       << " --L " << rp.param_L
       << " --o " << rp.param_o
       << " --g " << rp.param_g
       << " --P " << rp.param_P
       << " --coll " << "whatever";
    return os;
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

  RunParams(const RunParams&) = default;
  RunParams(RunParams* const rp) : RunParams(*rp) {}

  RunParams()
    : param_L(1),
      param_o(1),
      param_g(1),
      param_P(1),
      param_k(1),
      expect_runtime(0),
      expect_unreach(0),
      create_coll(nullptr)
  {}

};

namespace std
{
template<>
struct hash<RunParams>
{
  typedef RunParams argument_type;
  typedef std::size_t result_type;
  result_type operator()(argument_type const &rp) const noexcept
  {
    result_type h = ((std::hash<int>{}(rp.param_L)) ^
                     (std::hash<int>{}(rp.param_o) << 1) ^
                     (std::hash<int>{}(rp.param_g) << 2) ^
                     (std::hash<int>{}(rp.param_P) << 3) ^
                     (std::hash<int>{}(rp.param_k) << 3));
    std::for_each(rp.param_failed.begin(), rp.param_failed.end(),
                  [&](const auto &e) {
                    h = (h << 1) ^ std::hash<int>{}(e);
                  });
    typedef void function_t(void) ;
    h = (h << 1) ^ std::hash<function_t*>{}(rp.create_coll.target<function_t>());
    return h;
  }
};
}

class PhaseTest : public ::testing::TestWithParam<RunParams>
{
public:
  void do_test(create_coll_t create_coll)
  {
    namespace fs = std::experimental::filesystem;

    RunParams param = GetParam();

    param.create_coll = create_coll;

    fs::path log_dir = "./logs/";
    std::string log_prefix = log_dir.c_str() + std::to_string(std::hash<RunParams>{}(param));
    fs::path trace_filename(log_prefix + ".trace.csv");
    fs::path model_filename(log_prefix + ".model.csv");

    std::string info("Check " +
                     trace_filename.native() + " and " +
                     model_filename.native());

    auto conf = Configuration(param.param_k, param.expect_runtime + 10).
      LogP(param.param_L,
           param.param_o,
           param.param_g,
           param.param_P).faults("none");

    auto model = Model(conf);

    Globals::set({&conf, &model});

    auto coll = Collective({0}, std::make_unique<ListFaults>(param.param_failed));

    auto timeline = coll.run(create_coll(coll.reached_nodes));

    EXPECT_EQ(timeline.get_total_time(),
              Time(param.expect_runtime)) << info;

    auto [failed, finished, unreached] = timeline.node_stat();
    int fault_count = coll.faults->fault_count();
    auto expect_finished = (param.param_P - fault_count -
                            param.expect_unreach);
    EXPECT_EQ(failed, fault_count) << info;
    EXPECT_EQ(finished, expect_finished) << info;
    EXPECT_EQ(unreached, param.expect_unreach) << info;

    if (!fs::exists(log_dir)) {
      fs::create_directory(log_dir);
    } else if (!fs::is_directory(log_dir)) {
      FAIL() << "Not a log directory: " << log_dir;
    }
    std::ofstream trace_log(trace_filename);
    trace_log << timeline;

    std::ofstream model_log(model_filename);
    model_log << model;
  }
};
