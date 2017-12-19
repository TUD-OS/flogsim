#include "task.hpp"
#include "task_queue.hpp"
#include "globals.hpp"

bool RecvStartTask::execute(Timeline &timeline, TaskQueue &tq) const
{
  auto &cpu = timeline.per_cpu_time[receiver()];

  // Calculate time for CpuTime
  Time cpu_last = cpu.cpu_events.earliest_start_time();
  Time recv_last = cpu.recv_gaps.earliest_start_time();

  auto start_time = std::max({cpu_last, recv_last, tq.now()});

  if (start_time > tq.now()) {
    tq.schedule(make_task_attime(this, start_time));
    return false;
  }

  RecvGap rg{seq(), start_time};
  CpuEvent cpu_event{seq(), start_time, tag()};

  cpu.recv_gaps.append(rg);
  cpu.cpu_events.append(cpu_event);

  tq.mark_nonidle(receiver());

  Time task_end = std::max(cpu_event.end(), rg.end());
  tq.schedule(RecvEndTask::make_from_task(this, task_end, sender(), receiver()));
  return true;
}

bool RecvEndTask::execute(Timeline&, TaskQueue&) const
{
  return true;
}

bool MsgTask::execute(Timeline&, TaskQueue &tq) const
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
  Time cpu_last = cpu.cpu_events.earliest_start_time();
  Time send_last = cpu.send_gaps.earliest_start_time();

  auto start_time = std::max({cpu_last, send_last, tq.now()});

  if (start_time > tq.now()) {
    tq.schedule(make_task_attime(this, start_time));
    return false;
  }

  SendGap sg{seq(), start_time};
  CpuEvent cpu_event{seq(), start_time, tag()};

  cpu.send_gaps.append(sg);
  cpu.cpu_events.append(cpu_event);

  tq.mark_nonidle(sender());

  Time task_end = std::max(cpu_event.end(), sg.end());
  tq.schedule(SendEndTask::make_from_task(this, task_end, sender(), receiver()));
  return true;
}

bool SendEndTask::execute(Timeline&, TaskQueue &tq) const
{
  tq.schedule(MsgTask::make_from_task(this, tq.now(), sender(), receiver()));
  return true;
}

bool IdleTask::execute(Timeline&, TaskQueue&) const
{
  return true;
}

bool FinishTask::execute(Timeline &timeline, TaskQueue&) const
{
  auto &cpu = timeline.per_cpu_time[sender()];
  if (cpu.cpu_events.empty())
    throw std::runtime_error("CPU done without ever doing anything");

  cpu.finish.append(FinishEvent(seq(), cpu.cpu_events.end()));

  timeline.update_total_time(cpu.finish.end());
  return true;
}

bool TimerTask::execute(Timeline&, TaskQueue&) const
{
  return true;
}

bool InitTask::execute(Timeline&, TaskQueue&) const
{
  return true;
}

bool FailureTask::execute(Timeline &timeline, TaskQueue&) const
{
  auto &cpu = timeline.per_cpu_time[receiver()];

  // Calculate time for CpuTime
  cpu.failure.append(FailureEvent(seq(), start()));

  timeline.update_total_time(cpu.failure.end());
  return true;
}
