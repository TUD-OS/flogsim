#pragma once

#include "phase.hpp"
#include "topology/topology.hpp"
#include "topology/kary.hpp"
#include "topology/binomial.hpp"
#include "topology/lame.hpp"
#include "topology/optimal.hpp"

template<typename TOPOLOGY>
class Tree : public Phase
{
protected:
  bool exit_on_early_correction = true;
  void post_sends(const int sender, TaskQueue &tq) const;

  const TOPOLOGY topology;
public:
  Tree(ReachedNodes &reached_nodes, NodeOrder order = NodeOrder::INTERLEAVED) : 
    Phase(reached_nodes),
    topology(reached_nodes.size(), order)
  {
  }

  auto& forward_unexpected_message() {
    exit_on_early_correction = false;
    return *this;
  }

  Result dispatch(const InitTask&, TaskQueue&, int node_id) override final;
  Result dispatch(const FinishTask&, TaskQueue&, int node_id) override final;
  Result dispatch(const RecvEndTask&, TaskQueue&, int node_id) override final;

  Time deadline() const override final { return topology.deadline(); }
};
