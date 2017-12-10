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

NodeDemux::NodeDemux(std::unique_ptr<Phase> &&p)
  : phase(std::move(p))
{
  assert(phase && "Invalid phase");
}

void NodeDemux::accept(const InitTask &t, TaskQueue &tq)
{
  const int node_id = t.sender();

//   assert(is_reached(node_id) && "Init on unreached node");
  forward(t, tq, node_id);
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
//   mark_reached(node_id);
  forward(t, tq, node_id);
  tq.schedule(IdleTask::make_new(node_id));
}
