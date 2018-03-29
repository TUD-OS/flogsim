#pragma once

#include "phase.hpp"

template <typename CHILD>
class Tree : public Phase
{
protected:
  bool exit_on_early_correction = true;
  virtual void post_sends(const int sender, TaskQueue &tq) const = 0;

public:
  Tree(ReachedNodes &reached_nodes)
    : Phase(reached_nodes)
  {
  }

  CHILD& forward_unexpected_message() {
    exit_on_early_correction = false;
    return *static_cast<CHILD *>(this);
  }

  Result dispatch(const FinishTask&, TaskQueue&, int node_id) override final;
  Result dispatch(const RecvEndTask&, TaskQueue&, int node_id) override final;
};
