#include <assert.h>
#include <cmath>

// required for explicit template instantiation
#include "topology/kary.hpp"
#include "topology/binomial.hpp"
#include "topology/optimal.hpp"
#include "topology/lame.hpp"

#include "task_queue.hpp"

#include "phase/multitree.hpp"

using Result = Phase::Result;

template<typename T>
MultiTree<T>::MultiTree(ReachedNodes &reached_nodes)
  : Phase(reached_nodes),
    topology(reached_nodes.size(), NodeOrder::INTERLEAVED),
    tree_count(Globals::get().conf().d)
{
}

template<typename T>
void MultiTree<T>::post_sends(const int sender, TaskQueue &tq) const
{
  // For each tree
  for (int root : roots) {
    // Send tree messages
    for (Rank receiver: topology.receivers(to_virtual(sender, root))) {
      assert (receiver.get() < num_nodes());
      tq.schedule(SendStartTask::make_new(Tag::TREE, tq.now(), sender, to_real(receiver, root)));
    }
  }
}

template<typename T>
Result MultiTree<T>::dispatch(const InitTask &, TaskQueue &tq, int node_id)
{
  if(!reached_nodes[node_id]) {
    return Result::ONGOING;
  }

  // Find how many non-leaf nodes a topology has, assuming that all
  // the leaf nodes have higher ranks
  int inner_count = 1;
  while (inner_count < num_nodes()) {
    if (topology.receivers(Rank(inner_count)).size() == 0) {
      break;
    }
    inner_count++;
  }

  // If leaf nodes are less than half, something is broken
  assert(inner_count <= num_nodes() / 2);

  int cur_root = inner_count;
  for (unsigned i = 1; i < tree_count; i++) {
    if (cur_root + inner_count > num_nodes()) {
      throw std::runtime_error("Tree fan out is too low");
    }
    roots.push_back(cur_root);
    cur_root += inner_count;
  }

  for (int receiver : roots) {
    tq.schedule(SendStartTask::make_new(Tag::GOSSIP, tq.now(), 0, receiver));
  }

  // Ther Rank 0 is the root of the first tree, but we need to add it
  // after sending "Gossip" messages
  roots.insert(roots.begin(), 0);

  post_sends(0, tq);
  return Result::DONE_PHASE;
}

template<typename T>
Result MultiTree<T>::dispatch(const RecvEndTask &, TaskQueue &tq, int node_id)
{
  if(reached_nodes[node_id]) {
    return Result::DONE_PHASE;
  }

  reached_nodes[node_id] = true;
  post_sends(node_id, tq);

  return Result::DONE_PHASE;
}

template<typename T>
Result MultiTree<T>::dispatch(const FinishTask &, TaskQueue &tq, int node_id)
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
template class MultiTree<Binomial>;
template class MultiTree<KAry>;
