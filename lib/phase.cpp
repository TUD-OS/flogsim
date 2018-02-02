#include "time.hpp"
#include "phase.hpp"

#include <assert.h>

using Result = Phase::Result;


Result Phase::dispatch(const IdleTask &, TaskQueue &, int)
{
  return Result::ONGOING;
}

Result Phase::dispatch(const TimerTask &, TaskQueue &, int)
{
  return Result::ONGOING;
}

Result Phase::dispatch(const RecvStartTask &, TaskQueue &, int)
{
  return Result::ONGOING;
}

Result Phase::dispatch(const RecvEndTask &, TaskQueue &, int)
{
  return Result::ONGOING;
}

Result Phase::dispatch(const SendStartTask &, TaskQueue &, int)
{
  return Result::ONGOING;
}

Result Phase::dispatch(const SendEndTask &, TaskQueue &, int)
{
  return Result::ONGOING;
}

Result Phase::dispatch(const FinishTask &, TaskQueue &, int)
{
  return Result::ONGOING;
}

Time Phase::deadline() const
{
  return Time::max(); // infinite/unpredictable runtime
}
