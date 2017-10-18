#pragma once

#include <deque>
#include <vector>
#include <algorithm>
#include <ostream>
#include <iterator>

#include "event.hpp"

struct CpuTimeline
{
  static Time get_last_or_zero(const std::deque<auto> &queue)
  {
    if (queue.empty())
      return Time(0);
    return queue.back().end();
  }

  std::deque<CpuEvent> cpu_events;
  std::deque<SendGap> send_gaps;
  std::deque<RecvGap> recv_gaps;
  FinishEvent finish;

  CpuTimeline() = default;

  friend std::ostream &operator<<(std::ostream &os, const CpuTimeline &ctl)
  {
    std::copy(ctl.cpu_events.begin(), ctl.cpu_events.end(),
              std::ostream_iterator<Event>(os, " "));
    os << ',';
    std::copy(ctl.send_gaps.begin(), ctl.send_gaps.end(),
              std::ostream_iterator<Event>(os, " "));
    os << ',';
    std::copy(ctl.recv_gaps.begin(), ctl.recv_gaps.end(),
              std::ostream_iterator<Event>(os, " "));
    os << ',';
    os << ctl.finish;
    return os;
  }
};

class Timeline
{
  Time total_time;
public:
  std::vector<CpuTimeline> per_cpu_time;

  Timeline(int nodes) :
    per_cpu_time(nodes)
  {}

  void update_total_time(Time time)
  {
    if (total_time < time) {
      total_time = time;
    }
  }


  friend std::ostream &operator<<(std::ostream &os, const Timeline &tl)
  {
    os << "CPU, CpuEvent, SendGaps, RecvGaps, Finish\n";
    for(int i = 0; i < tl.per_cpu_time.size(); i++) {
      auto const &cpu = tl.per_cpu_time[i];
      os << i << ',' << cpu << '\n';
    }
    return os;
  }
};
