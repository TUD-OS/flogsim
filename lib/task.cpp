#include "task.hpp"
#include "task_queue.hpp"
#include "globals.hpp"

bool RecvStartTask::execute(Timeline &timeline, TaskQueue &tq) const
{
  auto &cpu = timeline.per_cpu_time[receiver()];

  // Calculate time for CpuTime
  Time cpu_last = cpu.cpu_events.get_last_or_zero();
  Time recv_last = cpu.recv_gaps.get_last_or_zero();

  auto start_time = std::max({cpu_last, recv_last, tq.now()});

  if (start_time > tq.now()) {
    tq.schedule(make_task_attime(this, start_time));
    return false;
  }

  RecvGap rg{seq(), start_time};
  CpuEvent cpu_event{seq(), start_time, tag()};

  cpu.recv_gaps.push_back(rg);
  cpu.cpu_events.push_back(cpu_event);

  Time task_end = std::max(cpu_event.end(), rg.end());
  tq.schedule(RecvEndTask::make_from_task(this, task_end, sender(), receiver()));
  return true;
}

bool RecvEndTask::execute(Timeline &timeline, TaskQueue &tq) const
{
  return true;
}

bool MsgTask::execute(Timeline &timeline, TaskQueue &tq) const
{
  // Calculate time when receive task can be scheduled
  auto recv_time = tq.now() + Globals::get().model().L;

  tq.schedule(RecvStartTask::make_from_task(this, recv_time, sender(), receiver()));
  return true;
}

bool SendStartTask::execute(Timeline &timeline, TaskQueue &tq) const
{
  auto &cpu = timeline.per_cpu_time[sender()];

  // Calculate time for CpuTime
  Time cpu_last = cpu.cpu_events.get_last_or_zero();
  Time send_last = cpu.send_gaps.get_last_or_zero();

  auto start_time = std::max({cpu_last, send_last, tq.now()});

  if (start_time > tq.now()) {
    tq.schedule(make_task_attime(this, start_time));
    return false;
  }

  SendGap sg{seq(), start_time};
  CpuEvent cpu_event{seq(), start_time, tag()};

  cpu.send_gaps.push_back(sg);
  cpu.cpu_events.push_back(cpu_event);

  Time task_end = std::max(cpu_event.end(), sg.end());
  tq.schedule(SendEndTask::make_from_task(this, task_end, sender(), receiver()));
  return true;
}

bool SendEndTask::execute(Timeline &timeline, TaskQueue &tq) const
{
  tq.schedule(MsgTask::make_from_task(this, tq.now(), sender(), receiver()));
  return true;
}

bool IdleTask::execute(Timeline &timeline, TaskQueue &tq) const
{
  auto &cpu = timeline.per_cpu_time[sender()];

  Time cpu_last = cpu.cpu_events.get_last_or_zero();

  // Check if the last cpu task has finished
  if (cpu_last <= tq.now()) {
    assert(tq.has_idle[sender()] == true && "The task had to post idle.");
    tq.has_idle[sender()] = false;
    return true;
  }

  // No, the last CPU task is still running
  tq.schedule(make_task_attime(this, cpu_last));
  return false;
}

bool FinishTask::execute(Timeline &timeline, TaskQueue &tq) const
{
  auto &cpu = timeline.per_cpu_time[sender()];
  assert(!cpu.cpu_events.empty() && "CPU done without ever doing anything");

  auto cpu_last =  cpu.cpu_events.back();

  cpu.finish.push_back(FinishEvent(seq(), cpu_last.end()));

  timeline.update_total_time(cpu.finish.back().end());
  return true;
}

bool TimerTask::execute(Timeline &timeline, TaskQueue &tq) const
{
  return true;
}

bool FailureTask::execute(Timeline &timeline, TaskQueue &tq) const
{
  auto &cpu = timeline.per_cpu_time[receiver()];

  // Calculate time for CpuTime
  cpu.failure.push_back(FailureEvent(seq(), start()));

  timeline.update_total_time(cpu.failure.back().end());
  return true;
}
