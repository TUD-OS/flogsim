#pragma once

#include <vector>
#include <ostream>
#include <sstream>
#include <algorithm>

#include "event.hpp"

struct CpuTimeline
{
  EventQueue<CpuEvent> cpu_events;
  EventQueue<SendGap> send_gaps;
  EventQueue<RecvGap> recv_gaps;
  EventQueue<FinishEvent> finish;

  CpuTimeline() = default;

  friend std::ostream &operator<<(std::ostream &os, const CpuTimeline &ctl)
  {
    os << ctl.cpu_events << ','
       << ctl.send_gaps << ','
       << ctl.recv_gaps << ','
       << ctl.finish;
    return os;
  }

  static std::string header()
  {
    std::stringstream ss;
    ss << decltype(cpu_events)::header() << ','
       << decltype(send_gaps)::header() << ','
       << decltype(recv_gaps)::header() << ','
       << decltype(finish)::header();
    return ss.str();
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
    os << "CPU," << CpuTimeline::header() << '\n';
    for(int i = 0; i < tl.per_cpu_time.size(); i++) {
      auto const &cpu = tl.per_cpu_time[i];
      os << i << ',' << cpu << '\n';
    }
    return os;
  }
};
