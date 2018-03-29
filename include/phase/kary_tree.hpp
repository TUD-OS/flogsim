#pragma once

#include "tree.hpp"

class KAryTree : public Tree<KAryTree>
{
  const size_t arity;
  void post_sends(const int sender, TaskQueue &tq) const override final;

public:
  KAryTree(ReachedNodes &reached_nodes)
    : Tree<KAryTree>(reached_nodes),
      arity(Globals::get().conf().k)
  {
  }

  virtual Time deadline() const override;
};
