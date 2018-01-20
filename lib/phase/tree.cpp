#include "phase/tree.hpp"

// required for explicit template instantiation
#include "phase/kary_tree.hpp"
#include "phase/binomial_tree.hpp"
#include "phase/optimal_tree.hpp"
#include "phase/lame_tree.hpp"

#include "task_queue.hpp"

using Result = Phase::Result;

template <typename CHILD>
Result Tree<CHILD>::dispatch(const FinishTask &, TaskQueue &tq, int node_id)
{
  // Goal is to record end of root
  if (node_id == 0) {
    auto &metrics = Globals::get().metrics();

    auto timeline = tq.timeline();

    Time tree_end(0);

    for (const auto &event : timeline.per_cpu_time[0].cpu_events.events()) {
      if (event.tag != Tag::TREE) {
        continue;
      }

      tree_end = std::max(tree_end, event.end());
    }
    metrics["RootTreeEnd"] = tree_end.get();
  }

  return Result::ONGOING;
}

// explicit instantiation
template class Tree<LameTree>;
template class Tree<KAryTree>;
template class Tree<OptimalTree>;
template class Tree<BinomialTree>;
