#include <assert.h>
#include <cmath>

#include "task_queue.hpp"
#include "topology/binomial.hpp"

namespace
{
int get_lvl(int sender)
{
  int lvl = 0;

  // Find most significant bit
  while (sender) {
    sender = sender / 2;
    lvl ++;
  }
  return lvl;
}

Time binomial_runtime(Time L, Time o, Time g, int P)
{

  if (P <=1) {
    return Time(0);
  }

  Time runtime = std::max(std::max(o, g) + L + o + binomial_runtime(L, o, g, P / 2),
                          std::max(o, g) + binomial_runtime(L, o, g, (P + 1) / 2));
  return runtime;
}

}

Binomial::Binomial(int num_nodes, NodeOrder order)
  : Topology(num_nodes, order)
{
  for (int sender = 0; sender < num_nodes; sender++) {
    switch (order) {
      case NodeOrder::INTERLEAVED: {
        for (int lvl = get_lvl(sender); lvl <= get_lvl(num_nodes); lvl++) {
          int receiver = sender + (1 << lvl);

          if (receiver < num_nodes) {
            add_edge(sender, receiver);
          }
        }
      }
      break;
      case NodeOrder::INORDER: {
        int mask = 1;
        // We create a vector first, because we want to create edges in
        // opposite order, so that the nodes sending more messages get a
        // messages earlier
        std::vector<int> receivers;
        while (mask < num_nodes) {
          int receiver = sender ^ mask;
          if (receiver < sender) {
            break;
          } else if (receiver < num_nodes) {
            receivers.push_back(receiver);
          }
          mask <<= 1;
        }

        // Push in opposite direction
        for (int i = receivers.size(); i > 0; i--) {
          add_edge(sender, receivers[i-1]);
        }
      }
      break;
    }
  }
}