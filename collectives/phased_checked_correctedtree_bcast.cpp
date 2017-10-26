#include <algorithm>

#include <cmath>

#include "collective.hpp"
#include "task_queue.hpp"

class PhasedCheckedCorrectedTreeBroadcast : public Collective
{
  int k;
  int nodes;

  std::vector<bool> tree_done;
  std::vector<bool> left_done;
  std::vector<bool> right_done;
  std::vector<int> left_offs;
  std::vector<int> right_offs;
  std::vector<bool> all_done;

  static Tag tree_tag() { return Tag(0); }
  static Tag left_ring_tag() { return Tag(2); }
  static Tag right_ring_tag() { return Tag(4); }

  void post_tree_sends(int sender, TaskQueue &tq)
  {
    if (tree_done[sender]) {
      return;
    }

    int lvl = int(std::log(sender + 1) / std::log(k));
    for (int i = 1; i <= k; i++) {
      int recv = sender + i * std::pow(k, lvl);
      if (recv < nodes) {
        tq.schedule(SendStartTask::make_new(tree_tag(), tq.now(), sender, recv));
      }
    }

    tree_done[sender] = true;
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
    } else if (!left_done[node]) {
      // Send left
      int offs = ++left_offs[node];
      int recv = (2 * nodes + node - offs) % nodes;
      tq.schedule(SendStartTask::make_new(left_ring_tag(), tq.now(), node, recv));
    } else if (!right_done[node]) {
      int offs = ++right_offs[node];
      int recv = (2 * nodes + node + offs) % nodes;
      tq.schedule(SendStartTask::make_new(right_ring_tag(), tq.now(), node, recv));
    }
  }

  Time tree_phase_end()
  {
    return Time(14);
  }

  void do_tree_phase(int node, TaskQueue &tq)
  {
    // Got message from the parent, need to propagate tree
    // communication
    post_tree_sends(node, tq);
    // Check if tree phase ended
    if (tq.now() >= tree_phase_end()) {
      for (int i = 0; i < nodes; i ++) {
        // start ring phase
        post_ring_sends(i, tq);
      }
    }
  }
public:
  PhasedCheckedCorrectedTreeBroadcast()
    : k(Configuration::get().k),
      nodes(Configuration::get().P),
      tree_done(nodes),
      left_done(nodes),
      right_done(nodes),
      left_offs(nodes),
      right_offs(nodes),
      all_done(nodes)
  {}

  virtual void accept(const SendGapEndTask &task, TaskQueue &tq)
  {
    if (task.tag() == left_ring_tag()) {
      post_ring_sends(task.sender(), tq);
    } else if (task.tag() == right_ring_tag()) {
      post_ring_sends(task.sender(), tq);
    }
  }

  virtual void accept(const RecvEndTask& task, TaskQueue& tq)
  {
    int me = task.receiver();

    if (task.tag() == tree_tag()) {
      do_tree_phase(me, tq);
    } else if (task.tag() == left_ring_tag()) {
      right_done[me] = true;
      tq.cancel_pending_sends(me, right_ring_tag());
    } else if (task.tag() == right_ring_tag()) {
      left_done[me] = true;
      tq.cancel_pending_sends(me, left_ring_tag());
    } else {
      assert(false);
    }

    if (left_done[me] && right_done[me] && !all_done[me]) {
      tq.schedule(FinishTask::make_new(me));
      all_done[me] = true;
    }
  }

  void populate(TaskQueue &tq) override
  {
    int root = 0;
    post_tree_sends(root, tq);
  }
};

static CollectiveRegistrator<PhasedCheckedCorrectedTreeBroadcast> reg("phased_checked_correctedtree_bcast");
