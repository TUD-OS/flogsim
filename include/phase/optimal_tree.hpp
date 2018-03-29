#pragma once

#include "tree.hpp"

class OptimalTree : public Tree<OptimalTree>
{
  Time end_of_phase;
  std::vector<std::vector<int>> send_to;
  void post_sends(const int sender, TaskQueue &tq) const override final;
public:
  OptimalTree(ReachedNodes &reached_nodes);

  virtual Time deadline() const override;
};
