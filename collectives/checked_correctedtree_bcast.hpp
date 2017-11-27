#include <algorithm>

#include <cmath>

#include "collective.hpp"
#include "task_queue.hpp"

class CheckedCorrectedTreeBroadcast : public Collective
{
  int k;
  int nodes;

  static Tag tree_tag() { return Tag(0); }
  static Tag left_ring_tag() { return Tag(2); }
  static Tag right_ring_tag() { return Tag(4); }

  typedef CheckedCorrectedTreeBroadcast coll_t;

  struct Node
  {
    bool all_done : 1;
    bool tree_recv : 1;
    bool tree_sent : 1;

    bool left_done : 1;

    bool right_done : 1;
    bool first_ring : 1;

    int left_offs, right_offs;
    int right_min_recv; // closest sender from right with "left" tag
    int left_min_recv; // closest sender from left with "right" tag
    int id;

    int pending_sends;

  private:
    void post_tree_sends(coll_t &coll, TaskQueue &tq)
    {
      assert(tree_sent == false);

      bool sent = false;
      int lvl = int(std::log(id + 1) / std::log(coll.k));
      for (int i = 1; i <= coll.k; i++) {
        int recv = id + i * std::pow(coll.k, lvl);
        if (recv < coll.nodes) {
          pending_sends++;
          tq.schedule(SendStartTask::make_new(tree_tag(), tq.now(), id, recv));
          sent = true;
        }
      }

      tree_sent = true;
    }

    void post_left_ring_messages(coll_t &coll, TaskQueue &tq, bool force = false)
    {
      // Don't send immediatelly, because we will get notification
      // later, when other sends finish

      int offs = ++left_offs;
      int recv = (2 * coll.nodes + id - offs) % coll.nodes;

      if (recv == coll.nodes - 1) {
        left_done = true;
        return;
      }

      pending_sends++;
      tq.schedule(SendStartTask::make_new(left_ring_tag(), tq.now(), id, recv));
    }

    void post_right_ring_messages(coll_t &coll, TaskQueue &tq, bool force = false)
    {
      // Don't send immediatelly, because we will get notification
      // later, when other sends finish

      int offs = ++right_offs;
      int recv = (2 * coll.nodes + id + offs) % coll.nodes;

      if (recv == 0) {
        right_done = true;
        return;
      }

      pending_sends++;
      tq.schedule(SendStartTask::make_new(right_ring_tag(), tq.now(), id, recv));
    }

    void post_first_ring_messages(coll_t &coll, TaskQueue &tq)
    {
      first_ring = true;
      post_left_ring_messages(coll, tq, true);
      post_right_ring_messages(coll, tq, true);
    }

    void post_finish_message(TaskQueue &tq)
    {
      all_done = true;
      tq.schedule(FinishTask::make_new(id));
    }

    void post_timer_task(TaskQueue &tq)
    {
      tq.schedule(TimerTask::make_new(tq.now(), id));
    }
  public:
    void post_next_message(coll_t &coll, TaskQueue &tq)
    {
      // Check if we are absolutely done
      if (all_done) {
        return;
      }

      // First message we ever post is always tree message
      if (tree_recv && !tree_sent) {
        post_tree_sends(coll, tq);
      }

      // We always sent at least one round of ring messages
      if (tree_recv && !first_ring) {
        post_first_ring_messages(coll, tq);
        return;
      }

      // Check if we done, and need to state that we are done
      if (tree_sent && left_done && right_done) {
        post_finish_message(tq);
      }

      // We don't do full checked correction, if we didn't receive
      // tree message
      if (!tree_recv) {
        return;
      }

      // Here we do correction
      if (!left_done && !right_done) {
        // If both are not done, choose the one, where we advanced less
        if (left_offs < right_offs) {
          post_left_ring_messages(coll, tq);
        } else {
          post_right_ring_messages(coll, tq);
        }
      } else if (!left_done) {
        post_left_ring_messages(coll, tq);
      } else if (!right_done) {
        post_right_ring_messages(coll, tq);
      }

    }

    void accept_receive(coll_t &coll, const Task &task)
    {
      if (task.tag() == tree_tag()) {
        tree_recv = true;
      } else if (task.tag() == left_ring_tag()) {
        int dist = std::min((coll.nodes + id - task.sender()) % coll.nodes,
                            (coll.nodes + task.sender() - id) % coll.nodes);
        left_min_recv = std::min(left_min_recv, dist);
        if (right_offs >= left_min_recv) {
          // We sent at least one message and we also sent to an alive
          // node on the right, meaning, we don't have to send to the
          // right anymore
          right_done = true;
        }
      } else if (task.tag() == right_ring_tag()) {
        int dist = std::min((coll.nodes + id - task.sender()) % coll.nodes,
                            (coll.nodes + task.sender() - id) % coll.nodes);
        right_min_recv = std::min(right_min_recv, dist);
        if (left_offs >= right_min_recv) {
          // See comment above, but the same about the left
          left_done = true;
        }
      }
    }

    void accept_send_end(coll_t &coll, const Task &task)
    {
      if (task.tag() == tree_tag()) {
        tree_recv = true;
      } else if (task.tag() == left_ring_tag()) {
        if (left_offs >= right_min_recv) {
          // See comment below about the right ring
          left_done = true;
        }
      } else if (task.tag() == right_ring_tag()) {
        if (right_offs >= left_min_recv) {
          // We sent at least one message and we also sent to an alive
          // node on the right, meaning, we don't have to send to the
          // right anymore
          right_done = true;
        }
      }
    }
  };

  std::vector<Node> nodeset;

public:
  CheckedCorrectedTreeBroadcast()
    : Collective(),
      k(Globals::get().conf().k),
      nodes(Globals::get().model().P),
      nodeset(nodes)
  {
    for (int i = 0; i < nodes; i++) {
      nodeset[i].id = i;
      nodeset[i].right_min_recv = nodes;
      nodeset[i].left_min_recv = nodes;
    }
  }

  virtual void accept(const IdleTask &task, TaskQueue &tq)
  {
    Node &node = nodeset[task.sender()];

    if (!node.pending_sends) {
      node.post_next_message(*this, tq);
    }
  }

  virtual void accept(const SendEndTask &task, TaskQueue &tq)
  {
    Node &node = nodeset[task.sender()];

    node.pending_sends--;
    node.accept_send_end(*this, task);
    tq.schedule(IdleTask::make_new(node.id));
  }

  virtual void accept(const RecvEndTask& task, TaskQueue& tq)
  {
    Node &node = nodeset[task.receiver()];

    node.accept_receive(*this, task);
    tq.schedule(IdleTask::make_new(node.id));
  }

  virtual void accept(const InitTask &task, TaskQueue &tq)
  {
    Node &root = nodeset[0];
    root.tree_recv = true;
    root.post_next_message(*this, tq);
  }
};

