#pragma once

#include <memory>
#include "collective.hpp"
#include "task_queue.hpp"


/* Act as a collective and forward relevant Tasks to individual nodes */
class NodeDemux : public Collective {

  std::unique_ptr<Phase> phase;

  template <class Task>
  void forward(const Task &t, TaskQueue &tq, const int node_id)
  {
    Phase::Result res = phase->dispatch(t, tq, node_id);

    if (res == Phase::Result::DONE_PHASE || res == Phase::Result::DONE_COLL) {
      tq.schedule(FinishTask::make_new(node_id));
    }
  }

public:
  NodeDemux(std::unique_ptr<Phase> &&p)
    : phase(std::move(p))
  {
  }

  NodeDemux(const NodeDemux&) = delete;

  virtual void accept(const InitTask &t, TaskQueue &tq)
  {
    forward(t, tq, t.sender());
  }

  virtual void accept(const TimerTask &t, TaskQueue &tq)
  {
    forward(t, tq, t.sender());
  }

  virtual void accept(const IdleTask &t, TaskQueue &tq)
  {
    forward(t, tq, t.sender());
  }

  virtual void accept(const SendEndTask &t, TaskQueue &tq)
  {
    tq.schedule(IdleTask::make_new(t.sender()));
  }

  virtual void accept(const RecvEndTask& t, TaskQueue& tq)
  {
    forward(t, tq, t.receiver());
    tq.schedule(IdleTask::make_new(t.receiver()));
  }
};
