#include <assert.h>
#include <cmath>

#include "task_queue.hpp"
#include "tree_phase.hpp"

namespace
{
int get_lvl(int sender)
{
  int lvl = 0;

  // Find most significant bit
  while (sender) {
    sender = sender / 2;
    lvl ++;
  }
  return lvl;
}
}

// BinomialTreePhase

Phase::Result
BinomialTreePhase::post_sends(const int sender, TaskQueue &tq) const
{
  for (int lvl = get_lvl(sender); lvl <= get_lvl(num_nodes()); lvl++) {
    int receiver = sender + (1 << lvl);

    if (receiver < num_nodes()) {
      tq.schedule(SendStartTask::make_new(Tag::TREE, tq.now(), sender, receiver));
    }
  }
  return Result::DONE_PHASE;
}

Phase::Result
BinomialTreePhase::dispatch(const InitTask &, TaskQueue &tq, int node_id)
{
  const int root [[maybe_unused]] = 0;
  assert(node_id == root && "SimpleTreePhase init on non-root node");
  assert(reached_nodes[root] && "Root unreached in tree");

  return post_sends(node_id, tq);
}

Phase::Result
BinomialTreePhase::dispatch(const RecvEndTask &, TaskQueue &tq, int node_id)
{
  return post_sends(node_id, tq);
}
