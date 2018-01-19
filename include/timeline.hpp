#pragma once

#include <vector>
#include <ostream>
#include <sstream>
#include <algorithm>
#include <tuple>

#include "event.hpp"
#include "globals.hpp"

struct CpuTimeline
{
  EventQueue<CpuEvent> cpu_events;
  EventQueue<SendGap> send_gaps;
  EventQueue<RecvGap> recv_gaps;
  EventQueue<FinishEvent> finish;
  EventQueue<FailureEvent> failure;

  CpuTimeline()
    : cpu_events(Globals::get().model().parallelism),
      send_gaps(),
      recv_gaps(),
      finish(),
      failure()
  {}

  friend std::ostream &operator<<(std::ostream &os, const CpuTimeline &ctl)
  {
    os << ctl.cpu_events << ','
       << ctl.send_gaps << ','
       << ctl.recv_gaps << ','
       << ctl.finish << ','
       << ctl.failure;
    return os;
  }

  static std::string header()
  {
    std::stringstream ss;
    ss << decltype(cpu_events)::header() << ','
       << decltype(send_gaps)::header() << ','
       << decltype(recv_gaps)::header() << ','
       << decltype(finish)::header() << ','
       << decltype(failure)::header();
    return ss.str();
  }
};

class Timeline
{
  Time total_time;
public:
  std::vector<CpuTimeline> per_cpu_time;

  Timeline() :
    per_cpu_time(Globals::get().model().P)
  {}

  Time get_total_time() const { return total_time; }
  void update_total_time(Time time)
  {
    if (total_time < time) {
      total_time = time;
    }
  }

  std::tuple<int, int, int> node_stat() const
  {
    int failed = 0, finished = 0, unreached = 0;
    for (auto &cpu : per_cpu_time) {
      if (!cpu.failure.empty()) {
        failed++;
      } else if (!cpu.finish.empty()) {
        finished++;
      } else {
        unreached++;
      }
    }
    return {failed, finished, unreached};
  }

  void store_metrics();

  friend std::ostream &operator<<(std::ostream &os, const Timeline &tl)
  {
    os << "CPU," << CpuTimeline::header() << '\n';
    for(unsigned i = 0; i < tl.per_cpu_time.size(); i++) {
      auto const &cpu = tl.per_cpu_time[i];
      os << i << ',' << cpu << '\n';
    }
    return os;
  }
};
