#include <cmath>

#include "collective.hpp"
#include "task_queue.hpp"

class BinaryBroadcast : public Collective
{
  int nodes;
  void post_sends(int sender, TaskQueue &tq)
  {
    for (int i = 1; i <= 2; i++) {
      int recv = 2 * sender + i;
      if (recv < nodes) {
        tq.schedule(SendStartTask::make_new(Tag(0), tq.now(), sender, recv));
      }
    }
    tq.schedule(FinishTask::make_new(sender));
  }
public:
  BinaryBroadcast()
    : nodes(Configuration::get().P)
  {
  }

  virtual void accept(const RecvStartTask& task, TaskQueue& tq)
  {
    post_sends(task.receiver(), tq);
  }

  void populate(TaskQueue &tq) override
  {
    int root = 0;
    post_sends(root, tq);
  }
};

class CorrectedTreeBroadcast : public Collective
{
  int k;
  int nodes;
  std::vector<bool> done;

  void post_sends(int sender, TaskQueue &tq)
  {
    if (done[sender]) {
      return;
    }

    int lvl = int(std::log(sender + 1) / std::log(k));
    for (int i = 1; i <= k; i++) {
      int recv = sender + i * std::pow(k, lvl);
      if (recv < nodes) {
        tq.schedule(SendStartTask::make_new(Tag(0), tq.now(), sender, recv));
      }
    }

    for (int i = 1; i <= k - 1; i++) {
      int recv = (sender + nodes - i) % nodes;
      tq.schedule(SendStartTask::make_new(Tag(0), tq.now(), sender, recv));
    }

    tq.schedule(FinishTask::make_new(sender));
    done[sender] = true;
  }

public:
  CorrectedTreeBroadcast()
    : k(Configuration::get().k),
      nodes(Configuration::get().P),
      done(nodes)
  {}

  virtual void accept(const RecvStartTask& task, TaskQueue& tq)
  {
    post_sends(task.receiver(), tq);
  }

  void populate(TaskQueue &tq) override
  {
    int root = 0;
    post_sends(root, tq);
  }
};

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
      tq.schedule(FinishTask::make_new(sender));
    }
  }

  bool check_done(int sender)
  {
    int left_shift = left.offs[sender];
    int right_shift = right.offs[sender];

    if (left_shift == 0 || right_shift == 0) {
      // Need to send at least once
      return false;
    }

    if ((left_shift + nodes) % nodes == (right_shift + nodes) % nodes) {
      left.done[sender] = true;
      right.done[sender] = true;

      return true;
    }

    return left.done[sender] && right.done[sender];
  }

  void post_ring_sends(int sender, TaskQueue &tq)
  {
    if (!left.done[sender]) {
      int left_shift = left.offs[sender] + left.inc;

      if (check_done(sender)) {
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

      if (check_done(sender)) {
        return;
      }

      right.offs[sender] = right_shift;

      int recv = (sender + right_shift + nodes) % nodes;
      tq.schedule(SendStartTask::make_new(right_ring_tag(), tq.now(), sender, recv));
    }

    if (left.done[sender] && right.done[sender]) {
      tq.schedule(FinishTask::make_new(sender));
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
        left.done[me] = true;
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
        right.done[me] = true;
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

std::unique_ptr<Collective> Collective::create()
{
  auto &conf = Configuration::get();

  if (conf.collective == "binary_bcast") {
    return std::make_unique<BinaryBroadcast>();
  } else if (conf.collective == "correctedtree_bcast") {
    return std::make_unique<CorrectedTreeBroadcast>();
  } else if (conf.collective == "checkedcorrectedtree_bcast") {
    return std::make_unique<CheckedCorrectedTreeBroadcast>();
  } else {
    throw std::invalid_argument("Collective does not exist:" +
                                conf.collective);
  }
}
