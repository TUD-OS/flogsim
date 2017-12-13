#pragma once

#include "tree_phase.hpp"

class OptimalTreePhase : public TreePhase
{
  std::vector<std::vector<int>> send_to;
  void post_sends(const int sender, TaskQueue &tq) const;
public:
  OptimalTreePhase(ReachedNodes &reached_nodes);

  virtual Result dispatch(const InitTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const RecvEndTask& t, TaskQueue &tq, int node_id) override;
};
