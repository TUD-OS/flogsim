#include <queue>

#include "globals.hpp"
#include "topology/topology.hpp"

struct Ready
{
  int node;
  Time time;
};

Time Topology::deadline() const
{
  
  auto &model = Globals::get().model();
  auto L = model.L;
  auto o = model.o;
  auto g = model.g;

  std::queue<Ready> wave;

  // Deadline does not depend on the traversal direction, meaning we always
  // can start from the root and go down
  wave.push({0, Time{0}});

  Time total{0};

  while (!wave.empty()) {
    Ready cur = wave.front();
    wave.pop();

    for (const auto &child : nodes[cur.node].children) {
      // Account for sender overhead/gap
      cur.time = cur.time + std::max(o, g);
      // Compute message arrival time
      Time completion = cur.time + L + std::max(o, g);
      total = std::max(total, completion);
      wave.push(Ready{child, completion});
    }
  }

  return total;
}