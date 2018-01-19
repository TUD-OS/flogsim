#pragma once

#include "phase.hpp"

class Tree : public Phase
{
protected:
  const bool exit_on_early_correction;

public:
  Tree(ReachedNodes &reached_nodes, bool exit_on_corr = true)
    : Phase(reached_nodes), exit_on_early_correction(exit_on_corr)
  {
  }

  Result dispatch(const FinishTask &, TaskQueue &, int) override final;
};
