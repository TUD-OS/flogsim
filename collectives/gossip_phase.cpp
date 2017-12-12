#include <assert.h>
#include <cmath>
#include <chrono>

#include "task_queue.hpp"
#include "gossip_phase.hpp"

namespace
{
unsigned generate_seed()
{
  return std::chrono::system_clock::now().time_since_epoch().count();
}
}

GossipPhase::GossipPhase(ReachedNodes &reached_nodes)
  : Phase(reached_nodes),
    generator(generate_seed()),
    gossip_time(Globals::get().conf().k)
{
}

Phase::Result GossipPhase::post_sends(const int sender, TaskQueue &tq)
{
  if (tq.now() >= gossip_time) {
    return Result::DONE_PHASE;
  }

  // Pick an arbitrary node
  //
  // We do n-2, because n-1 is the maximum id, but also we exclude
  // ourselves
  std::uniform_int_distribution<int> distribution(0, num_nodes() - 2);
  int receiver = distribution(generator);

  if (receiver == sender) {
    receiver++;
  }

  // Send a message to it
  tq.schedule(SendStartTask::make_new(Tag::GOSSIP, tq.now(), sender, receiver));
  return Result::ONGOING;
}

Phase::Result
GossipPhase::dispatch(const InitTask &, TaskQueue &tq, int node_id)
{
  const int root [[maybe_unused]] = 0;
  assert(node_id == root && "GossipPhase init on non-root node");
  assert(reached_nodes[root] && "Root unreached in Gossip");

  return post_sends(node_id, tq);
}

Phase::Result
GossipPhase::dispatch(const IdleTask &, TaskQueue &tq, int node_id)
{
  return post_sends(node_id, tq);
}

Phase::Result
GossipPhase::dispatch(const RecvEndTask &, TaskQueue &, int node_id)
{
  reached_nodes[node_id] = true;
  return Result::ONGOING;
}

Time
GossipPhase::deadline() const
{
  return gossip_time;
}
