#pragma once

#include "globals.hpp"

class TaskQueue;
class InitTask;
class IdleTask;
class TimerTask;
class RecvEndTask;

/* Abstract interface of a collective phase for a single node */
class Phase
{
protected:
  const int num_nodes;

public:
  enum class Result
  {
    ONGOING    = 1,
    DONE_PHASE = 2,
    DONE_COLL  = 3,
  };

  Phase()
    : num_nodes(Globals::get().model().P)
  {
  }

  virtual Result do_phase(const InitTask &, TaskQueue &, int)
  {
    return Result::ONGOING;
  }

  virtual Result do_phase(const IdleTask &, TaskQueue &, int)
  {
    return Result::ONGOING;
  }

  virtual Result do_phase(const TimerTask &, TaskQueue &, int)
  {
    return Result::ONGOING;
  }

  virtual Result do_phase(const RecvEndTask &, TaskQueue &, int)
  {
    return Result::ONGOING;
  }
};
