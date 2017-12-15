#pragma once

#include <memory>
#include <vector>

#include "phase.hpp"

class CombinerPhase : public Phase
{
public:
  using PhasePtr = std::unique_ptr<Phase>;
  using PhaseVec = std::vector<PhasePtr>;

private:
  PhaseVec phases;
  std::vector<size_t> cur_phase;

  Result forward(const auto &t, TaskQueue &tq, const int node_id);

public:
  CombinerPhase(ReachedNodes &reached_nodes, PhaseVec &&phases);

  virtual Result dispatch(const InitTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const IdleTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const TimerTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const RecvEndTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const SendEndTask &t, TaskQueue &tq, int node_id) override;

  virtual Time deadline() const;
};
