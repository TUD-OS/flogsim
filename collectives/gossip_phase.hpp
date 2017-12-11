#pragma once

#include <random>

#include "globals.hpp"
#include "phase.hpp"

class GossipPhase : public Phase
{
private:
  std::default_random_engine generator;
  const Time gossip_time;

  Result post_sends(const int sender, TaskQueue &tq);
public:
  GossipPhase(ReachedNodes &reached_nodes);

  virtual Result dispatch(const InitTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const IdleTask& t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const RecvEndTask& t, TaskQueue &tq, int node_id) override;
};
