#pragma once

#include <memory>
#include "collective.hpp"
#include "globals.hpp"
#include "reached_nodes.hpp"

class Phase;

/* Act as a collective and forward relevant Tasks to individual nodes */
//
// This class is responsible to set up reach_nodes. It either accepts
// explicit list of reached nodes as an initializer list (for example
// only root: {0}) or it enables everybody.
class NodeDemux : public Collective
{
private:
  std::unique_ptr<Phase> phase;

  void forward(const auto &t, TaskQueue &tq, const int node_id);

public:
  ReachedNodes reached_nodes;

  // Enable everybody
  NodeDemux()
    : reached_nodes(Globals::get().model().P)
  {
    reached_nodes.assign(reached_nodes.size(), true);
  }

  // Enable root selected
  NodeDemux(std::initializer_list<int> selected)
    : reached_nodes(Globals::get().model().P)
  {
    for (auto i : selected) {
      reached_nodes[i] = true;
    }
  }

  void set_phase(std::unique_ptr<Phase> &&_phase)
  {
    phase = std::move(_phase);
  }

  virtual void accept(const InitTask &t, TaskQueue &tq);
  virtual void accept(const TimerTask &t, TaskQueue &tq);
  virtual void accept(const IdleTask &t, TaskQueue &tq);
  virtual void accept(const SendEndTask &t, TaskQueue &tq);
  virtual void accept(const RecvEndTask& t, TaskQueue& tq);
};
