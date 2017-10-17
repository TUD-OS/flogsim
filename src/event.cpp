#include "event.hpp"
#include "model.hpp"

extern Model model;

Time CpuEvent::end() const
{
  return start() + model.o;
}

Time SendGap::end() const
{
  return start() + model.g;
}

Time RecvGap::end() const
{
  return start() + model.g;
}

Time FinishEvent::end() const
{
  return start();
}
