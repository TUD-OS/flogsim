#pragma once

#include "phase.hpp"

/* Multiphase wraps any phase P. Then it randomly selects a subset of
 * initial nodes I. Then, the root sends a message to each node in
 * I. Then, each node in I becomes a root for an instance of P.
 *
 * As result, Multiphase runs multiple concurrent instances of P
 */
template<typename TOPOLOGY>
class MultiTree : public Phase
{
private:
  const TOPOLOGY topology;
  size_t tree_count;

  std::vector<int> roots;
  void post_sends(const int sender, TaskQueue &tq) const;

  int to_real(Rank rank, int root) const {
    return (rank.get() + root) % reached_nodes.size();
  }

  Rank to_virtual(int rank, int root) const {
    return Rank((rank - root + reached_nodes.size()) % reached_nodes.size());
  }

public:
  MultiTree(ReachedNodes &reached_nodes);

  virtual Result dispatch(const InitTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const RecvEndTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const FinishTask &t, TaskQueue &tq, int node_id) override;
};
