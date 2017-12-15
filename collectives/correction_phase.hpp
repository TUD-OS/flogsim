#pragma once

#include <vector>
#include <limits>

#include "phase.hpp"

class CorrectionPhase : public Phase
{
protected:
  enum Direction
  {
    DIR_LEFT  = -1,
    DIR_RIGHT = 1,
  };

public:
  CorrectionPhase(ReachedNodes &reached_nodes);
};


template<bool send_over_root>
class OpportunisticCorrectionPhase : public CorrectionPhase
{
  const int max_dist; // maximum distance to cover

public:
  OpportunisticCorrectionPhase(ReachedNodes &reached_nodes);

  virtual Phase::Result dispatch(const InitTask &, TaskQueue &tq, int node_id) override;
  virtual Phase::Result dispatch(const RecvEndTask &, TaskQueue &tq, int node_id) override;

  virtual Time deadline() const override;
};

template<bool send_over_root>
class CheckedCorrectionPhase : public CorrectionPhase
{
  // relevant information per direction (array) and node (vector)
  struct Ring
  {
    int offset;   // send offset
    int min_recv; // distance to closest node we received from

    Ring() : offset(0), min_recv(std::numeric_limits<int>::max())
    {}
  };

  std::vector<Ring> left;
  std::vector<Ring> right;

  Phase::Result post_message(TaskQueue &tq, int node_id);
public:
  CheckedCorrectionPhase(ReachedNodes &reached_nodes);

  virtual Phase::Result dispatch(const InitTask&, TaskQueue &tq, int node_id) override;
  virtual Phase::Result dispatch(const IdleTask&, TaskQueue &tq, int node_id) override;
  virtual Phase::Result dispatch(const RecvEndTask&, TaskQueue &tq, int node_id) override;
  virtual Phase::Result dispatch(const SendEndTask&, TaskQueue &tq, int node_id) override;
};
