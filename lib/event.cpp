#include "event.hpp"
#include "model.hpp"
#include "globals.hpp"

Time CpuEvent::end() const
{
  return start() + Globals::get().model().o;
}

Time SendGap::end() const
{
  return start() + Globals::get().model().g;
}

Time RecvGap::end() const
{
  return start() + Globals::get().model().g;
}

Time FinishEvent::end() const
{
  return start();
}

Time FailureEvent::end() const
{
  return start();
}
