#pragma once

#include "topology/topology.hpp"

class KAry : public Topology
{
  const size_t arity;

public:
  KAry(int num_nodes, NodeOrder order);
};
