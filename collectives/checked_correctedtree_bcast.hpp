#include <algorithm>

#include <cmath>

#include "collective.hpp"
#include "task_queue.hpp"

class CheckedCorrectedTreeBroadcast : public Collective
{
  int k;
  int nodes;

  std::vector<bool> tree_recv;
  std::vector<bool> tree_sent;
  std::vector<bool> left_done;
  std::vector<bool> right_done;
  std::vector<int> left_offs;
  std::vector<int> right_offs;
  std::vector<bool> all_done;
  std::vector<int> right_min_recv; // closest sender from right with "left" tag
  std::vector<int> left_min_recv; // closest sender from left with "right" tag

  static Tag tree_tag() { return Tag(0); }
  static Tag left_ring_tag() { return Tag(2); }
  static Tag right_ring_tag() { return Tag(4); }

  bool post_tree_sends(int sender, TaskQueue &tq)
  {
    if (tree_sent[sender]) {
      return false;
    }

    bool sent = false;
    int lvl = int(std::log(sender + 1) / std::log(k));
    for (int i = 1; i <= k; i++) {
      int recv = sender + i * std::pow(k, lvl);
      if (recv < nodes) {
        tq.schedule(SendStartTask::make_new(tree_tag(), tq.now(), sender, recv));
        sent = true;
      }
    }

    tree_sent[sender] = true;
    return sent;
  }

  void post_ring_sends(int node, TaskQueue &tq)
  {
    if (!tree_recv[node]) {
      return;
    }

    if (!left_done[node] && !right_done[node]) {
      if (left_offs[node] <= right_offs[node]) {
        int offs = ++left_offs[node];
        int recv = (2 * nodes + node - offs) % nodes;
        tq.schedule(SendStartTask::make_new(left_ring_tag(), tq.now(), node, recv));
        if (right_min_recv[node] <= left_offs[node]) {
          left_done[node] = true;
        }
      } else {
        int offs = ++right_offs[node];
        int recv = (2 * nodes + node + offs) % nodes;
        tq.schedule(SendStartTask::make_new(right_ring_tag(), tq.now(), node, recv));
        if (left_min_recv[node] <= right_offs[node]) {
          right_done[node] = true;
        }
      }
    } else if (!left_done[node]) {
      int offs = ++left_offs[node];
      int recv = (2 * nodes + node - offs) % nodes;
      tq.schedule(SendStartTask::make_new(left_ring_tag(), tq.now(), node, recv));
      if (right_min_recv[node] <= left_offs[node]) {
        left_done[node] = true;
      }
    } else if (!right_done[node]) {
      int offs = ++right_offs[node];
      int recv = (2 * nodes + node + offs) % nodes;
      tq.schedule(SendStartTask::make_new(right_ring_tag(), tq.now(), node, recv));
      if (left_min_recv[node] <= right_offs[node]) {
        right_done[node] = true;
      }
    } else {
      assert(left_done[node] && right_done[node]);

      tq.schedule(FinishTask::make_new(tq.now(), node));
      all_done[node] = true;
    }
  }

  void do_tree_phase(int node, TaskQueue &tq)
  {
    // Got message from the parent, need to propagate tree
    // communication
    if (tree_recv[node]) {
      return;
    }

    tree_recv[node] = true;
    post_tree_sends(node, tq);

    {
      assert(left_offs[node] == 0);
      // Send left
      int offs = ++left_offs[node];
      int recv = (2 * nodes + node - offs) % nodes;
      tq.schedule(SendStartTask::make_new(left_ring_tag(), tq.now(), node, recv));
    }

    {
      assert(right_offs[node] == 0);
      // Send right
      int offs = ++right_offs[node];
      int recv = (2 * nodes + node + offs) % nodes;
      tq.schedule(SendStartTask::make_new(right_ring_tag(), tq.now(), node, recv));
    }

  }
public:
  CheckedCorrectedTreeBroadcast()
    : Collective(),
      k(Globals::get().conf().k),
      nodes(Globals::get().model().P),
      tree_recv(nodes),
      tree_sent(nodes),
      left_done(nodes),
      right_done(nodes),
      left_offs(nodes),
      right_offs(nodes),
      all_done(nodes),
      left_min_recv(nodes, nodes),
      right_min_recv(nodes, nodes)
  {}

  virtual void accept(const SendEndTask &task, TaskQueue &tq)
  {
    int node = task.sender();

    if (all_done[node])  {
      return;
    }

    if ((right_min_recv[node] <= left_offs[node]) && (left_offs[node] > 1)) {
      left_done[node] = true;
      tq.cancel_pending_sends(node, left_ring_tag());
    }

    if ((left_min_recv[node] <= right_offs[node]) && (right_offs[node] > 1)) {
      right_done[node] = true;
      tq.cancel_pending_sends(node, right_ring_tag());
    }

    if (tq.now_empty(node)) {
      post_ring_sends(node, tq);
    }

    if (left_done[node] && right_done[node]) {
      tq.schedule(FinishTask::make_new(tq.now(), node));
      all_done[node] = true;
    }
  }

  virtual void accept(const RecvEndTask& task, TaskQueue& tq)
  {
    int node = task.receiver();

    if (all_done[node])  {
      return;
    }

    if (task.tag() == tree_tag()) {
      do_tree_phase(node, tq);
    } else if (task.tag() == left_ring_tag()) {
      do_tree_phase(node, tq);
      int dist = std::min((nodes + node - task.sender()) % nodes,
                          (nodes + task.sender() - node) % nodes);
      left_min_recv[node] = std::min(left_min_recv[node], dist);
      if (tq.now_empty(node)) {
        post_ring_sends(node, tq);
      } else if (!right_done[node] && (left_min_recv[node] <= right_offs[node])) {
        right_done[node] = true;
        if (right_offs[node] > 1) {
          tq.cancel_pending_sends(node, right_ring_tag());
        }
      }
    } else if (task.tag() == right_ring_tag()) {
      do_tree_phase(node, tq);
      int dist = std::min((nodes + node - task.sender()) % nodes,
                          (nodes + task.sender() - node) % nodes);
      right_min_recv[node] = std::min(right_min_recv[node], dist);
      if (tq.now_empty(node)) {
        post_ring_sends(node, tq);
      } else if (!left_done[node] && (right_min_recv[node] <= left_offs[node])) {
        left_done[node] = true;
        if (left_offs[node] > 1) {
          tq.cancel_pending_sends(node, left_ring_tag());
        }
      }
    } else {
      assert(false);
    }

    if (left_done[node] && right_done[node]) {
      tq.schedule(FinishTask::make_new(tq.now(), node));
      tq.cancel_pending_sends(node, left_ring_tag());
      tq.cancel_pending_sends(node, right_ring_tag());
      all_done[node] = true;
    }
  }

  void populate(TaskQueue &tq) override
  {
    int root = 0;
    tree_recv[root] = true;
    post_tree_sends(root, tq);
    post_ring_sends(root, tq);
  }
};

