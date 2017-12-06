#pragma once

// #include <vector>

#include "phase.hpp"

template<bool send_over_root>
class CorrectionPhase : public Phase
{
protected:
  enum Direction
  {
    DIR_LEFT  = -1,
    DIR_RIGHT = 1,
  };

public:
  CorrectionPhase(ReachedPtr reached_nodes);
};


template<bool send_over_root>
class OpportunisticCorrectionPhase
  : public CorrectionPhase<send_over_root>
{
public:
  OpportunisticCorrectionPhase(Phase::ReachedPtr reached_nodes)
    : CorrectionPhase<send_over_root>(reached_nodes)
  {
  }

  virtual Phase::Result dispatch(const InitTask &t, TaskQueue &tq, int node_id) override;
  virtual Phase::Result dispatch(const RecvEndTask& t, TaskQueue &tq, int node_id) override;
};


#if 0
template<bool send_over_root, int faults_to_tolerate>
class CheckedCorrectionPhase
  : public CorrectionPhase<send_over_root>
{
  // relevant information per direction (array) and node (vector)
  std::vector<int> offset[2];   // send offset
  std::vector<int> min_recv[2]; // distance to closest node we received from

public:
  CorrectionPhase()
  :
  {
    for (int dir = 0; dir <= 1; ++dir) {
      for (int node_id = 0; node_id < num_nodes; ++node_id) {
        offset[dir].emplace_back(1);           // start with immediate neighbour
        min_recv[dir].emplace_back(num_nodes); // max possible value (==self)
      }
    }
  }
};
#endif
