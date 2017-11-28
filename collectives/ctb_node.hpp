#pragma once

static Tag tree_tag() { return Tag(0); }
static Tag left_ring_tag() { return Tag(2); }
static Tag right_ring_tag() { return Tag(4); }

template<class COLL_T, bool send_over_root>
struct CTBNode
{
  bool all_done : 1;

  struct TreePropagation
  {
    bool recv : 1;
    bool sent : 1;

    void dispatch_receive(COLL_T &coll, const Task &task)
    {
      recv = true;
    }

    void dispatch_send(COLL_T &coll, const Task &task)
    {
      // recv = true;
    }

    void post_sends(CTBNode &node, COLL_T &coll, TaskQueue &tq)
    {
      // Should not send if we did not received a message on the tree
      if (!recv || sent) {
        return;
      }
      assert(sent == false);

      int lvl = int(std::log(node.id + 1) / std::log(coll.k));
      for (int i = 1; i <= coll.k; i++) {
        int receiver = node.id + i * std::pow(coll.k, lvl);
        if (receiver < coll.nodes) {
          node.send(tq, tree_tag(), receiver);
        }
      }

      sent = true;
    }
  };

  TreePropagation tree;

  bool left_done : 1;

  bool right_done : 1;
  bool first_ring : 1;

  int left_offs, right_offs;
  int right_min_recv; // closest sender from right with "left" tag
  int left_min_recv; // closest sender from left with "right" tag
  int id;

  int pending_sends;

private:

  void send(TaskQueue &tq, Tag tag, int receiver)
  {
    pending_sends++;
    tq.schedule(SendStartTask::make_new(tag, tq.now(), id, receiver));
  }

  void post_left_ring_messages(COLL_T &coll, TaskQueue &tq, bool force = false)
  {
    // Don't send immediatelly, because we will get notification
    // later, when other sends finish

    int offs = ++left_offs;
    int recv = (2 * coll.nodes + id - offs) % coll.nodes;

    if (!send_over_root && recv == coll.nodes - 1) {
      left_done = true;
      return;
    }

    send(tq, left_ring_tag(), recv);
  }

  void post_right_ring_messages(COLL_T &coll, TaskQueue &tq, bool force = false)
  {
    // Don't send immediatelly, because we will get notification
    // later, when other sends finish

    int offs = ++right_offs;
    int recv = (2 * coll.nodes + id + offs) % coll.nodes;

    if (!send_over_root && recv == 0) {
      right_done = true;
      return;
    }

    send(tq, right_ring_tag(), recv);
  }

  void post_first_ring_messages(COLL_T &coll, TaskQueue &tq)
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

public:
  void post_next_message(COLL_T &coll, TaskQueue &tq)
  {
    // Check if we are absolutely done
    if (all_done) {
      return;
    }

    // First message we ever post is always tree message
    tree.post_sends(*this, coll, tq);

    if (!(tq.now() >= coll.correction_phase_start(coll.k))) {
      return;
    }

    // We always sent at least one round of ring messages
    if (tree.recv && !first_ring) {
      post_first_ring_messages(coll, tq);
      return;
    }

    // Check if we done, and need to state that we are done
    if (left_done && right_done) {
      post_finish_message(tq);
    }

    // We don't do full checked correction, if we didn't receive
    // tree message
    if (!tree.recv) {
      post_finish_message(tq);
      return;
    }

    // Here we do correction
    if (!left_done && !right_done) {
      // If both are not done, choose the one, where we advanced less
      if (left_offs <= right_offs) {
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

  void accept_receive(COLL_T &coll, const Task &task)
  {
    if (task.tag() == tree_tag()) {
      tree.dispatch_receive(coll, task);
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

  void accept_send_end(COLL_T &coll, const Task &task)
  {
    pending_sends--;

    if (task.tag() == tree_tag()) {
      tree.dispatch_send(coll, task);
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
