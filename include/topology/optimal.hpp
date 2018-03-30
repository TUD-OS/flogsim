#pragma once

#include "topology/topology.hpp"

class Optimal : public Topology
{
  Time end_of_phase;
  std::vector<std::vector<int>> send_to;
public:
  Optimal(int num_nodes, NodeOrder order);
};
