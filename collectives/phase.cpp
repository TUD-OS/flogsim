#include "time.hpp"
#include "phase.hpp"

#include <assert.h>

using Result = Phase::Result;


Phase::Phase(ReachedPtr reached_nodes)
  : num_nodes(Globals::get().model().P),
    reached_nodes(reached_nodes)
{
  assert(reached_nodes && "Invalid reached-nodes info");
}

bool Phase::is_reached(int node_id)
{
  return (*reached_nodes)[node_id];
}

void Phase::mark_reached(int node_id)
{
  (*reached_nodes)[node_id] = true;
}

Result Phase::dispatch(const IdleTask &, TaskQueue &, int)
{
  return Result::ONGOING;
}

Result Phase::dispatch(const TimerTask &, TaskQueue &, int)
{
  return Result::ONGOING;
}

Result Phase::dispatch(const RecvEndTask &, TaskQueue &, int)
{
  return Result::ONGOING;
}

Time Phase::deadline() const
{
  return Time::max(); // infinite/unpredictable runtime
}
