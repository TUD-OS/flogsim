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

GossipPhase::GossipPhase(ReachedPtr reached_nodes)
  : Phase(reached_nodes),
    generator(generate_seed()),
    gossip_time(Globals::get().conf().k)
{
  assert(reached_nodes && (*reached_nodes)[0] && "Root unreached in tree");
}

void GossipPhase::post_sends(const int sender, TaskQueue &tq)
{
  if (tq.now() >= gossip_time) {
    return Result::DONE_PHASE;
  }

  // Pick an arbitrary node
  //
  // We do n-2, because n-1 is the maximum id, but also we exclude
  // ourselves
  std::uniform_int_distribution<int> distribution(0, num_nodes - 2);
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
  const int root = 0;
  assert(node_id == root && "GossipPhase init on non-root node");

  return post_sends(root, tq);
}

Phase::Result
GossipPhase::dispatch(const IdleTask &, TaskQueue &tq, int node_id)
{
  return post_sends(node_id, tq);
}
