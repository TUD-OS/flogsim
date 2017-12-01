#pragma once

#include <tuple>

template<class COLL_T, bool send_over_root>
struct CTBNode
{
  constexpr static Tag tree_tag{0};
  constexpr static Tag left_ring_tag{2};
  constexpr static Tag right_ring_tag{4};

  struct TreePropagation
  {
    bool recv : 1;
    bool sent : 1;

    void dispatch_receive(COLL_T&, const Task&)
    {
      recv = true;
    }

    void dispatch_send(COLL_T&, const Task&)
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
          node.send(tq, tree_tag, receiver);
        }
      }

      sent = true;
    }
  };

  enum class Direction
  {
    LEFT = -1,
    RIGHT = 1,
  };

  struct RingPropagation
  {
    RingPropagation()
      : offs(0), min_recv(0), done(false)
    {}
    RingPropagation(const RingPropagation&) = delete;

    int offs;
    int min_recv;  // closest sender from where we are sending to
    bool done;

    Direction get_direction(const CTBNode &node) const
    {
      if (&node.left == this) {
        return Direction::LEFT;
      } else {
        return Direction::RIGHT;
      }
    }

    void check_done()
    {
      if (offs >= min_recv) {
        // We sent at least one message and we also sent to an alive
        // node on the right, meaning, we don't have to send to the
        // right anymore
        done = true;
      }
    }

    void dispatch_receive(CTBNode &node, COLL_T &coll, const Task &task)
    {
      int dist;
      if (get_direction(node) == Direction::LEFT) {
        dist = (coll.nodes + node.id - task.sender()) % coll.nodes;
      } else {
        dist = (coll.nodes + task.sender() - node.id) % coll.nodes;
      }
      min_recv = std::min(min_recv, dist);
      check_done();
    }

    void dispatch_send(CTBNode&, COLL_T&, const Task&)
    {
      // XXX: I think this line is not required but I'd linke to have tests first
      check_done();
    }

    void post_sends(CTBNode &node, COLL_T &coll, TaskQueue &tq)
    {
      // Don't send immediatelly, because we will get notification
      // later, when other sends finish

      Direction direction = get_direction(node);

      int cur_offs = (++offs) * static_cast<int>(direction);

      int recv = (2 * coll.nodes + node.id + cur_offs) % coll.nodes;

      if (!send_over_root) {
        if ((direction == Direction::LEFT && recv == coll.nodes - 1) ||
            (direction == Direction::RIGHT && recv == 0)) {
          done = true;
          return;
        }
      }

      if (direction == Direction::LEFT) {
        node.send(tq, left_ring_tag, recv);
      } else {
        node.send(tq, right_ring_tag, recv);
      }
    }
  };

  TreePropagation tree;
  RingPropagation left;
  RingPropagation right;

  bool all_done : 1;
  bool first_ring : 1;

  int id;

  int pending_sends;

private:

  void send(TaskQueue &tq, Tag tag, int receiver)
  {
    pending_sends++;
    tq.schedule(SendStartTask::make_new(tag, tq.now(), id, receiver));
  }

  void post_first_ring_messages(COLL_T &coll, TaskQueue &tq)
  {
    first_ring = true;
    left.post_sends(*this, coll, tq);
    right.post_sends(*this, coll, tq);
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

    if (!tree.recv && !(left.min_recv < coll.nodes) && !(right.min_recv < coll.nodes)) {
      return;
    }

    // We always sent at least one round of ring messages
    // XXX: This look also unnecessary
    if (tree.recv && !first_ring) {
      post_first_ring_messages(coll, tq);
      return;
    }

    // Check if we done, and need to state that we are done
    if (left.done && right.done) {
      post_finish_message(tq);
    }

    // We don't do full checked correction, if we didn't receive
    // tree message
    if (!tree.recv) {
      post_finish_message(tq);
      return;
    }

    // Here we do correction
    if (!left.done && !right.done) {
      // If both are not done, choose the one, where we advanced less
      if (left.offs <= right.offs) {
        left.post_sends(*this, coll, tq);
      } else {
        right.post_sends(*this, coll, tq);
      }
    } else if (!left.done) {
      left.post_sends(*this, coll, tq);
    } else if (!right.done) {
      right.post_sends(*this, coll, tq);
    }

  }

  void accept_receive(COLL_T &coll, const Task &task)
  {
    switch (task.tag().get()) {
      case tree_tag.get():
        tree.dispatch_receive(coll, task);
        break;
      case left_ring_tag.get():
        right.dispatch_receive(*this, coll, task);
        break;
      case right_ring_tag.get():
        left.dispatch_receive(*this, coll, task);
        break;
      default:
        assert(0 && "Unknown tag");
    }
  }

  void accept_send_end(COLL_T &coll, const Task &task)
  {
    pending_sends--;

    switch (task.tag().get()) {
      case tree_tag.get():
        tree.dispatch_send(coll, task);
        break;
      case left_ring_tag.get():
        left.dispatch_send(*this, coll, task);
        break;
      case right_ring_tag.get():
        right.dispatch_send(*this, coll, task);
        break;
      default:
        assert(0 && "Unknown tag");
    }
  }
};
