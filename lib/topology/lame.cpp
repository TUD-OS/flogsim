#include <assert.h>
#include <cmath>

#include "task_queue.hpp"
#include "topology/lame.hpp"

#include <map>

// Lame

int Lame::compute_l_max(int n) const
{
  if (n == 0)
    return 0;

  int offs = ready_to_send(start(n) - 1);
  return 1 + compute_l_max(n - offs);
}

int Lame::ready_to_send(int t) const
{
  if (t < 0) {
    return 0;
  }

  if (t < k) {
    return 1;
  }

  unsigned idx = t - k;

  // If rts value is not known
  while (idx >= rts_cache.size()) {
    unsigned new_t = rts_cache.size() + k;
    int new_rts = ready_to_send(new_t - 1) + ready_to_send(new_t - k);
    rts_cache.push_back(new_rts);
  }

  return rts_cache[idx];
}

int Lame::start(int id) const
{
  if (id == 0) {
    return 0;
  }

  for (int t = 0;;t++) {
    if (ready_to_send(t) > id) {
      return t;
    }
  }
  // Not reached
}

Lame::Lame(int num_nodes, NodeOrder order) :
  Topology(num_nodes, order),
  k(Globals::get().conf().k)
{
  assert(order == NodeOrder::INTERLEAVED);

  for (int sender = 0; sender < num_nodes; sender++) {
    int lvl = start(sender);
    while (true) {
      int receiver = sender + ready_to_send(lvl + k - 1);
      if (receiver >= num_nodes) {
        break;
      }

      add_edge(Rank(sender), Rank(receiver));
      lvl ++;
    }
  }
  calc_leaves();
}

Time Lame::latency_at_node(int id, int t) const
{
  // vector of size time(t) of vectors of size num_nodes()
  static std::vector<std::vector<Time>> cache;

  while (cache.size() <= static_cast<size_t>(t)) {
    // Need to create cache for new time slot
    cache.push_back(std::vector<Time>(num_nodes()));
  }

  auto &timeslot_cache = cache[t];

  assert(timeslot_cache.size() > 0 && "Space should be allocated");

  Time result;

  // Empty value is 0
  if (timeslot_cache[id] == Time(0)) {
    // cache is empty
    result = __latency_at_node(id, t);
    // WARNING: we need to take address of cache[t] once again,
    // because it can change inside __latency_at_node
    cache[t][id] = result + Time(1);
  } else {
    result = timeslot_cache[id] - Time(1);
  }

  return result;
}

// Returns a latency at node *id*, who is about to send i-th message
// This is a recursive implementation, for what I try to find a closed
// form. For now it is good enough.
Time Lame::__latency_at_node(int id, int t) const
{
  auto &model = Globals::get().model();
  auto L = model.L;
  auto o = model.o;
  [[maybe_unused]] auto g = model.g;

  assert(o == g && "Model is expected to have o == g");

  if (id > ready_to_send(t)) {
    return Time(0);
  }

  Time local(0);
  if (id + ready_to_send(t + k - 1) < num_nodes()) {
    local = o + latency_at_node(id, t + 1);
  }

  int receiver = id + ready_to_send(t + k - 1);
  Time remote(0);
  if (receiver < num_nodes()) {
    remote = 2 * o + L + latency_at_node(receiver, t + k);
  }

  return std::max(local, remote);
}
