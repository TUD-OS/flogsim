#include <assert.h>
#include <cmath>

#include "task_queue.hpp"
#include "tree_phase.hpp"

// Implementation of optimal tree broadcast from Section 2 "Optimal
// Broadcast and Summation in the LogP Model", Karp et. al.
namespace
{
struct Node
{
  Time label;
  int parent;
  int i;

  bool operator==(const Node &other) const
  {
    return (other.label == label) && (other.parent == parent) && (other.i == i);
  }

  bool operator<(const Node &other) const
  {
    if (label == other.label)
      return parent < other.parent;

    return label < other.label;
  }
};

#if 0
std::vector<Node> compute_opt_tree(Time L, Time o, Time g, int num_nodes)
{
  std::vector<Node> nodes;

  nodes.reserve(num_nodes);

  nodes.emplace_back(Node{Time(0), 0, 0});
  while (static_cast<int>(nodes.size()) < num_nodes) {
    int min_node = 0;
    Time min_time(Time::max());

    for (int cur_i = 0; cur_i < static_cast<int>(nodes.size()); cur_i++) {
      auto &cur = nodes[cur_i];

      if (min_time < cur.label) {
        break;
      }

      Time cur_time = cur.label + std::max(g, o) * cur.i + L + o * 2;
      if (cur_time < min_time) {
        min_node = cur_i;
        min_time = cur_time;
      }
    }

    nodes.push_back({min_time, min_node, 0});
    nodes[min_node].i++;
  }

  std::cout << "Norm" << std::endl;
  for (unsigned i = 0; i < nodes.size(); i++) {
    std::cout << nodes[i].label << " "
              << nodes[i].parent << " "
              << nodes[i].i << " " << std::endl;
  }

  return nodes;
}
#endif

std::vector<Node> compute_opt_tree(Time L, Time o, Time g, int num_nodes)
{
  std::vector<Node> nodes;

  nodes.reserve(num_nodes);

  auto time_predict = [&](const Node &cur) {
    return cur.label + std::max(g, o) * cur.i + L + o * 2;
  };

  int running_i = 0;

  nodes.emplace_back(Node{Time(0), 0, 0});
  nodes.push_back({time_predict(nodes[running_i]), running_i, 0});
  nodes[running_i].i++;

  while (static_cast<int>(nodes.size()) < num_nodes) {
    while (static_cast<int>(nodes.size()) < num_nodes) {
      int next_node = (running_i + 1) % nodes.size();

      Time time_cur = time_predict(nodes[running_i]);
      Time time_next = time_predict(nodes[next_node]);
      if (time_cur >= time_next) {
        break;
      }
      nodes.push_back({time_cur, running_i, 0});
      nodes[running_i].i++;
    }

    if (time_predict(nodes[0]) < time_predict(nodes[running_i])) {
      running_i--;
    } else {
      running_i++;
    }
  }

  // Sort to make arrows on the plot parallel and do not intersect
  std::sort(nodes.begin(), nodes.end());

  return nodes;
}

}

// OptimalTreePhase

OptimalTreePhase::OptimalTreePhase(ReachedNodes &reached_nodes)
  : TreePhase(reached_nodes)
{
  auto &model = Globals::get().model();
  auto L = model.L;
  auto o = model.o;
  auto g = model.g;

  std::vector<Node> nodes = compute_opt_tree(L, o, g, num_nodes());

  send_to.resize(num_nodes());
  for (int i = 0; i < static_cast<int>(nodes.size()); i++) {
    auto &cur = nodes[i];
    if (cur.parent != i) {
      send_to[cur.parent].push_back(i);
    }
  }
}

void OptimalTreePhase::post_sends(const int sender, TaskQueue &tq) const
{
  const auto &my_send_to = send_to[sender];
  for (auto receiver : my_send_to) {
    tq.schedule(SendStartTask::make_new(Tag::TREE, tq.now(), sender, receiver));
  }
}

Phase::Result
OptimalTreePhase::dispatch(const InitTask &, TaskQueue &tq, int node_id)
{
  assert(node_id == 0 && "TreePhase init on non-root node");

  post_sends(node_id, tq);

  return Result::DONE_PHASE;
}

Phase::Result
OptimalTreePhase::dispatch(const RecvEndTask &t, TaskQueue &tq, int node_id)
{
  reached_nodes[node_id] = true;

  post_sends(node_id, tq);

  if (t.tag() == Tag::TREE) {
    return Result::DONE_PHASE;
  } else {
    return Result::DONE_COLL;
  }
}
