#pragma once

#include <memory>
#include <vector>

#include "phase.hpp"

class CombinerPhase : public Phase
{
  using PhasePtr = std::unique_ptr<Phase>;
  using PhaseVec = std::vector<PhasePtr>;

  PhaseVec phases;
  std::vector<size_t> cur_phase;

  Result forward(const auto &t, TaskQueue &tq, const int node_id);

public:
  CombinerPhase(ReachedPtr reached_nodes, PhaseVec &&phases);

  virtual Result dispatch(const InitTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const IdleTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const TimerTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const RecvEndTask &t, TaskQueue &tq, int node_id) override;
};

// This class to be used by CombinerPhase as a kick-off to set up
// reach_nodes. It either sets up the root or enables
// everybody. Alternatively it accepts explicit list of reached nodes
// as an initializer list to be used in testing.
class StartPhase : public Phase
{
public:
  // Enable root
  StartPhase(Phase::ReachedPtr reached_nodes, int root)
    : Phase(reached_nodes)
  {
    (*reached_nodes)[root] = true;
  }

  // Enable everybody
  StartPhase(Phase::ReachedPtr reached_nodes)
    : Phase(reached_nodes)
  {
    std::fill(reached_nodes->begin(), reached_nodes->end(), true);
  }

  // Enable selected
  StartPhase(Phase::ReachedPtr reached_nodes, std::initializer_list<int> selected)
    : Phase(reached_nodes)
  {
    for (auto i : selected) {
      (*reached_nodes)[i] = true;
    }
  }

  virtual Result dispatch(const InitTask &, TaskQueue &tq, int node_id) override
  {
    return Result::DONE_PHASE;
  }
};
