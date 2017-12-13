#pragma once

#include "phase.hpp"

class TreePhase : public Phase
{
public:
  TreePhase(ReachedNodes &reached_nodes)
    : Phase(reached_nodes)
  {
  }
};
