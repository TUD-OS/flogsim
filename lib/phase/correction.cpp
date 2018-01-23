#include <assert.h>
#include <algorithm>
#include <cmath>

#include "task_queue.hpp"
#include "phase/correction.hpp"

using Result = Phase::Result;

Correction::Correction(ReachedNodes &reached_nodes)
  : Phase(reached_nodes)
{
  assert(std::any_of(reached_nodes.begin(),
                     reached_nodes.end(),
                     [] (bool reached) {return reached;})
         && "No reached node");
}


template<bool send_over_root, bool optimised>
OpportunisticCorrection<send_over_root, optimised>::OpportunisticCorrection(
  ReachedNodes &reached_nodes)
  : Correction(reached_nodes),
    send_state(this->num_nodes()),
    max_dist(Globals::get().conf().k)
{
  assert(max_dist < this->num_nodes() && "Nonsensical correction distance");
}

template<bool send_over_root, bool optimised>
Result
OpportunisticCorrection<send_over_root, optimised>::dispatch(
  const InitTask &, TaskQueue &tq, int node_id)
{
  tq.schedule(IdleTask::make_new(node_id));
  return Result::ONGOING;
}

template<bool send_over_root, bool optimised>
Result
OpportunisticCorrection<send_over_root, optimised>::dispatch(
  const IdleTask &, TaskQueue &tq, int node_id)
{
  if(!reached_nodes[node_id]) {
    return Result::ONGOING;
  }

  // reached nodes send out correction messages, one by one
  int receiver = node_id - ++sent_dist[node_id];

  if constexpr (send_over_root) {
    receiver = (receiver + this->num_nodes()) % this->num_nodes();
  }

  if (receiver >= 0) {
    tq.schedule(SendStartTask::make_new(Tag::RING_LEFT, tq.now(), node_id, receiver));
  }

  if (sent_dist[node_id] >= max_dist) {
    return Result::DONE_PHASE;
  } else {
    tq.schedule(IdleTask::make_new(node_id));
    return Result::ONGOING;
  }
}

template<bool send_over_root, bool optimised>
Result
OpportunisticCorrection<send_over_root, optimised>::dispatch(
  const RecvEndTask &t, TaskQueue &tq, int node_id)
{
  this->reached_nodes[node_id] = true;

  if constexpr (optimised) {
// do not send to nodes the one who sent to us will cover anyway
      int dist;

      int *max_covered;
      switch (t.tag()) {
        case Tag::RING_RIGHT:
          max_covered = &send_state[node_id];
          dist = (P + node_id - t.sender()) % P;
          break;
        case Tag::RING_LEFT:
          max_covered = &send_state[node_id];
          dist = (P + t.sender() - node_id) % P;
          break;
        default:
          break;
      }

      assert(*max_covered <= max_dist && "Should have stopped correction");
      *max_covered = std::max(*max_covered, dist);

      sent_dist[node_id] = std::max(sent_dist[node_id], remain);

    if (sent_dist[node_id] <= 0) {
      return Result::DONE_PHASE;
    }
  }

  tq.schedule(IdleTask::make_new(node_id));
  return Result::ONGOING;
}

template<bool send_over_root, bool optimised>
Time
OpportunisticCorrection<send_over_root, optimised>::deadline() const
{
  auto &model = Globals::get().model();
  auto o = model.o;
  auto g = model.g;

  return o + (max_dist - 1) * std::max(o,g);
}

// explicit instantiation
template class OpportunisticCorrection<true,true>;
template class OpportunisticCorrection<true,false>;
template class OpportunisticCorrection<false,true>;
template class OpportunisticCorrection<false,false>;



// CheckedCorrection

template<bool send_over_root>
CheckedCorrection<send_over_root>::CheckedCorrection(
  ReachedNodes &reached_nodes)
  : Correction(reached_nodes),
    left(this->num_nodes()),
    right(left.size()),
    is_now_left(left.size()),
    is_throttling(false)
{
}

// This function should decide which direction we are sending to
template<bool send_over_root>
int CheckedCorrection<send_over_root>::get_dir(int node_id)
{
  Ring &left = this->left[node_id];
  Ring &right = this->right[node_id];

  // If we are throttling, we should only flip the direction
  if (is_throttling) {
    bool is_left = this->is_now_left[node_id];
    this->is_now_left[node_id].flip();
    return is_left ? DIR_LEFT : DIR_RIGHT;
  }

  // If we are not throttling, check what direction is not done. If
  // neither is, pick the one where we sent less messages
  if ((left.offset < left.min_recv) && (right.offset < right.min_recv)) {
    return ((left.offset <= right.offset) ? DIR_LEFT : DIR_RIGHT);
  } else {
    // Here one of the directions is done, so we just pick another one
    return ((left.offset < left.min_recv) ? DIR_LEFT : DIR_RIGHT);
  }
}

template<bool send_over_root>
bool CheckedCorrection<send_over_root>::should_skip(
  TaskQueue &tq, Ring &ring, int node_id)
{
  if (!is_throttling) {
    assert((ring.offset < ring.min_recv) &&
           "Throttling is off. The ring expected to be active");
    return false;
  }

  // If ring is active, do not skip the slot
  if (ring.offset < ring.min_recv) {
    return false;
  }

  // If yes skip some time
  auto &model = Globals::get().model();
  auto o = model.o;
  tq.schedule(TimerTask::make_new(tq.now() + o, node_id));
  return true;
}

template<bool send_over_root>
Result
CheckedCorrection<send_over_root>::post_message(
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
    return Result::DONE_PHASE;
  }

  // Also need to check if we reached this node from opposite direction
  if (left.offset + right.offset >= P) {
    return Result::DONE_PHASE;
  }

  int dir = this->get_dir(node_id);

  Ring &send_dir = dir == DIR_LEFT ? left : right;

  if (this->should_skip(tq, send_dir, node_id)) {
    return Result::ONGOING;
  }
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

  return Result::ONGOING;
}

template<bool send_over_root>
Result
CheckedCorrection<send_over_root>::dispatch(
  const TimerTask&, TaskQueue &tq, int node_id)
{
  tq.schedule(IdleTask::make_new(node_id));
  return Result::ONGOING;
}

template<bool send_over_root>
Result
CheckedCorrection<send_over_root>::dispatch(
  const InitTask&, TaskQueue &tq, int node_id)
{
  if (reached_nodes[node_id]) {
    // We were not reached in previous phase, we declare that we want
    // to participate in the correction
    tq.schedule(IdleTask::make_new(node_id));
  }
  return Result::ONGOING;
}

template<bool send_over_root>
Result
CheckedCorrection<send_over_root>::dispatch(
  const IdleTask&, TaskQueue &tq, int node_id)
{
  return post_message(tq, node_id);
}

template<bool send_over_root>
Result
CheckedCorrection<send_over_root>::dispatch(
  const RecvEndTask &t, TaskQueue&, int node_id)
{
  Ring &left = this->left[node_id];
  Ring &right = this->right[node_id];

  int P = this->num_nodes();
  int dist;
  // The message comes from the right, so it is remembered by the
  // left ring
  switch (t.tag()) {
    case Tag::RING_RIGHT:
      dist = (P + node_id - t.sender()) % P;
      left.min_recv = std::min(left.min_recv, dist);
      break;
    case Tag::RING_LEFT:
      dist = (P + t.sender() - node_id) % P;
      right.min_recv = std::min(right.min_recv, dist);
      break;
    default:
      break;
  }

  if (!this->reached_nodes[node_id]) {
    this->reached_nodes[node_id] = true;
    return Result::DONE_PHASE;
  }

  return Result::ONGOING;
}

template<bool send_over_root>
Result
CheckedCorrection<send_over_root>::dispatch(
  const SendEndTask &, TaskQueue &tq, int node_id)
{
  tq.schedule(IdleTask::make_new(node_id));
  return Result::ONGOING;
}

template<bool send_over_root>
Result
CheckedCorrection<send_over_root>::dispatch(
  const FinishTask &, TaskQueue &, int node_id)
{
  // Everybody except root skips this task
  if (node_id) {
    return Result::ONGOING;
  }

  auto &metrics = Globals::get().metrics();

  int recv_left = -1, recv_right = -1, missing_around = -1;
  for (int i = 0; i < num_nodes(); i++) {
    Ring &left = this->left[node_id];
    Ring &right = this->right[node_id];

    if (left.min_recv == std::numeric_limits<int>::max() ||
        right.min_recv == std::numeric_limits<int>::max()) {
      // The node did not participate in the correction, skip it
      continue;
    }

    recv_left = std::max(recv_left, left.min_recv - 1);
    recv_right = std::max(recv_right, right.min_recv - 1);
    missing_around = std::max(recv_left + recv_right, missing_around);
  }

  metrics["CorrectedGapLeft"] = recv_left;
  metrics["CorrectedGapRight"] = recv_right;
  metrics["CorrectedGap"] = missing_around;
  return Result::ONGOING;

}

template class CheckedCorrection<true>;
template class CheckedCorrection<false>;
