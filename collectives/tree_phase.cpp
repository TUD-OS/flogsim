#include <assert.h>
#include <cmath>

#include "globals.hpp"
#include "task_queue.hpp"
#include "tree_phase.hpp"

using Result = Phase::Result;


namespace {
  // Helper function to calculate runtime of whole tree
  int calc_runtime(auto L, auto o, auto g, int P, int k)
  {
    if (P <= 1) {
      return 0;
    }

    if (P <= k + 1) {
      return std::max(o, g) * P + L;
    }

    int subtree_max(0);

    for (int i = 1; i <= k; i++) {
      int sub_P = (P - 1) / k + (i <= (P - 1) % k);
      int sub_time = calc_runtime(L, o, g, sub_P, k);
      subtree_max = std::max(subtree_max, std::max(o, g) * (i + 1) + L + sub_time);
    }

    return subtree_max;
  }
} // end anon namespace

template <bool interleave>
void
KAryTreePhase<interleave>::post_sends(const int sender, TaskQueue &tq) const
{
  const int lvl = static_cast<int>(std::log(sender + 1) / std::log(arity));

  for (size_t child = 1; child <= arity; ++child) {
    int receiver = interleave ? (sender + child * std::pow(arity, lvl))
                              : (arity * sender + child);

    if (receiver < num_nodes) {
      tq.schedule(SendStartTask::make_new(Tag::TREE, tq.now(), sender, receiver));
    }
  }
}

template <bool interleave>
Result
KAryTreePhase<interleave>::dispatch(const InitTask &, TaskQueue &tq, int node_id)
{
  const int root = 0;
  assert(node_id == root  && "TreePhase init on non-root node");
  assert(is_reached(root) && "Root unreached in tree");

  post_sends(root, tq);
  return Result::DONE_PHASE;
}

template <bool interleave>
Result
KAryTreePhase<interleave>::dispatch(const RecvEndTask &, TaskQueue &tq, int node_id)
{
  post_sends(node_id, tq);
  return Result::DONE_PHASE;
}

template <bool interleave>
Time
KAryTreePhase<interleave>::deadline(const int L, const int o, const int g) const
{
  return Time{calc_runtime(L, o, g, num_nodes, arity)};
}

// explicit instantiation
template class KAryTreePhase<true>;
template class KAryTreePhase<false>;
