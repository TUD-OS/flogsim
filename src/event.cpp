#include "event.hpp"
#include "model.hpp"

Time CpuEvent::end() const
{
  return start() + LogP::Model::get().o;
}

Time SendGap::end() const
{
  return start() + LogP::Model::get().g;
}

Time RecvGap::end() const
{
  return start() + LogP::Model::get().g;
}

Time FinishEvent::end() const
{
  return start();
}
