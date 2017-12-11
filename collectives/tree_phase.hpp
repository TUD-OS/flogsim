#pragma once

#include "phase.hpp"

class TreePhase : public Phase
{
public:
  TreePhase(ReachedPtr reached_nodes)
    : Phase(reached_nodes)
  {
  }
};


template <bool interleave>
class KAryTreePhase : public TreePhase
{
  const size_t arity;
  void post_sends(const int sender, TaskQueue &tq) const;

public:
  KAryTreePhase(ReachedPtr reached_nodes, size_t arity)
    : TreePhase(reached_nodes),
      arity(arity)
  {
  }

  virtual Result dispatch(const InitTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const RecvEndTask& t, TaskQueue &tq, int node_id) override;

  virtual Time deadline() const override;
};

class BinomialTreePhase : public TreePhase
{
  Result post_sends(const int sender, TaskQueue &tq) const;
public:
  BinomialTreePhase(ReachedPtr reached_nodes)
    : TreePhase(reached_nodes)
  {
  }

  virtual Result dispatch(const InitTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const RecvEndTask& t, TaskQueue &tq, int node_id) override;
};

class OptimalTreePhase : public TreePhase
{
  std::vector<std::vector<int>> send_to;
  Result post_sends(const int sender, TaskQueue &tq) const;
public:
  OptimalTreePhase(ReachedPtr reached_nodes);

  virtual Result dispatch(const InitTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const RecvEndTask& t, TaskQueue &tq, int node_id) override;
};
