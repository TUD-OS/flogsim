#pragma once

#include "tree_phase.hpp"

class BinomialTreePhase : public TreePhase
{
  void post_sends(const int sender, TaskQueue &tq) const;
public:
  BinomialTreePhase(ReachedNodes &reached_nodes)
    : TreePhase(reached_nodes)
  {
  }

  virtual Result dispatch(const InitTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const RecvEndTask& t, TaskQueue &tq, int node_id) override;

  virtual Time deadline() const override;
};
