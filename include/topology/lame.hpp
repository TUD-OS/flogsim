#pragma once

#include "topology/topology.hpp"

class Lame : public Topology
{
  // Table with cached ready to send count values
  mutable std::vector<int> rts_cache;
  int k;

  int compute_l_max(int n) const;
  // t as in the paper
  int ready_to_send(int t) const;
  // As start from the paper, compute initial t for a node
  int start(int id) const;
public:
  Lame(int num_nodes, NodeOrder order);

  Time __latency_at_node(int id, int i) const;
  Time latency_at_node(int id, int i) const;
  virtual Time deadline() const override;
};
