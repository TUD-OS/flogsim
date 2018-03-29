#pragma once

#include "tree.hpp"

class BinomialTree : public Tree<BinomialTree>
{
  void post_sends(const int sender, TaskQueue &tq) const override final;
public:
  BinomialTree(ReachedNodes &reached_nodes)
    : Tree(reached_nodes)
  {
  }

  virtual Result dispatch(const InitTask &t, TaskQueue &tq, int node_id) override;

  virtual Time deadline() const override;
};
