#pragma once

#include "collective.hpp"
#include "task_queue.hpp"


/* Act as a collective and forward relevant Tasks to individual nodes */
class NodeDemux : public Collective {

  Phase phase;

public:
  virtual void accept(const InitTask &t, TaskQueue &tq)
  {
    phase.do_phase(t, tq, task.sender());
  }

  virtual void accept(const TimerTask &t, TaskQueue &tq)
  {
    phase.do_phase(t, tq, task.sender());
  }

  virtual void accept(const IdleTask &t, TaskQueue &tq)
  {
    phase.do_phase(t, tq, task.sender());
  }

  virtual void accept(const SendEndTask &task, TaskQueue &tq)
  {
    // setup idle task to inform us if no message arrives in current slot
    tq.schedule(IdleTask::make_new(task.sender()));
  }

  virtual void accept(const RecvEndTask& task, TaskQueue& tq)
  {
    phase.do_phase(t, tq, task.receiver());
    tq.schedule(IdleTask::make_new(task.receiver()));
  }
};
