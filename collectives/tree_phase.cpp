#include <assert.h>
#include <cmath>

#include "task_queue.hpp"
#include "tree_phase.hpp"


namespace {
}

template <>
void
SimpleTreePhase<false>::post_sends(const int sender, TaskQueue &tq) const
{
  for (int cc = 1; cc <= arity; ++cc) {
    int receiver = arity * sender + cc;
    if (receiver < num_nodes) {
      tq.schedule(SendStartTask::make_new(Tag::TREE, tq.now(), sender, receiver));
    }
  }
}

template <>
void
SimpleTreePhase<true>::post_sends(const int sender, TaskQueue &tq) const
{
  const int lvl = static_cast<int>(std::log(sender + 1) / std::log(arity));
  for (int i = 1; i <= arity; i++) {
    int receiver = sender + i * std::pow(arity, lvl);
    if (receiver < num_nodes) {
      tq.schedule(SendStartTask::make_new(Tag::TREE, tq.now(), sender, receiver));
    }
  }
}

template <bool interleave>
Phase::Result
SimpleTreePhase<interleave>::do_phase(const InitTask &, TaskQueue &tq, int node_id)
{
  const int root = 0;
  assert(node_id == root && "SimpleTreePhase init on non-root node");

  post_sends(root, tq);
  return Result::DONE_PHASE;
}

template <bool interleave>
Phase::Result
SimpleTreePhase<interleave>::do_phase(const RecvEndTask &, TaskQueue &tq, int node_id)
{
  post_sends(node_id, tq);
  return Result::DONE_PHASE;
}


// explicit instantiation
template class SimpleTreePhase<true>;
template class SimpleTreePhase<false>;
