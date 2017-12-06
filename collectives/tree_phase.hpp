#pragma once

#include "globals.hpp"
#include "phase.hpp"

class TreePhase : public Phase
{
protected:
  const int arity;

public:
  TreePhase(ReachedPtr reached_nodes);
};


template <bool interleave>
class RegularTreePhase : public TreePhase
{
  void post_sends(const int sender, TaskQueue &tq) const;
public:
  RegularTreePhase(ReachedPtr reached_nodes)
    : TreePhase(reached_nodes)
  {
  }

  virtual Result dispatch(const InitTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const RecvEndTask& t, TaskQueue &tq, int node_id) override;
};
