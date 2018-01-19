#pragma once

#include "phase.hpp"

class Tree : public Phase
{
protected:
  bool exit_on_early_correction = true;

public:
  Tree(ReachedNodes &reached_nodes)
    : Phase(reached_nodes)
  {
  }

  void forward_unexpected() {
    exit_on_early_correction = false;
  }

  Result dispatch(const FinishTask &, TaskQueue &, int) override final;
};
