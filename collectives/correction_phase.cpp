#include <assert.h>
#include <algorithm>
#include <cmath>

#include "correction_phase.hpp"
#include "task_queue.hpp"


template<bool send_over_root>
CorrectionPhase<send_over_root>::CorrectionPhase(ReachedNodes &reached_nodes)
  : Phase(reached_nodes)
{
  assert(std::any_of(reached_nodes.begin(),
                     reached_nodes.end(),
                     [] (bool reached) {return reached;})
         && "No reached node");
}


template<bool send_over_root>
OpportunisticCorrectionPhase<send_over_root>::OpportunisticCorrectionPhase(
  ReachedNodes &reached_nodes)
  : CorrectionPhase<send_over_root>(reached_nodes),
    max_dist(Globals::get().conf().k)
{
  assert(max_dist < this->num_nodes() && "Nonsensical correction distance");
}

template<bool send_over_root>
Phase::Result
OpportunisticCorrectionPhase<send_over_root>::dispatch(
  const InitTask &, TaskQueue &tq, int node_id)
{
  assert(this->reached_nodes[node_id] && "Init on unreached node");

  // all reached nodes send out correction messages
  for (int offset = 1; offset <= max_dist; ++offset) {
    int receiver = node_id - offset;

    if (send_over_root) {
      receiver = (receiver + this->num_nodes()) % this->num_nodes();
    }

    if (receiver >= 0) {
      tq.schedule(SendStartTask::make_new(Tag::RING_LEFT, tq.now(), node_id, receiver));
    }
  }

  return Phase::Result::DONE_PHASE;
}

template<bool send_over_root>
Phase::Result
OpportunisticCorrectionPhase<send_over_root>::dispatch(
  const RecvEndTask &, TaskQueue &, int node_id)
{
  this->reached_nodes[node_id] = true;
  return Phase::Result::ONGOING;
}

template<bool send_over_root>
Time
OpportunisticCorrectionPhase<send_over_root>::deadline() const
{
  auto &model = Globals::get().model();
  auto o = model.o;
  auto g = model.g;

  return o + (max_dist - 1) * std::max(o,g);
}

// explicit instantiation
template class OpportunisticCorrectionPhase<true>;
template class OpportunisticCorrectionPhase<false>;

// CheckedCorrectionPhase

template<bool send_over_root>
CheckedCorrectionPhase<send_over_root>::CheckedCorrectionPhase(
  ReachedNodes &reached_nodes)
  : CorrectionPhase<send_over_root>(reached_nodes),
    left{unsigned(this->num_nodes())},
    right{unsigned(left.size())}
{
}

template<bool send_over_root>
Phase::Result
CheckedCorrectionPhase<send_over_root>::post_message(
  TaskQueue &tq, int node_id)
{
  auto DIR_LEFT = this->DIR_LEFT;
  auto DIR_RIGHT = this->DIR_RIGHT;

  int P = this->num_nodes();

  Ring &left = this->left[node_id];
  Ring &right = this->right[node_id];

  // Check if we sent past the closest sender-to-us from both
  // directions, i. e. check if we are done
  if ((left.offset >= left.min_recv) &&
      (right.offset >= right.min_recv)) {
    return Phase::Result::DONE_PHASE;
  }

  // Also need to check if we reached this node from opposite direction
  if (left.offset + right.offset >= P) {
    return Phase::Result::DONE_PHASE;
  }

  int dir;
  // If we are not done in both directions pick the one where we sent
  // less messages
  if ((left.offset < left.min_recv) && (right.offset < right.min_recv)) {
    dir = ((left.offset < right.offset) ? DIR_LEFT : DIR_RIGHT);
  } else {
    // Here one of the directions is done, so we just pick another one
    dir = ((left.offset < left.min_recv) ? DIR_LEFT : DIR_RIGHT);
  }

  Ring &send_dir = dir == DIR_LEFT ? left : right;

  // Record next offset
  send_dir.offset += 1;

  // We found a direction, so now we need to send, but first we have
  // to check if we are going to send in the first place.
  int receiver = (node_id + send_dir.offset * dir + P) % P;
  if (!send_over_root) {
    if ((dir == DIR_LEFT && receiver == P - 1) || (dir == DIR_RIGHT && receiver == 0)) {
      // We are actually trying to send over run, although we shouldn't
      // So, instead of sending over root send in opposite direction
      send_dir.min_recv = 1;
      return post_message(tq, node_id);
    }
  }

  assert(receiver != node_id);

  // all reached nodes send out correction messages
  if (dir == DIR_LEFT) {
    tq.schedule(SendStartTask::make_new(Tag::RING_LEFT,
                                        tq.now(), node_id, receiver));
  } else {
    tq.schedule(SendStartTask::make_new(Tag::RING_RIGHT,
                                        tq.now(), node_id, receiver));
  }

  return Phase::Result::ONGOING;
}

template<bool send_over_root>
Phase::Result
CheckedCorrectionPhase<send_over_root>::dispatch(
  const InitTask&, TaskQueue &tq, int node_id)
{
  assert(this->reached_nodes[node_id] && "Init on unreached node");

  return post_message(tq, node_id);
}

template<bool send_over_root>
Phase::Result
CheckedCorrectionPhase<send_over_root>::dispatch(
  const IdleTask&, TaskQueue &tq, int node_id)
{
  return post_message(tq, node_id);
}

template<bool send_over_root>
Phase::Result
CheckedCorrectionPhase<send_over_root>::dispatch(
  const RecvEndTask &t, TaskQueue&, int node_id)
{
  Ring &left = this->left[node_id];
  Ring &right = this->right[node_id];

  int P = this->num_nodes();
  int dist;
  // The message comes from the right, so it is remembered by the
  // left ring
  if (t.tag() == Tag::RING_RIGHT) {
    dist = (P + node_id - t.sender()) % P;
    left.min_recv = std::min(left.min_recv, dist);
  } else if (t.tag() == Tag::RING_LEFT) {
    dist = (P + t.sender() - node_id) % P;
    right.min_recv = std::min(right.min_recv, dist);
  } else {
    throw std::runtime_error("Unknown tag");
  }
  return Phase::Result::ONGOING;
}

template class CheckedCorrectionPhase<true>;
template class CheckedCorrectionPhase<false>;
