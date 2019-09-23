#include "phase/gather.hpp"

// required for explicit template instantiation
#include "topology/binomial.hpp"
#include "topology/kary.hpp"
#include "topology/lame.hpp"
#include "topology/optimal.hpp"

#include "task_queue.hpp"

using Result = Phase::Result;

template <typename T>
Gather<T>::Gather(ReachedNodes &reached_nodes, NodeOrder order)
    : Phase(reached_nodes),
      to_receive(reached_nodes.size()),
      topology(reached_nodes.size(), order) {
  // Change traversal order
  topology.lookup_up();

  for (size_t receiver = 0; receiver < to_receive.size(); receiver++) {
    to_receive[receiver] = topology.senders(Rank(receiver)).size();
  }
}

template <typename T>
Result Gather<T>::post_sends(const int sender, TaskQueue &tq) {
  // Still need to receive some messages
  if (to_receive[sender] > 0) {
    return Result::ONGOING;
  }

  // ... or already sent everything
  assert(to_receive[sender] == 0 && "Shouldn't try to send at this point");

  for (Rank receiver : topology.receivers(Rank(sender))) {
    assert(receiver.get() < num_nodes());
    tq.schedule(SendStartTask::make_new(Tag::GATHER, tq.now(),
					sender, receiver.get()));
  }

  to_receive[sender]--;
  return Result::DONE_PHASE;
}

template <typename T>
Result Gather<T>::dispatch(const InitTask &, TaskQueue &tq, int node_id) {
  if (!reached_nodes[node_id]) {
    return Result::ONGOING;
  }

  const int root[[maybe_unused]] = 0;

  return post_sends(node_id, tq);
}

template <typename T>
Result Gather<T>::dispatch(const FinishTask &, TaskQueue &tq, int node_id) {
  // Goal is to record end of root
  if (node_id == 0) {
    auto &metrics = Globals::get().metrics();

    auto timeline = tq.timeline();

    Time gather_end(0);

    for (const auto &event : timeline.per_cpu_time[0].cpu_events.events()) {
      if (event.tag != Tag::GATHER) {
        continue;
      }

      gather_end = std::max(gather_end, event.end());
    }
    metrics["RootGatherEnd"] = gather_end.get();
  }

  return Result::ONGOING;
}

template <typename T>
Result Gather<T>::dispatch(const RecvEndTask &, TaskQueue &tq, int node_id) {
  reached_nodes[node_id] = true;

  // Received another message
  to_receive[node_id]--;
  return post_sends(node_id, tq);
}

// explicit instantiation
template class Gather<Lame>;
template class Gather<KAry>;
template class Gather<Optimal>;
template class Gather<Binomial>;
