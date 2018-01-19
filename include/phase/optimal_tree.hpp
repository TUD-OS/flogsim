#pragma once

#include "tree.hpp"

class OptimalTree : public Tree
{
  Time end_of_phase;
  std::vector<std::vector<int>> send_to;
  void post_sends(const int sender, TaskQueue &tq) const;
public:
  OptimalTree(ReachedNodes &reached_nodes, bool exit_on_corr = true);

  virtual Result dispatch(const InitTask &t, TaskQueue &tq, int node_id) override;
  virtual Result dispatch(const RecvEndTask& t, TaskQueue &tq, int node_id) override;

  virtual Time deadline() const override;
};
