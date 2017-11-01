#include <algorithm>

#include <cmath>

#include "collective.hpp"
#include "task_queue.hpp"

class CheckedCorrectedTreeBroadcast : public Collective
{
  int k;
  int nodes;

  std::vector<bool> tree_done;
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
    if (tree_done[sender]) {
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

    tree_done[sender] = true;
    return sent;
  }

  void post_ring_sends(int node, TaskQueue &tq)
  {
    if (!tree_done[node]) {
      return;
    }

    if (!left_done[node] && !right_done[node]) {
      if (left_offs[node] <= right_offs[node]) {
        int offs = ++left_offs[node];
        int recv = (2 * nodes + node - offs) % nodes;
        tq.schedule(SendStartTask::make_new(left_ring_tag(), tq.now(), node, recv));
      } else {
        int offs = ++right_offs[node];
        int recv = (2 * nodes + node + offs) % nodes;
        tq.schedule(SendStartTask::make_new(right_ring_tag(), tq.now(), node, recv));
      }
    } else if (!left_done[node] || left_offs[node] == 0) {
      // Send left
      int offs = ++left_offs[node];
      int recv = (2 * nodes + node - offs) % nodes;
      tq.schedule(SendStartTask::make_new(left_ring_tag(), tq.now(), node, recv));
    } else if (!right_done[node] || right_offs[node] == 0) {
      int offs = ++right_offs[node];
      int recv = (2 * nodes + node + offs) % nodes;
      tq.schedule(SendStartTask::make_new(right_ring_tag(), tq.now(), node, recv));
    }
  }

  void do_tree_phase(int node, TaskQueue &tq)
  {
    // Got message from the parent, need to propagate tree
    // communication
    if (!post_tree_sends(node, tq))
      post_ring_sends(node, tq);
  }
public:
  CheckedCorrectedTreeBroadcast()
    : Collective(),
      k(Globals::get().conf().k),
      nodes(Globals::get().model().P),
      tree_done(nodes),
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
    if ((task.tag() == left_ring_tag()) ||
        (task.tag() == right_ring_tag())) {
      post_ring_sends(task.sender(), tq);
    }
  }

  virtual void accept(const RecvEndTask& task, TaskQueue& tq)
  {
    int me = task.receiver();

    if (task.tag() == tree_tag()) {
      do_tree_phase(me, tq);
    } else if (task.tag() == left_ring_tag()) {
      int dist = std::min((nodes + me - task.sender()) % nodes,
                          (nodes + task.sender() - me) % nodes);
      left_min_recv[me] = std::min(left_min_recv[me], dist);
      if (left_min_recv[me] <= right_offs[me]) {
        right_done[me] = true;
        tq.cancel_pending_sends(me, right_ring_tag());
      }
      post_ring_sends(me, tq);
    } else if (task.tag() == right_ring_tag()) {
      int dist = std::min((nodes + me - task.sender()) % nodes,
                          (nodes + task.sender() - me) % nodes);
      right_min_recv[me] = std::min(right_min_recv[me], dist);
      if (right_min_recv[me] <= left_offs[me]) {
        left_done[me] = true;
        tq.cancel_pending_sends(me, left_ring_tag());
      }
      post_ring_sends(me, tq);
    } else {
      assert(false);
    }

    if (left_done[me] && right_done[me] && !all_done[me]) {
      tq.schedule(FinishTask::make_new(tq.now(), me));
      all_done[me] = true;
    }
  }

  void populate(TaskQueue &tq) override
  {
    int root = 0;
    post_tree_sends(root, tq);
    post_ring_sends(root, tq);
  }
};

