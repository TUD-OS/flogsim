#include <cmath>

#include "collective.hpp"
#include "task_queue.hpp"

class CheckedCorrectedTreeBroadcast : public Collective
{
  int k;
  int nodes;

  struct Ring
  {
    std::vector<bool> done;
    std::vector<int> offs;
    int inc;

    Ring(int nodes, int inc):
      done(nodes), offs(nodes), inc(inc)
    {}

    void complete(int sender, TaskQueue &tq)
    {
      done[sender] = true;
      tq.cancel_pending_sends(sender, Tag(3 + inc));
    }
  };

  Ring left, right;
  std::vector<bool> tree_done;

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

    if (left.done[sender] && right.done[sender]) {
      tq.schedule(FinishTask::make_new(tq.now(), sender));
    }
  }

  bool check_done(int sender, TaskQueue &tq)
  {
    int left_shift = left.offs[sender];
    int right_shift = right.offs[sender];

    if (left_shift == 0 || right_shift == 0) {
      // Need to send at least once
      return false;
    }

    if ((left_shift + nodes) % nodes == (right_shift + nodes) % nodes) {
      left.complete(sender, tq);
      right.complete(sender, tq);

      return true;
    }

    return left.done[sender] && right.done[sender];
  }

  void post_ring_sends(int sender, TaskQueue &tq)
  {
    if (!left.done[sender]) {
      int left_shift = left.offs[sender] + left.inc;

      if (check_done(sender, tq)) {
        return;
      }

      left.offs[sender] = left_shift;

      // Send correction messages to the left
      int recv = (sender + left_shift + nodes) % nodes;
      tq.schedule(SendStartTask::make_new(left_ring_tag(), tq.now(), sender, recv));
    }

    if (!right.done[sender]) {
      // Send correction messages to the right
      int right_shift = right.offs[sender] + right.inc;

      if (check_done(sender, tq)) {
        return;
      }

      right.offs[sender] = right_shift;

      int recv = (sender + right_shift + nodes) % nodes;
      tq.schedule(SendStartTask::make_new(right_ring_tag(), tq.now(), sender, recv));
    }

    if (left.done[sender] && right.done[sender]) {
      tq.schedule(FinishTask::make_new(tq.now(), sender));
    }
  }

public:
  CheckedCorrectedTreeBroadcast()
    : k(Configuration::get().k),
      nodes(Configuration::get().P),
      tree_done(nodes),
      left(nodes, -1),
      right(nodes, +1)
  {}

  virtual void accept(const SendGapEndTask& task, TaskQueue& tq)
  {
    post_ring_sends(task.sender(), tq);
  }

  virtual void accept(const RecvEndTask& task, TaskQueue& tq)
  {
    int me = task.receiver();

    if (task.tag() == tree_tag()) {
      // Got message from the parent, need to propagate tree
      // communication
      post_tree_sends(me, tq);
    } else if (task.tag() == right_ring_tag()) {
      // Received correction from the left
      if (left.offs[me] == 0) {
        // Need to send a reply at least once
        tq.schedule(SendStartTask::make_new(left_ring_tag(),
                                            tq.now(), me, task.sender()));
      }
      if (left.offs[me] > 0) {
        left.complete(me, tq);
      }
      if (left.done[me] && right.done[me]) {
        tq.schedule(FinishTask::make_new(tq.now(), me));
      }
    } else if (task.tag() == left_ring_tag()) {
      // Received correction from the right
      if (right.offs[me] == 0) {
        // Need to send a reply at least once
        right.offs[me] ++;
        tq.schedule(SendStartTask::make_new(right_ring_tag(),
                                            tq.now(), me, task.sender()));
      }
      if (right.done[me] > 0) {
        right.complete(me, tq);
      }
      if (left.done[me] && right.done[me]) {
        tq.schedule(FinishTask::make_new(tq.now(), me));
      }
    } else {
      assert(false);
    }
  }

  void populate(TaskQueue &tq) override
  {
    int root = 0;
    post_tree_sends(root, tq);
    post_ring_sends(root, tq);
  }
};

static CollectiveRegistrator<CheckedCorrectedTreeBroadcast> reg("checked_correctedtree_bcast");
