#include <algorithm>

#include <cmath>

#include "collective.hpp"
#include "task_queue.hpp"

class CheckedCorrectedTreeBroadcast : public Collective
{
  int k;
  int nodes;

  struct Node
  {
    int all_done : 1;
    int tree_recv : 1;
    int tree_sent : 1;

    int left_done : 1;

    int right_done : 1;

    int left_offs, right_offs;
    int right_min_recv; // closest sender from right with "left" tag
    int left_min_recv; // closest sender from left with "right" tag
    int id;
  };

  std::vector<Node> nodeset;

  static Tag tree_tag() { return Tag(0); }
  static Tag left_ring_tag() { return Tag(2); }
  static Tag right_ring_tag() { return Tag(4); }

  bool post_tree_sends(Node &node, TaskQueue &tq)
  {
    if (node.tree_sent) {
      return false;
    }

    bool sent = false;
    int lvl = int(std::log(node.id + 1) / std::log(k));
    for (int i = 1; i <= k; i++) {
      int recv = node.id + i * std::pow(k, lvl);
      if (recv < nodes) {
        tq.schedule(SendStartTask::make_new(tree_tag(), tq.now(), node.id, recv));
        sent = true;
      }
    }

    node.tree_sent = true;
    return sent;
  }

  void post_ring_sends(Node &node, TaskQueue &tq)
  {
    if (!node.tree_recv) {
      return;
    }

    if (!node.left_done && !node.right_done) {
      if (node.left_offs <= node.right_offs) {
        int offs = ++node.left_offs;
        int recv = (2 * nodes + node.id - offs) % nodes;
        tq.schedule(SendStartTask::make_new(left_ring_tag(), tq.now(), node.id, recv));
        if (node.right_min_recv <= node.left_offs) {
          node.left_done = true;
        }
      } else {
        int offs = ++node.right_offs;
        int recv = (2 * nodes + node.id + offs) % nodes;
        tq.schedule(SendStartTask::make_new(right_ring_tag(), tq.now(), node.id, recv));
        if (node.left_min_recv <= node.right_offs) {
          node.right_done = true;
        }
      }
    } else if (!node.left_done) {
      int offs = ++node.left_offs;
      int recv = (2 * nodes + node.id - offs) % nodes;
      tq.schedule(SendStartTask::make_new(left_ring_tag(), tq.now(), node.id, recv));
      if (node.right_min_recv <= node.left_offs) {
        node.left_done = true;
      }
    } else if (!node.right_done) {
      int offs = ++node.right_offs;
      int recv = (2 * nodes + node.id + offs) % nodes;
      tq.schedule(SendStartTask::make_new(right_ring_tag(), tq.now(), node.id, recv));
      if (node.left_min_recv <= node.right_offs) {
        node.right_done = true;
      }
    } else {
      assert(node.left_done && node.right_done);

      tq.schedule(FinishTask::make_new(tq.now(), node.id));
      node.all_done = true;
    }
  }

  void do_tree_phase(Node &node, TaskQueue &tq)
  {
    // Got message from the parent, need to propagate tree
    // communication
    if (node.tree_recv) {
      return;
    }

    node.tree_recv = true;
    post_tree_sends(node, tq);

    {
      assert(node.left_offs == 0);
      // Send left
      int offs = ++node.left_offs;
      int recv = (2 * nodes + node.id - offs) % nodes;
      tq.schedule(SendStartTask::make_new(left_ring_tag(), tq.now(), node.id, recv));
    }

    {
      assert(node.right_offs == 0);
      // Send right
      int offs = ++node.right_offs;
      int recv = (2 * nodes + node.id + offs) % nodes;
      tq.schedule(SendStartTask::make_new(right_ring_tag(), tq.now(), node.id, recv));
    }

  }
public:
  CheckedCorrectedTreeBroadcast()
    : Collective(),
      k(Globals::get().conf().k),
      nodes(Globals::get().model().P),
      nodeset(nodes)
  {
    for (int i = 0; i < nodes; i++) {
      nodeset[i].id = i;
    }
  }

  virtual void accept(const SendEndTask &task, TaskQueue &tq)
  {
    Node &node = nodeset[task.sender()];

    if (node.all_done)  {
      return;
    }

    if ((node.right_min_recv <= node.left_offs) && (node.left_offs > 1)) {
      node.left_done = true;
      tq.cancel_pending_sends(node.id, left_ring_tag());
    }

    if ((node.left_min_recv <= node.right_offs) && (node.right_offs > 1)) {
      node.right_done = true;
      tq.cancel_pending_sends(node.id, right_ring_tag());
    }

    if (tq.now_empty(node.id)) {
      post_ring_sends(node, tq);
    }

    if (node.left_done && node.right_done) {
      tq.schedule(FinishTask::make_new(tq.now(), node.id));
      node.all_done = true;
    }
  }

  virtual void accept(const RecvEndTask& task, TaskQueue& tq)
  {
    Node &node = nodeset[task.receiver()];

    if (node.all_done)  {
      return;
    }

    if (task.tag() == tree_tag()) {
      do_tree_phase(node, tq);
    } else if (task.tag() == left_ring_tag()) {
      do_tree_phase(node, tq);
      int dist = std::min((nodes + node.id - task.sender()) % nodes,
                          (nodes + task.sender() - node.id) % nodes);
      node.left_min_recv = std::min(node.left_min_recv, dist);
      if (tq.now_empty(node.id)) {
        post_ring_sends(node, tq);
      } else if (!node.right_done && (node.left_min_recv <= node.right_offs)) {
        node.right_done = true;
        if (node.right_offs > 1) {
          tq.cancel_pending_sends(node.id, right_ring_tag());
        }
      }
    } else if (task.tag() == right_ring_tag()) {
      do_tree_phase(node, tq);
      int dist = std::min((nodes + node.id - task.sender()) % nodes,
                          (nodes + task.sender() - node.id) % nodes);
      node.right_min_recv = std::min(node.right_min_recv, dist);
      if (tq.now_empty(node.id)) {
        post_ring_sends(node, tq);
      } else if (!node.left_done && (node.right_min_recv <= node.left_offs)) {
        node.left_done = true;
        if (node.left_offs > 1) {
          tq.cancel_pending_sends(node.id, left_ring_tag());
        }
      }
    } else {
      assert(false);
    }

    if (node.left_done && node.right_done) {
      tq.schedule(FinishTask::make_new(tq.now(), node.id));
      tq.cancel_pending_sends(node.id, left_ring_tag());
      tq.cancel_pending_sends(node.id, right_ring_tag());
      node.all_done = true;
    }
  }

  void populate(TaskQueue &tq) override
  {
    Node &root = nodeset[0];
    root.tree_recv = true;
    post_tree_sends(root, tq);
    post_ring_sends(root, tq);
  }
};

