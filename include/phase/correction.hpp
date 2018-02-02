#pragma once

#include <vector>
#include <limits>

#include "phase.hpp"

class Correction : public Phase
{
protected:
  enum Direction
  {
    DIR_LEFT  = -1,
    DIR_RIGHT = 1,
  };

  std::vector<bool> correction_participant;
  /** Child class calls this to confirm a node participating in
      correction */
  void confirm_correction(int node_id);
public:
  Correction(ReachedNodes &reached_nodes);

  Result dispatch(const InitTask&, TaskQueue &tq, int node_id) final;
};


template<bool send_over_root, bool optimised>
class OpportunisticCorrection : public Correction
{
  struct SendDist
  {
    int left;
    int right;
  };
  std::vector<SendDist> sent_dist;  // how many nodes have already
                                    // gotten correction
  const int left_dist;  // maximum distance to cover to the left
  const int right_dist;  // maximum distance to cover to the right

public:
  OpportunisticCorrection(ReachedNodes &reached_nodes);

  Phase::Result dispatch(const IdleTask &, TaskQueue &tq, int node_id) override;
  Phase::Result dispatch(const RecvEndTask &t, TaskQueue &tq, int node_id) override;
  Phase::Result dispatch(const FinishTask&, TaskQueue&, int node_id) override;

  Time deadline() const override;
};

template<bool send_over_root>
class CheckedCorrection : public Correction
{
  // relevant information per direction (array) and node (vector)
  struct Ring
  {
    int offset;   // send offset
    int min_recv; // distance to closest node we received from

    Ring() : offset(0), min_recv(std::numeric_limits<int>::max())
    {}
  };

  std::vector<Ring> left;
  std::vector<Ring> right;
  std::vector<bool> is_now_left;

  bool is_throttling;

  int get_dir(int node_id);
  bool should_skip(TaskQueue&, Ring&, int);
  Phase::Result post_message(TaskQueue &tq, int node_id);
public:
  CheckedCorrection(ReachedNodes &reached_nodes);

  // Sets throttled to true
  CheckedCorrection &throttled()
  {
    is_throttling = true;
    return *this;
  }

  Phase::Result dispatch(const TimerTask&, TaskQueue&, int node_id) override;
  Phase::Result dispatch(const IdleTask&, TaskQueue&, int node_id) override;
  Phase::Result dispatch(const RecvEndTask&, TaskQueue&, int node_id) override;
  Phase::Result dispatch(const SendEndTask&, TaskQueue&, int node_id) override;
  Phase::Result dispatch(const FinishTask&, TaskQueue&, int node_id) override;
};
