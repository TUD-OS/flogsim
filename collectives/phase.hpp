#pragma once

#include <memory>
#include <vector>

#include "globals.hpp"

class TaskQueue;
class InitTask;
class IdleTask;
class TimerTask;
class RecvEndTask;

/* Interface of a collective phase for a single node
 *
 * 'reached_nodes' holds information on which nodes have been reached. It is
 * both read and updated by each phase.
 */
class Phase
{
public:
  using ReachedVec = std::vector<bool>;
  using ReachedPtr = std::shared_ptr<ReachedVec>;

protected:
  const int num_nodes;
  ReachedPtr reached_nodes;

  bool is_reached(int node_id) // TODO: templatise with mix-ins (r,w,rw)
  {
    if (reached_nodes) {
      return (*reached_nodes)[node_id];
    }
  }

  void reached(int node_id)
  {
    if (reached_nodes) {
      (*reached_nodes)[node_id] = true;
    }
  }

public:
  enum class Result
  {
    ONGOING    = 1,
    DONE_PHASE = 2,
    DONE_COLL  = 3,
  };

  Phase() = delete;

  Phase(ReachedPtr reached_nodes)
    : num_nodes(Globals::get().model().P),
      reached_nodes(reached_nodes)
  {
  }

  virtual Result dispatch(const InitTask &, TaskQueue &, int)
  {
    return Result::ONGOING;
  }

  virtual Result dispatch(const IdleTask &, TaskQueue &, int)
  {
    return Result::ONGOING;
  }

  virtual Result dispatch(const TimerTask &, TaskQueue &, int)
  {
    return Result::ONGOING;
  }

  virtual Result dispatch(const RecvEndTask &, TaskQueue &, int)
  {
    return Result::ONGOING;
  }
};
