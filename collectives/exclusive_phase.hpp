#pragma once

#include "phase.hpp"

/* Wraps any other phase and makes sure it does not signal phase completion
 * ('DONE_PHASE') before its 'deadline' passed. Note that 'DONE_COLL' is
 * forwarded immediately.                                                     */
class ExclusivePhase : public Phase
{
public:
  using PhasePtr = std::unique_ptr<Phase>;

private:
  PhasePtr phase;
  Time start, rel_deadline;
  Result forward(const auto &t, TaskQueue &tq, const int node_id);

public:
  ExclusivePhase(ReachedNodes &reached_nodes, PhasePtr &&phase);

  virtual Result dispatch(const InitTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const IdleTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const TimerTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const RecvEndTask &t, TaskQueue &tq, int node_id) override;

  virtual Time deadline() const;
};
