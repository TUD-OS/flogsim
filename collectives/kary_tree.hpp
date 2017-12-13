#pragma once
#include "tree_phase.hpp"

template <bool interleave>
class KAryTreePhase : public TreePhase
{
  const size_t arity;
  void post_sends(const int sender, TaskQueue &tq) const;

public:
  KAryTreePhase(ReachedNodes &reached_nodes)
    : TreePhase(reached_nodes),
      arity(Globals::get().conf().k)
  {
  }

  virtual Result dispatch(const InitTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const RecvEndTask& t, TaskQueue &tq, int node_id) override;

  virtual Time deadline() const override;
};
