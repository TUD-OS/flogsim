#pragma once

#include <memory>

namespace LogP
{

class SendTask;
class RecvTask;
class MsgTask;
class FinishTask;
class FailureTask;

}

class TaskQueue;

class Collective
{
public:
  virtual void populate(TaskQueue &eq) = 0;

  virtual void accept(const LogP::SendTask&, TaskQueue&)
  {
  }

  virtual void accept(const LogP::RecvTask&, TaskQueue&)
  {
  }

  virtual void accept(const LogP::MsgTask&, TaskQueue&)
  {
  }

  virtual void accept(const LogP::FinishTask&, TaskQueue&)
  {
  }

  virtual void accept(const LogP::FailureTask&, TaskQueue&)
  {
  }

  // Factory method, which creates collectives based on
  // configuration.
  static std::unique_ptr<Collective> create();
};
