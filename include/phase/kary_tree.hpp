#pragma once
#include "tree.hpp"

template <bool interleave>
class KAryTree : public Tree
{
  const size_t arity;
  void post_sends(const int sender, TaskQueue &tq) const;

public:
  KAryTree(ReachedNodes &reached_nodes, bool exit_on_corr = true)
    : Tree(reached_nodes, exit_on_corr),
      arity(Globals::get().conf().k)
  {
  }

  virtual Result dispatch(const InitTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const RecvEndTask& t, TaskQueue &tq, int node_id) override;

  virtual Time deadline() const override;
};
