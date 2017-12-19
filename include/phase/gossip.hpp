#pragma once

#include <random>

#include "globals.hpp"
#include "phase.hpp"

class Gossip : public Phase
{
private:
  std::default_random_engine generator;
  const Time gossip_time;
  Time start_time;

  Result post_sends(const int sender, TaskQueue &tq);
public:
  Gossip(ReachedNodes &reached_nodes);

  virtual Result dispatch(const InitTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const IdleTask& t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const RecvEndTask& t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const SendEndTask& t, TaskQueue &tq, int node_id) override;

  virtual Time deadline() const override;
};
