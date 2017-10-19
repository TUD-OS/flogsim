#pragma once

#include <cassert>

#include <ostream>

#include "time.hpp"

struct Event
{
  Time start() const
  {
    return time;
  }

  virtual Time end() const = 0;

  friend std::ostream& operator<<(std::ostream &os, const Event& e)
  {
    os << e.time;
    return os;
  }

  Event() = default;

  Event(Time time) :
    time(time)
  {}

protected:
  Time time;
};

struct CpuEvent : public Event
{
  CpuEvent() = default;

  CpuEvent(Time time) :
    Event{time}
  {}

  Time end() const override;
};

struct SendGap : public Event
{
  SendGap() = default;

  SendGap(Time time) :
    Event{time}
  {}

  Time end() const override;
};

struct RecvGap : public Event
{
  RecvGap() = default;

  RecvGap(Time time) :
    Event{time}
  {}

  Time end() const override;
};

struct FinishEvent : public Event
{
  FinishEvent() = default;

  FinishEvent(Time time) :
    Event(time)
  {}

  Time end() const override;
};
