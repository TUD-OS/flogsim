#pragma once

#include <cassert>

#include <deque>
#include <ostream>
#include <iterator>

#include "time.hpp"
#include "sequence.hpp"

struct Event
{
  Time start() const
  {
    return time;
  }

  virtual Time end() const = 0;

  friend std::ostream& operator<<(std::ostream &os, const Event& e)
  {
    os << e.seq.id << '|' << e.time;
    return os;
  }

  Event() = default;

  Event(Sequence seq, Time time) :
    seq(seq), time(time)
  {}

protected:
  Sequence seq;
  Time time;
};

struct CpuEvent : public Event
{
  CpuEvent() = default;

  CpuEvent(Sequence seq, Time time) :
    Event{seq, time}
  {}

  Time end() const override;

  static std::string header()
  {
    return "CpuEvent_Sequence|Time";
  }
};

struct SendGap : public Event
{
  SendGap() = default;

  SendGap(Sequence seq, Time time) :
    Event{seq, time}
  {}

  Time end() const override;

  static std::string header()
  {
    return "SendGap_Sequence|Time";
  }
};

struct RecvGap : public Event
{
  RecvGap() = default;

  RecvGap(Sequence seq, Time time) :
    Event{seq, time}
  {}

  Time end() const override;

  static std::string header()
  {
    return "RecvGap_Sequence|Time";
  }
};

struct FinishEvent : public Event
{
  FinishEvent() = default;

  FinishEvent(Sequence seq, Time time) :
    Event(seq, time)
  {}

  Time end() const override;

  static std::string header()
  {
    return "Finish_Sequence|Time";
  }
};

struct FailureEvent : public Event
{
  int sender;

  FailureEvent() = default;

  FailureEvent(Sequence seq, Time time) :
    Event(seq, time)
  {}

  Time end() const override;

  static std::string header()
  {
    return "Failure_Sequence|Time";
  }
};

template<class T>
struct EventQueue
{
  std::deque<T> items;

  friend std::ostream &operator<<(std::ostream &os, const EventQueue &eq)
  {
    std::copy(eq.items.begin(), eq.items.end(),
              std::ostream_iterator<T>(os, " "));
    return os;
  }

  bool empty() const
  {
    return items.size() == 0;
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
