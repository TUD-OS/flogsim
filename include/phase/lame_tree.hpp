#pragma once

#include "tree.hpp"

class LameTree : public Tree
{
  void post_sends(const int sender, TaskQueue &tq);
  // Table with cached ready to send count values
  mutable std::vector<int> rts_cache;
  int k;

  int compute_l_max(int n) const;
  // t as in the paper
  int ready_to_send(int t) const;
  // As start from the paper, compute initial t for a node
  int start(int id) const;
public:
  LameTree(ReachedNodes &reached_nodes)
    : Tree(reached_nodes),
      k(Globals::get().conf().k)
  {
  }

  virtual Result dispatch(const InitTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const RecvEndTask& t, TaskQueue &tq, int node_id) override;

  Time __latency_at_node(int id, int i) const;
  Time latency_at_node(int id, int i) const;
  virtual Time deadline() const override;
};
