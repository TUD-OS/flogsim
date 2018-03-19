#include <assert.h>
#include <cmath>

#include "task_queue.hpp"
#include "phase/lame_tree.hpp"

#include <map>

// LameTree

int LameTree::compute_l_max(int n) const
{
  if (n == 0)
    return 0;

  int offs = ready_to_send(start(n) - 1);
  return 1 + compute_l_max(n - offs);
}

int LameTree::ready_to_send(int t) const
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

int LameTree::start(int id) const
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

void LameTree::post_sends(const int sender, TaskQueue &tq)
{
  reached_nodes[sender] = true;

  int lvl = start(sender);
  while (true) {
    int receiver = sender + ready_to_send(lvl + k - 1);
    if (receiver >= num_nodes()) {
      break;
    }

    tq.schedule(SendStartTask::make_new(Tag::TREE, tq.now(), sender, receiver));
    lvl ++;
  }
}

Phase::Result
LameTree::dispatch(const InitTask &, TaskQueue &tq, int node_id)
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

Phase::Result
LameTree::dispatch(const RecvEndTask &t, TaskQueue &tq, int node_id)
{
  post_sends(node_id, tq);

  return (t.tag() == Tag::TREE ?
                     Result::DONE_PHASE :
                     (exit_on_early_correction ?
                       Result::DONE_COLL :
                       Result::DONE_FORWARD));
}

Time LameTree::latency_at_node(int id, int t) const
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
Time LameTree::__latency_at_node(int id, int t) const
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

Time LameTree::deadline() const
{
  auto L_an = latency_at_node(0, 0);
  return L_an;

  auto &model = Globals::get().model();
  auto L = model.L;
  auto o = model.o;
  [[maybe_unused]] auto g = model.g;

  assert(o == g && "Model is expected to have o == g");
  // Maximum number of hops in the tree
  int height = compute_l_max(num_nodes());
  // Number of messages a root sends
  auto m_root = start(num_nodes() - 1) - k + 1;
  // Left over factor
  std::cout << "height = " << height << "  m_root = " << m_root << std::endl;
  Time L_rest(std::max(0L, (height - 1) * (L.get() - k + 2 * o.get())));
  std::cout << "fixed = " << o * (m_root + 1) + L << std::endl;
  return o * (m_root + 1) + L + L_rest;
}
