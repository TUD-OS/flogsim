#include <assert.h>
#include <cmath>

#include "task_queue.hpp"

#include "phase/gossip.hpp"

Gossip::Gossip(ReachedNodes &reached_nodes)
  : Phase(reached_nodes),
    generator(Globals::get().entropy().generator),
    gossip_time(int(std::ceil(2 * std::log2(num_nodes()))) + Globals::get().conf().k),
    start_time(Time::max())
{
}

Phase::Result Gossip::post_sends(const int sender, TaskQueue &tq)
{
  if (tq.now() >= start_time + gossip_time) {
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
Gossip::dispatch(const InitTask &, TaskQueue &tq, int node_id)
{
  if (start_time == Time::max()) {
    start_time = tq.now();
  }

  const Time abs_deadline = start_time + gossip_time;

  if (abs_deadline <= tq.now()) {
    return Result::DONE_PHASE;
  }

  tq.schedule(TimerTask::make_new(abs_deadline, node_id));

  if(reached_nodes[node_id]) {
    tq.schedule(IdleTask::make_new(node_id));
  }

  return Result::ONGOING;
}

Phase::Result
Gossip::dispatch(const IdleTask &, TaskQueue &tq, int node_id)
{
  return post_sends(node_id, tq);
}

Phase::Result
Gossip::dispatch(const RecvEndTask &t, TaskQueue &tq, int node_id)
{
  if (t.tag() != Tag::GOSSIP) {
    return Result::DONE_COLL;
  }

  reached_nodes[node_id] = true;

  const Time abs_deadline = start_time + gossip_time;

  if (abs_deadline <= tq.now()) {
    return Result::DONE_PHASE;
  }

  tq.schedule(IdleTask::make_new(node_id));
  return Result::ONGOING;
}

Phase::Result
Gossip::dispatch(const SendEndTask &, TaskQueue &tq, int node_id)
{
  tq.schedule(IdleTask::make_new(node_id));
  return Result::ONGOING;
}

Time
Gossip::deadline() const
{
  return gossip_time;
}
