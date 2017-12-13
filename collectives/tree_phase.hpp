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


template <bool interleave>
class KAryTreePhase : public TreePhase
{
  const size_t arity;
  void post_sends(const int sender, TaskQueue &tq) const;

public:
  KAryTreePhase(ReachedNodes &reached_nodes)
    : TreePhase(reached_nodes),
      arity(Globals::get().conf().k)
  {
  }

  virtual Result dispatch(const InitTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const RecvEndTask& t, TaskQueue &tq, int node_id) override;

  virtual Time deadline() const override;
};

class BinomialTreePhase : public TreePhase
{
  void post_sends(const int sender, TaskQueue &tq) const;
public:
  BinomialTreePhase(ReachedNodes &reached_nodes)
    : TreePhase(reached_nodes)
  {
  }

  virtual Result dispatch(const InitTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const RecvEndTask& t, TaskQueue &tq, int node_id) override;

  virtual Time deadline() const override;
};

class OptimalTreePhase : public TreePhase
{
  std::vector<std::vector<int>> send_to;
  void post_sends(const int sender, TaskQueue &tq) const;
public:
  OptimalTreePhase(ReachedNodes &reached_nodes);

  virtual Result dispatch(const InitTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const RecvEndTask& t, TaskQueue &tq, int node_id) override;
};
