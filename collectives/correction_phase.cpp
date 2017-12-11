#include <assert.h>
#include <algorithm>
#include <cmath>

#include "correction_phase.hpp"
#include "task_queue.hpp"


template<bool send_over_root>
CorrectionPhase<send_over_root>::CorrectionPhase(ReachedPtr reached_nodes)
  : Phase(reached_nodes)
{
  assert(reached_nodes && std::any_of(reached_nodes->begin(),
                                      reached_nodes->end(),
                                      [] (bool reached) {return reached;})
                       && "No reached node");
}


template<bool send_over_root>
OpportunisticCorrectionPhase<send_over_root>::OpportunisticCorrectionPhase(
  Phase::ReachedPtr reached_nodes, size_t max_dist)
  : CorrectionPhase<send_over_root>(reached_nodes),
    max_dist(max_dist)
{
  assert(max_dist < this->num_nodes && "Nonsensical correction distance");
}

template<bool send_over_root>
Phase::Result
OpportunisticCorrectionPhase<send_over_root>::dispatch(
  const InitTask &, TaskQueue &tq, int node_id)
{
  // all reached nodes send out correction messages
  for (int offset = 1; offset <= max_dist - 1; ++offset) {
    int receiver = node_id - offset;

    if (send_over_root) {
      receiver = (receiver + this->num_nodes) % this->num_nodes;
    }

    if (receiver >= 0) {
      tq.schedule(SendStartTask::make_new(Tag::RING_LEFT, tq.now(), node_id, receiver));
    }
  }

  return Phase::Result::DONE_PHASE;
}

template<bool send_over_root>
Time
OpportunisticCorrectionPhase<send_over_root>::deadline(
  const int, const int o, const int g) const
{
  return o + (max_dist - 1) * std::max(o,g);
}

#if 0
OpportunisticCorrectionPhase::
{
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

  virtual void accept(const RecvStartTask& task, TaskQueue& tq)
  {
    post_sends(task.receiver(), tq);
  }

  virtual void accept(const InitTask &, TaskQueue &tq)
  {
    int root = 0;
    post_sends(root, tq);
  }
#endif
