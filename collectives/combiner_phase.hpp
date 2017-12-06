#pragma once

#include <memory>
#include <vector>

#include "phase.hpp"

class CombinerPhase : public Phase
{
  using PhasePtr = std::unique_ptr<Phase>;

  PhasePtr phases[2];
  std::vector<int> cur_phase;

public:
  CombinerPhase(ReachedPtr reached_nodes, PhasePtr &&p1, PhasePtr &&p2)
    : Phase(reached_nodes),
      phases{std::move(p1), std::move(p2)},
      cur_phase(num_nodes, 0)
  {
  }
};



// - keep current phase
// - forward to current phase
// - delay next phase (estimate current phase length -> phase.interface (optional))
// - skip phase on DONE_COLL

