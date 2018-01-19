#include <assert.h>
#include <stdexcept>

#include "task.hpp"
#include "task_queue.hpp"

#include "phase/exclusive.hpp"

using Result = Phase::Result;


Result Exclusive::forward(const auto &t, TaskQueue &tq, const int node_id)
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
      res = Result::ONGOING;
    }
  }

  return res;
}

Exclusive::Exclusive(ReachedNodes &reached_nodes, PhasePtr &&_phase)
  : Phase(reached_nodes),
    phase(std::move(_phase)),
    start(Time::max()), // cannot initialise, we don't know the time yet
    rel_deadline(Time::max()) // cannot initialise, 'phase' may hold 'nullptr'
{
  assert(phase && "Invalid phase");

  if (phase->deadline() == Time::max()) {
    throw std::invalid_argument("Provided phase without specified deadline");
  }

  rel_deadline = phase->deadline();
}


Result
Exclusive::dispatch(const InitTask &t, TaskQueue &tq, int node_id)
{
  const Time now = tq.now();
  if (start == Time::max()) {
    start = now;
  }

  const Time abs_deadline = start + rel_deadline;

  if (abs_deadline <= tq.now()) {
    return Result::DONE_PHASE;
  }

  tq.schedule(TimerTask::make_new(Tag::EXCLUSIVE, abs_deadline, node_id));
  return forward(t, tq, node_id);
}

Result
Exclusive::dispatch(const IdleTask &t, TaskQueue &tq, int node_id)
{
  return forward(t, tq, node_id);
}

Result
Exclusive::dispatch(const TimerTask &t, TaskQueue &tq, int node_id)
{
  return forward(t, tq, node_id);
}

Result
Exclusive::dispatch(const RecvEndTask &t, TaskQueue &tq, int node_id)
{
  return forward(t, tq, node_id);
}

Result
Exclusive::dispatch(const FinishTask &t, TaskQueue &tq, int node_id)
{
  return forward(t, tq, node_id);
}
