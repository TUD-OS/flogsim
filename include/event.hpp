#pragma once

#include <cassert>

#include <deque>
#include <ostream>
#include <iterator>
#include <vector>
#include <algorithm>

#include "tag.hpp"
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
  Tag tag;

  CpuEvent() = default;

  CpuEvent(Sequence seq, Time time, Tag tag) :
    Event{seq, time}, tag(tag)
  {}

  Time end() const override;

  friend std::ostream& operator<<(std::ostream &os, const CpuEvent& e)
  {
    os << e.seq.id << '|' << e.time << "|" << e.tag;
    return os;
  }

  static std::string header()
  {
    return "CpuEvent_Sequence|Time|Tag";
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
  // EventQueue is a wrapper around "event queue", which constists of
  // several "event threads". Each line contains a chronologically
  // ordered deque of events. Threads can have concurrently running
  // events.
private:
  struct EventThread
  {
    using EventDeque = std::deque<T>;
    typedef typename EventDeque::const_iterator const_iterator;
    EventDeque events;

    friend std::ostream &operator<<(std::ostream &os, const EventThread &ef)
    {
      std::copy(ef.events.begin(), ef.events.end(),
                std::ostream_iterator<T>(os, " "));
      return os;
    }

    auto size() const
    {
      return events.size();
    }

    bool empty() const
    {
      return size() == 0;
    }

    void push_back(T item)
    {
      events.push_back(item);
    }

    Time get_last_or_zero() const
    {
      if (events.empty()) {
        return Time(0);
      }
      return events.back().end();
    }

    EventThread()
      : events()
    {}

    static bool compare_ends(const EventThread &a, const EventThread &b)
    {
      return a.get_last_or_zero() < b.get_last_or_zero();
    }
  };

  using EventGroup = std::vector<EventThread>;

  class EventRange
  {
    const EventGroup &queue;
  public:
    EventRange(const EventGroup &queue)
      : queue(queue)
    {}

    class const_iterator : public std::iterator<std::forward_iterator_tag,T>
    {
      const EventGroup &queue;
      typename EventGroup::const_iterator thread;
      typename EventThread::const_iterator event;
      bool done;

    public:
      static const_iterator cend(const EventGroup &queue)
      {
        const_iterator it(queue);
        it.done = true;
        return it;
      }

      const_iterator(const EventGroup &queue)
        : queue(queue), thread(queue.cbegin()),
          event(thread->events.cbegin()),
          done(false)
      {
      }

      const_iterator &operator++()
      {
        // Simple case within thread
        ++ event;
        if (event != thread->events.cend()) {
          return *this;
        }

        // Need to switch thread
        while (++thread != queue.cend()) {
          event = thread->events.cbegin();
          if (event != thread->events.cend()) {
            return *this;
          }
        }

        // Completely done
        done = true;
        return *this;
      }

      bool operator==(const const_iterator &other) const
      {
        if (other.done && done && &other.queue == &queue) {
          return true;
        }
        return ((&other.queue == &queue)
                && (&other.thread == &thread)
                && (&other.event == &event));
      }

      bool operator!=(const const_iterator &other) const
      {
        return !(*this == other);
      }

      const T &operator*() const
      {
        return *event;
      }
    };
    const_iterator begin() { return const_iterator(queue); }
    const_iterator end() { return const_iterator::cend(queue); }
  };

  EventGroup queue;

  // Returns index of the thread, which allows earliest time
  auto which_earlist_start_time() const
  {
    return std::min_element(queue.begin(), queue.end(),
                            EventThread::compare_ends);
  }

  auto which_latest_start_time() const
  {
    return std::max_element(queue.begin(), queue.end(),
                            EventThread::compare_ends);
  }
public:
  EventRange events()
  {
    return EventRange(queue);
  }

  friend std::ostream &operator<<(std::ostream &os, const EventQueue &eq)
  {
    std::copy(eq.queue.begin(), eq.queue.end(),
              std::ostream_iterator<EventThread>(os, " "));
    return os;
  }

  bool empty() const
  {
    return std::all_of(queue.begin(), queue.end(),
                       [](const auto &thread) {
                         return thread.size() == 0;
                       });
  }

  // Push an event onto a thread. The algorithm choses a thread with
  // the least slack.
  void append(T item)
  {
    assert(item.start() >= earliest_start_time() && "Event start time should"
           " be not less than time returned by EventQueue.");

    int best_fit = -1;
    for (unsigned i = 0; i < queue.size(); i++) {
      Time cur_end = queue[i].get_last_or_zero();
      if (cur_end > item.start()) {
        continue;
      }

      if (best_fit == -1) {
        best_fit = i;
      } else if (cur_end > queue[best_fit].get_last_or_zero()) {
        best_fit = i;
      }
    }

    assert(best_fit != -1 && "Why previous assert did not trigger?!");

    queue[best_fit].push_back(item);
  }

  // Time when last thread ends its latest event
  Time end() const
  {
    return which_latest_start_time()->get_last_or_zero();
  }

  // Earliest time when at least some thread becomes idle and can
  // accept another event.
  Time earliest_start_time() const
  {
    return which_earlist_start_time()->get_last_or_zero();
  }

  static std::string header()
  {
    return T::header();
  }

  EventQueue(int width = 1)
    : queue(width)
  {}
};
