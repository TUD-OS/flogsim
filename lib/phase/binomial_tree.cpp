#include <assert.h>
#include <cmath>

#include "task_queue.hpp"
#include "phase/binomial_tree.hpp"

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

// BinomialTree

void BinomialTree::post_sends(const int sender, TaskQueue &tq) const
{
  for (int lvl = get_lvl(sender); lvl <= get_lvl(num_nodes()); lvl++) {
    int receiver = sender + (1 << lvl);

    if (receiver < num_nodes()) {
      tq.schedule(SendStartTask::make_new(Tag::TREE, tq.now(), sender, receiver));
    }
  }
}

Phase::Result
BinomialTree::dispatch(const InitTask &, TaskQueue &tq, int node_id)
{
  if(!reached_nodes[node_id]) {
    return Result::ONGOING;
  }

  const int root [[maybe_unused]] = 0;
  assert(node_id == root && "SimpleTree init on non-root node");
  assert(reached_nodes[root] && "Root unreached in tree");

  post_sends(node_id, tq);

  return Result::DONE_PHASE;
}

Time BinomialTree::deadline() const
{
  auto &model = Globals::get().model();
  auto L = model.L;
  auto o = model.o;
  auto g = model.g;

  return binomial_runtime(L, o, g, num_nodes());
}
