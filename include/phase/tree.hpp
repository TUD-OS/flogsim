#pragma once

#include "phase.hpp"

class Tree : public Phase
{
public:
  Tree(ReachedNodes &reached_nodes)
    : Phase(reached_nodes)
  {
  }

  Result dispatch(const FinishTask &, TaskQueue &, int) override final;
};
