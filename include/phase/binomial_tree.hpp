#pragma once

#include "tree.hpp"

class BinomialTree : public Tree
{
  void post_sends(const int sender, TaskQueue &tq) const;
public:
  BinomialTree(ReachedNodes &reached_nodes)
    : Tree(reached_nodes)
  {
  }

  virtual Result dispatch(const InitTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const RecvEndTask& t, TaskQueue &tq, int node_id) override;

  virtual Time deadline() const override;
};
