#include "task.hpp"
#include "task_queue.hpp"

Sequence Sequence::next()
{
  static int next_id = 0;
  return Sequence(next_id++);
}

bool LogP::RecvTask::execute(Timeline &timeline, TaskQueue &tq) const
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
  CpuEvent cpu_event{seq(), start_time};

  cpu.recv_gaps.push_back(rg);
  cpu.cpu_events.push_back(cpu_event);

  return true;
}

bool LogP::MsgTask::execute(Timeline &timeline, TaskQueue &tq) const
{
  // Calculate time when receive task can be scheduled
  auto recv_time = tq.now() + LogP::Model::get().L;

  tq.schedule(make_from_task<RecvTask>(this, recv_time, sender(), receiver()));
  return true;
}

bool LogP::SendTask::execute(Timeline &timeline, TaskQueue &tq) const
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
  CpuEvent cpu_event{seq(), start_time};

  cpu.send_gaps.push_back(sg);
  cpu.cpu_events.push_back(cpu_event);

  tq.schedule(make_from_task<MsgTask>(this, cpu_event.end(), sender(), receiver()));
  return true;
}

bool LogP::FinishTask::execute(Timeline &timeline, TaskQueue &tq) const
{
  auto &cpu = timeline.per_cpu_time[sender()];

  // Calculate time for CpuTime
  auto cpu_last =  cpu.cpu_events.back();

  cpu.finish.push_back(FinishEvent(seq(), cpu_last.end()));

  timeline.update_total_time(cpu.finish.back().end());
  return true;
}

bool LogP::FailureTask::execute(Timeline &timeline, TaskQueue &tq) const
{
  auto &cpu = timeline.per_cpu_time[receiver()];

  // Calculate time for CpuTime
  cpu.failure.push_back(FailureEvent(seq(), start()));

  timeline.update_total_time(cpu.failure.back().end());
  return true;
}
