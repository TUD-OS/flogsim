#include <assert.h>
#include <stdexcept>

#include "exclusive_phase.hpp"
#include "task.hpp"
#include "task_queue.hpp"

using Result = Phase::Result;


Result ExclusivePhase::forward(const auto &t, TaskQueue &tq, const int node_id)
{
  // handle our own tasks...
  if (t.tag() == Tag::EXCLUSIVE) {
    // we only ever use TimerTask ourselves
    assert(typeid(t) == typeid(TimerTask) && "Unexpected task type");
    return Result::DONE_PHASE;
  }

  // ... forward anything else
  Result res = phase->dispatch(t, tq, node_id);

  switch (res) {
    case Result::ONGOING: [[fallthrough]]
    case Result::DONE_COLL:
      break;

    case Result::DONE_PHASE: // delay nodes that finish the phase early
    {
      const Time abs_deadline = start + rel_deadline;

      if (abs_deadline <= tq.now()) { break; }

      tq.schedule(TimerTask::make_new(Tag::EXCLUSIVE, abs_deadline, node_id));
      res = Result::ONGOING;
    }
  }

  return res;
}

ExclusivePhase::ExclusivePhase(ReachedNodes &reached_nodes, PhasePtr &&_phase)
  : Phase(reached_nodes),
    phase(std::move(_phase)),
    start(Time::max()), // cannot initialise, we don't know the time yet
    rel_deadline(Time::max()) // cannot initialise, 'phase' may hold 'nullptr'
{
  assert(phase.get() && "Invalid phase");

  if (phase->deadline() == Time::max()) {
    throw std::invalid_argument("Provided phase without specified deadline");
  }

  rel_deadline = phase->deadline();
}


Result
ExclusivePhase::dispatch(const InitTask &t, TaskQueue &tq, int node_id)
{
  const Time now = tq.now();
  assert((start == Time::max() || start == now) && "Skewed init");

  return forward(t, tq, node_id);
}

Result
ExclusivePhase::dispatch(const IdleTask &t, TaskQueue &tq, int node_id)
{
  return forward(t, tq, node_id);
}

Result
ExclusivePhase::dispatch(const TimerTask &t, TaskQueue &tq, int node_id)
{
  return forward(t, tq, node_id);
}

Result
ExclusivePhase::dispatch(const RecvEndTask &t, TaskQueue &tq, int node_id)
{
  return forward(t, tq, node_id);
}
