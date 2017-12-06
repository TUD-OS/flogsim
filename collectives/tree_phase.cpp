#include <assert.h>
#include <cmath>

#include "task_queue.hpp"
#include "tree_phase.hpp"


TreePhase::TreePhase(ReachedPtr reached_nodes)
  : Phase(reached_nodes),
    arity(Globals::get().conf().k)
{
  assert(reached_nodes && (*reached_nodes)[0] && "Root unreached in tree");
}

template <bool interleave>
void
RegularTreePhase<interleave>::post_sends(const int sender, TaskQueue &tq) const
{
  const int lvl = static_cast<int>(std::log(sender + 1) / std::log(arity));

  for (int cc = 1; cc <= arity; cc++) {
    int receiver = interleave ? (sender + cc * std::pow(arity, lvl))
                              : (arity * sender + cc);

    if (receiver < num_nodes) {
      tq.schedule(SendStartTask::make_new(Tag::TREE, tq.now(), sender, receiver));
    }
  }
}

template <bool interleave>
Phase::Result
RegularTreePhase<interleave>::dispatch(const InitTask &, TaskQueue &tq, int node_id)
{
  const int root = 0;
  assert(node_id == root && "SimpleTreePhase init on non-root node");

  post_sends(root, tq);
  return Result::DONE_PHASE;
}

template <bool interleave>
Phase::Result
RegularTreePhase<interleave>::dispatch(const RecvEndTask &, TaskQueue &tq, int node_id)
{
  post_sends(node_id, tq);
  return Result::DONE_PHASE;
}


// explicit instantiation
template class RegularTreePhase<true>;
template class RegularTreePhase<false>;
