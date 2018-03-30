#pragma once

#include "topology/topology.hpp"

class Binomial : public Topology
{
public:
  Binomial(int num_nodes, NodeOrder order);

  virtual Time deadline() const override;
};
