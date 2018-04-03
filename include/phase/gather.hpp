#pragma once

#include "phase.hpp"
#include "topology/topology.hpp"
#include "topology/kary.hpp"
#include "topology/binomial.hpp"
#include "topology/lame.hpp"
#include "topology/optimal.hpp"

template<typename TOPOLOGY>
class Gather : public Phase
{
protected:
  std::vector<int> to_receive;
  Result post_sends(const int sender, TaskQueue &tq);

  TOPOLOGY topology;
public:
  Gather(ReachedNodes &reached_nodes, NodeOrder order = NodeOrder::INTERLEAVED);

  Result dispatch(const InitTask&, TaskQueue&, int node_id) override final;
  Result dispatch(const FinishTask&, TaskQueue&, int node_id) override final;
  Result dispatch(const RecvEndTask&, TaskQueue&, int node_id) override final;

  Time deadline() const override final { return topology.deadline(); }
};
