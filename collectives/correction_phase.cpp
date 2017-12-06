#include <algorithm>

#include "correction_phase.hpp"
#include "task_queue.hpp"


template<bool send_over_root>
CorrectionPhase<send_over_root>::CorrectionPhase(ReachedPtr reached_nodes)
  : Phase(reached_nodes)
{
  assert(reached_nodes && std::any_of(reached_nodes->begin(),
                                      reached_nodes->end(),
                                      [](bool reached) {return reached;})
                       && "No reached node");
}

template<bool send_over_root>
Phase::Result
OpportunisticCorrectionPhase<send_over_root>::dispatch(
  const InitTask &t, TaskQueue &tq, int node_id)
{
  // all reached nodes send out correction messages (TODO: left only!?)
  for (int offset = 1; offset <= this->arity - 1; ++offset) {
    int receiver = (node_id + (CorrectionPhase<send_over_root>::DIR_LEFT * offset));

    if (send_over_root) {
      receiver =  (receiver + this->num_nodes) % this->num_nodes;
    }

    if (receiver >= 0) {
      tq.schedule(SendStartTask::make_new(Tag::RING_LEFT, tq.now(), node_id, receiver));
    }
  }

  tq.schedule(FinishTask::make_new(node_id));
  return Phase::Result::DONE_PHASE;
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
