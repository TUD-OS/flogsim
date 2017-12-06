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
    int const recv = arity * sender + cc;
    if (recv < num_nodes) {
      tq.schedule(SendStartTask::make_new(Tag::TREE, tq.now(), sender, recv));
    }
  }
}

template <>
void
SimpleTreePhase<true>::post_sends(const int sender, TaskQueue &tq) const
{

      int lvl = int(std::log(node.id + 1) / std::log(coll.k));
      for (int i = 1; i <= coll.k; i++) {
        int receiver = node.id + i * std::pow(coll.k, lvl);
        if (receiver < coll.nodes) {
          node.send(tq, Tag::TREE, receiver);
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
