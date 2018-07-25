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
    generator(Globals::get().entropy().generator),
    tree_count(Globals::get().conf().k)
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
  std::uniform_int_distribution<int> distribution(1, num_nodes() - 1);

  // We send tree_count - 1 messages, to create tree_count - 1
  // tree. The last tree is started by the root
  std::set<int> other_roots;
  while (other_roots.size() < tree_count - 1) {
    int receiver = distribution(generator);
    other_roots.insert(receiver);
  }

  roots.push_back(0);
  for (int receiver : other_roots) {
    roots.push_back(receiver);
    tq.schedule(SendStartTask::make_new(Tag::GOSSIP, tq.now(), 0, receiver));
  }

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
