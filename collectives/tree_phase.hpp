#pragma once

#include "globals.hpp"
#include "phase.hpp"

class TreePhase : public Phase
{
protected:
  const int arity;

public:
  TreePhase()
    : arity(Globals::get().conf().k)
  {
  }
};


template <bool interleave>
class SimpleTreePhase : public TreePhase
{
  void post_sends(const int sender, TaskQueue &tq) const;
public:
  virtual Result do_phase(const InitTask &t, TaskQueue &tq, int node_id) override;
  virtual Result do_phase(const RecvEndTask& t, TaskQueue &tq, int node_id) override;
};
