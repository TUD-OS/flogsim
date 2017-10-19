#pragma once

#include <cassert>

#include <deque>
#include <ostream>
#include <iterator>

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

  static std::string header()
  {
    return "CpuEvent";
  }
};

struct SendGap : public Event
{
  SendGap() = default;

  SendGap(Time time) :
    Event{time}
  {}

  Time end() const override;

  static std::string header()
  {
    return "SendGap";
  }
};

struct RecvGap : public Event
{
  RecvGap() = default;

  RecvGap(Time time) :
    Event{time}
  {}

  Time end() const override;

  static std::string header()
  {
    return "RecvGap";
  }
};

struct FinishEvent : public Event
{
  FinishEvent() = default;

  FinishEvent(Time time) :
    Event(time)
  {}

  Time end() const override;

  static std::string header()
  {
    return "Finish";
  }
};

template<class T>
struct EventQueue
{
  std::deque<T> items;

  friend std::ostream &operator<<(std::ostream &os, const EventQueue &eq)
  {
    std::copy(eq.items.begin(), eq.items.end(),
              std::ostream_iterator<Event>(os, " "));
    return os;
  }

  void push_back(T item)
  {
    items.push_back(item);
  }

  auto back()
  {
    return items.back();
  }

  Time get_last_or_zero() const
  {
    if (items.empty())
      return Time(0);
    return items.back().end();
  }

  static std::string header()
  {
    return T::header();
  }
};
