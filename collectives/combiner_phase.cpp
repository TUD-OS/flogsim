#include <algorithm>
#include <assert.h>

#include "task.hpp"
#include "task_queue.hpp"
#include "combiner_phase.hpp"

using Result = Phase::Result;

Result CombinerPhase::forward(const auto &t, TaskQueue &tq, const int node_id)
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

    case Result::DONE_PHASE:
      ++node_cur_phase; // switch to next phase

      // init next phase for this node iff it was reached
      if (reached_nodes[node_id]) {
        InitTask::make_new(tq.now(), node_id);
      }
      break;

    case Result::DONE_COLL:
      node_cur_phase = phases.size(); // skip any remaining phases
      break;
  }

  if (node_cur_phase < phases.size()) {
    assert(res == Result::DONE_PHASE || res == Result::ONGOING);
    res = Result::ONGOING; // we actually still have more work to do
  }

  return res;
}

CombinerPhase::CombinerPhase(ReachedNodes &reached_nodes, PhaseVec &&phases)
  : Phase(reached_nodes),
    phases(std::move(phases)),
    cur_phase(num_nodes(), 0)
{
  assert(std::all_of(phases.cbegin(),
                     phases.cend(),
                     [](const PhasePtr &pp) -> bool {return pp.get();})
         && "Invalid phases");
}

Result CombinerPhase::dispatch(const InitTask &t, TaskQueue &tq, int node_id)
{
  return forward(t, tq, node_id);
}

Result CombinerPhase::dispatch(const IdleTask &t, TaskQueue &tq, int node_id)
{
  return forward(t, tq, node_id);
}

Result CombinerPhase::dispatch(const TimerTask &t, TaskQueue &tq, int node_id)
{
  return forward(t, tq, node_id);
}

Result CombinerPhase::dispatch(const RecvEndTask &t, TaskQueue &tq, int node_id)
{
  return forward(t, tq, node_id);
}
