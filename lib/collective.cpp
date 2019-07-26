#include <cmath>
#include <memory>

#include "configuration.hpp"
#include "globals.hpp"
#include "collective.hpp"
#include "task_queue.hpp"
#include "phase.hpp"
#include "fault_injector.hpp"
#include "topology/topology.hpp"

// Enable root selected
Collective::Collective(FaultInjector *faults, Topology _topology)
  : done_nodes(Globals::get().model().P),
    coloured_nodes(Globals::get().model().P),
    reached_nodes(Globals::get().model().P),
    topology(_topology),
    faults(faults)
{
  coloured_nodes[0] = true;
}

Collective::Collective(std::initializer_list<int> selected,
                       FaultInjector *faults,
                       Topology _topology)
  : done_nodes(Globals::get().model().P),
    coloured_nodes(Globals::get().model().P),
    reached_nodes(Globals::get().model().P),
    topology(_topology),
    faults(faults)
{
  for (auto i : selected) {
    reached_nodes[i] = true;
  }

  coloured_nodes[0] = true;
}

void Collective::forward(const auto &t, TaskQueue &tq, const int node_id)
{
  Phase::Result res = phase->dispatch(t, tq, node_id);

  if (res == Phase::Result::DONE_PHASE || res == Phase::Result::DONE_COLL) {
    if (!done_nodes[node_id] && reached_nodes[node_id]) {
      tq.schedule(FinishTask::make_new(node_id));
      done_nodes[node_id] = true;
    }
  }
}

void Collective::accept(const InitTask &t, TaskQueue &tq)
{
  forward(t, tq, t.sender());
}

void Collective::accept(const TimerTask &t, TaskQueue &tq)
{
  forward(t, tq, t.sender());
}

void Collective::accept(const IdleTask &t, TaskQueue &tq)
{
  forward(t, tq, t.sender());
}

void Collective::accept(const SendEndTask &t, TaskQueue &tq)
{
  forward(t, tq, t.sender());
}

void Collective::accept(const SendStartTask &t, TaskQueue &tq)
{
  forward(t, tq, t.sender());
}

void Collective::accept(const RecvStartTask &t, TaskQueue &tq)
{
  forward(t, tq, t.sender());
}

void Collective::accept(const RecvEndTask &t, TaskQueue &tq)
{
  const int node_id = t.receiver();
  forward(t, tq, node_id);

  if (!coloured_nodes[node_id]) {
    coloured_time = std::max(tq.now(), coloured_time);
    coloured_nodes[node_id] = true;
  }
}

void Collective::accept(const MsgTask&, TaskQueue&)
{
}

void Collective::accept(const FinishTask &t, TaskQueue &tq)
{
  forward(t, tq, t.sender());

  auto &metrics = Globals::get().metrics();

  metrics["ColouringTime"] = coloured_time.get();
}

void Collective::accept(const FailureTask&, TaskQueue&)
{
}

void Collective::run(Timeline &timeline, std::unique_ptr<Phase> &&_phase)
{
  phase = std::move(_phase);

  // Here we basically run it
  TaskQueue tq{faults};

  // broadcast InitTask to all (initially) reached nodes
  for (size_t i = 0; i < reached_nodes.size(); i++) {
    tq.schedule(InitTask::make_new(Time(0), i));
  }

  tq.run(*this, timeline);
}
