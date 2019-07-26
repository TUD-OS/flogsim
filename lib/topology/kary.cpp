#include <assert.h>
#include <cmath>

#include "globals.hpp"
#include "task_queue.hpp"

#include "topology/kary.hpp"

using Result = Phase::Result;

namespace
{
// Helper function to calculate runtime of whole tree
Time calc_runtime(auto L, auto o, auto g, int P, int k)
{
  if (P <= 1) {
    return Time(0);
  }

  if (P <= k + 1) {
    return std::max(o, g) * P + L;
  }

  Time subtree_max(0);

  for (int i = 1; i <= k; i++) {
    int sub_P = (P - 1) / k + (i <= (P - 1) % k);
    Time sub_time = calc_runtime(L, o, g, sub_P, k);
    subtree_max = std::max(subtree_max, std::max(o, g) * (i + 1) + L + sub_time);
  }

  return subtree_max;
}

int get_lvl(int sender, int arity)
{
  int n_at_lvl = 1;
  int lvl = 0;

  while (sender >= n_at_lvl) {
    sender -= n_at_lvl;
    n_at_lvl *= arity;
    lvl ++;
  }
  return lvl;
}
} // end anon namespace

KAry::KAry(int num_nodes, NodeOrder order) :
  Topology(num_nodes, order),
  arity(Globals::get().conf().k)
{
  assert(order == NodeOrder::INTERLEAVED);

  for (int sender = 0; sender < num_nodes; sender++) {
    for (size_t child = 1; child <= arity; ++child) {
      int receiver = -1;

      switch (order) {
        case NodeOrder::INTERLEAVED: {
          const int lvl = get_lvl(sender, arity);
          receiver = sender + child * std::pow(arity, lvl);
          break;
        }
        case NodeOrder::INORDER: {
          receiver = arity * sender + child;
          break;
        }
      }

      if (receiver < num_nodes) {
        add_edge(Rank(sender), Rank(receiver));
      }
    }
  }
  calc_leaves();
}
