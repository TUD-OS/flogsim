#include "phase/tree.hpp"

// required for explicit template instantiation
#include "topology/kary.hpp"
#include "topology/binomial.hpp"
#include "topology/optimal.hpp"
#include "topology/lame.hpp"

#include "task_queue.hpp"

using Result = Phase::Result;


template<typename T>
void Tree<T>::post_sends(const int sender, TaskQueue &tq) const
{
  for (int receiver: topology.receivers(sender)) {
    assert (receiver < num_nodes());
    tq.schedule(SendStartTask::make_new(Tag::TREE, tq.now(), sender, receiver));
  }
}

template<typename T>
Result Tree<T>::dispatch(const InitTask &, TaskQueue &tq, int node_id)
{
  if(!reached_nodes[node_id]) {
    return Result::ONGOING;
  }

  const int root [[maybe_unused]] = 0;
  assert(node_id == root && "SimpleTree init on non-root node");
  assert(reached_nodes[root] && "Root unreached in tree");

  post_sends(node_id, tq);

  return Result::DONE_PHASE;
}

template<typename T>
Result Tree<T>::dispatch(const FinishTask &, TaskQueue &tq, int node_id)
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

template<typename T>
Result Tree<T>::dispatch(const RecvEndTask &t, TaskQueue &tq, int node_id)
{
  reached_nodes[node_id] = true;
  post_sends(node_id, tq);

  return (t.tag() == Tag::TREE ?
                     Result::DONE_PHASE :
                     (exit_on_early_correction ?
                       Result::DONE_COLL :
                       Result::DONE_FORWARD));
}

// explicit instantiation
template class Tree<Lame>;
template class Tree<KAry>;
template class Tree<Optimal>;
template class Tree<Binomial>;
