#include <assert.h>

#include "node_demux.hpp"
#include "phase.hpp"
#include "task_queue.hpp"


void NodeDemux::forward(const auto &t, TaskQueue &tq, const int node_id)
{
  Phase::Result res = phase->dispatch(t, tq, node_id);

  if (res == Phase::Result::DONE_PHASE || res == Phase::Result::DONE_COLL) {
    tq.schedule(FinishTask::make_new(node_id));
  }
}

void NodeDemux::accept(const InitTask &t, TaskQueue &tq)
{
  forward(t, tq, t.sender());
}

void NodeDemux::accept(const TimerTask &t, TaskQueue &tq)
{
  forward(t, tq, t.sender());
}

void NodeDemux::accept(const IdleTask &t, TaskQueue &tq)
{
  forward(t, tq, t.sender());
}

void NodeDemux::accept(const SendEndTask &t, TaskQueue &tq)
{
  tq.schedule(IdleTask::make_new(t.sender()));
}

void NodeDemux::accept(const RecvEndTask& t, TaskQueue& tq)
{
  const int node_id = t.receiver();
  forward(t, tq, node_id);
  tq.schedule(IdleTask::make_new(node_id));
}

Timeline NodeDemux::run(std::unique_ptr<Phase> &&_phase)
{
  phase = std::move(_phase);

  // Here we basically run it
  TaskQueue tq{faults.get()};
  Timeline timeline;

  // broadcast InitTask to all (initially) reached nodes
  for (size_t i = 0; i < reached_nodes.size(); i++) {
    if (reached_nodes[i]) {
      tq.schedule(InitTask::make_new(Time(0), i));
    }
  }

  tq.run(*this, timeline);

  return timeline;
}
