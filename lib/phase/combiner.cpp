#include <algorithm>
#include <assert.h>

#include "task.hpp"
#include "task_queue.hpp"
#include "phase/combiner.hpp"

using Result = Phase::Result;

Result Combiner::forward(const auto &t, TaskQueue &tq, const int node_id)
{
  size_t &node_cur_phase = cur_phase[node_id];
  if (node_cur_phase >= phases.size()) {
    // node has already finished, no forwarding
    return Result::DONE_PHASE;
  }

  Result res = phases[node_cur_phase]->dispatch(t, tq, node_id);

  switch (res) {
    case Result::ONGOING:
      break;

    case Result::DONE_PHASE: [[fallthrough]]
    case Result::DONE_FORWARD:
      ++node_cur_phase; // switch to next phase

      // init next phase for this node...
      if (node_cur_phase < phases.size()) {
        tq.schedule(InitTask::make_new(tq.now(), node_id));

        // ... and forward current task
        if (res == Result::DONE_FORWARD) {
          tq.schedule(InitTask::make_from_task(t));
        }
      }
      break;

    case Result::DONE_COLL:
      node_cur_phase = phases.size(); // skip any remaining phases
      break;
  }

  if (node_cur_phase < phases.size()) {
    assert(res == Result::DONE_PHASE || res == Result::ONGOING || res == Result::DONE_FORWARD);
    res = Result::ONGOING; // we actually still have more work to do
  }

  return res;
}

Combiner::Combiner(Phases &&_phases)
  : Phase(_phases.reached_nodes),
    phases(std::move(_phases.phases)),
    cur_phase(num_nodes(), 0)
{
  assert(std::all_of(phases.cbegin(),
                     phases.cend(),
                     [](const PhasePtr &pp) -> bool {return pp.get();})
         && "Invalid phases");
}

Result Combiner::dispatch(const InitTask &t, TaskQueue &tq, int node_id)
{
  return forward(t, tq, node_id);
}

Result Combiner::dispatch(const IdleTask &t, TaskQueue &tq, int node_id)
{
  return forward(t, tq, node_id);
}

Result Combiner::dispatch(const TimerTask &t, TaskQueue &tq, int node_id)
{
  return forward(t, tq, node_id);
}

Result Combiner::dispatch(const RecvEndTask &t, TaskQueue &tq, int node_id)
{
  return forward(t, tq, node_id);
}

Result Combiner::dispatch(const SendEndTask &t, TaskQueue &tq, int node_id)
{
  return forward(t, tq, node_id);
}

Result Combiner::dispatch(const FinishTask &t, TaskQueue &tq, int node_id)
{
  for (auto &phase : phases) {
    [[maybe_unused]] Result res;
    res = phase->dispatch(t, tq, node_id);
    assert(res == Result::ONGOING);
  }
  return Result::ONGOING;
}

Time Combiner::deadline() const
{
  Time length{0};
  for (const auto &phase : phases) {
    if (phase->deadline() == Time::max()) {
      return Time::max();
    }
    length = length + phase->deadline();
  }

  return length;
}
