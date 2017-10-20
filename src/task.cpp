#include "task.hpp"

bool LogP::RecvTask::execute(Timeline &timeline, TaskQueue &tq) const
{
  auto &cpu = timeline.per_cpu_time[receiver()];

  // Calculate time for CpuTime
  Time cpu_last = cpu.cpu_events.get_last_or_zero();
  Time recv_last = cpu.recv_gaps.get_last_or_zero();

  auto start_time = std::max({cpu_last, recv_last, tq.now()});

  if (start_time > tq.now()) {
    tq.schedule(std::make_shared<RecvTask>(start_time, sender(), receiver()));
    return false;
  }

  RecvGap rg{start_time, sender()};
  CpuEvent cpu_event{start_time};

  cpu.recv_gaps.push_back(rg);
  cpu.cpu_events.push_back(cpu_event);

  return true;
}

bool LogP::MsgTask::execute(Timeline &timeline, TaskQueue &tq) const
{
  // Calculate time when receive task can be scheduled
  auto recv_time = tq.now() + LogP::Model::get().L;

  tq.schedule(std::make_shared<RecvTask>(recv_time, sender(), receiver()));
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
    tq.schedule(std::make_shared<SendTask>(start_time, sender(), receiver()));
    return false;
  }

  SendGap sg{start_time};
  CpuEvent cpu_event{start_time};

  cpu.send_gaps.push_back(sg);
  cpu.cpu_events.push_back(cpu_event);

  tq.schedule(std::make_shared<MsgTask>(cpu_event.end(), sender(), receiver()));
  return true;
}

bool LogP::FinishTask::execute(Timeline &timeline, TaskQueue &tq) const
{
  auto &cpu = timeline.per_cpu_time[sender()];

  // Calculate time for CpuTime
  auto cpu_last =  cpu.cpu_events.back();

  cpu.finish.push_back(FinishEvent(cpu_last.end()));

  timeline.update_total_time(cpu.finish.back().end());
  return true;
}

bool LogP::FailureTask::execute(Timeline &timeline, TaskQueue &tq) const
{
  auto &cpu = timeline.per_cpu_time[receiver()];

  // Calculate time for CpuTime
  cpu.failure.push_back(FailureEvent(start(), sender()));

  timeline.update_total_time(cpu.failure.back().end());
  return true;
}
