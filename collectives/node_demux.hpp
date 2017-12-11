#pragma once

#include <memory>
#include "collective.hpp"
#include "globals.hpp"
#include "reached_nodes.hpp"

class Phase;

/* Act as a collective and forward relevant Tasks to individual nodes */
class NodeDemux : public Collective
{
  std::unique_ptr<Phase> phase;

  void forward(const auto &t, TaskQueue &tq, const int node_id);

public:
  NodeDemux(std::unique_ptr<Phase> &&p);

  virtual void accept(const InitTask &t, TaskQueue &tq);
  virtual void accept(const TimerTask &t, TaskQueue &tq);
  virtual void accept(const IdleTask &t, TaskQueue &tq);
  virtual void accept(const SendEndTask &t, TaskQueue &tq);
  virtual void accept(const RecvEndTask& t, TaskQueue& tq);
};
