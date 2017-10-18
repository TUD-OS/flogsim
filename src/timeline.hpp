#pragma once

#include <deque>
#include <vector>
#include <algorithm>

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

  void dump() const
  {
    auto dumper = [] (const Event &e) {
      e.dump();
      std::printf(" ");
    };
    std::for_each(cpu_events.begin(), cpu_events.end(), dumper);
    std::printf(",");
    std::for_each(send_gaps.begin(), send_gaps.end(), dumper);
    std::printf(",");
    std::for_each(recv_gaps.begin(), recv_gaps.end(), dumper);
    std::printf(",");
    finish.dump();
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

  void dump() const
  {
    std::printf("CPU, CpuEvent, SendGaps, RecvGaps, Finish\n");
    for(int i = 0; i < per_cpu_time.size(); i++) {
      auto const &cpu = per_cpu_time[i];
      std::printf("%d,", i);
      cpu.dump();
      std::printf("\n");
    }
  }
};
