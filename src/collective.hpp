#pragma once

#include <memory>

namespace LogP
{

class SendStartTask;
class SendEndTask;
class SendGapEndTask;
class RecvStartTask;
class RecvEndTask;
class RecvGapEndTask;
class MsgTask;
class FinishTask;
class FailureTask;

}

class TaskQueue;

class Collective
{
public:
  virtual void populate(TaskQueue &eq) = 0;

  virtual void accept(const LogP::SendStartTask&, TaskQueue&)
  {
  }

  virtual void accept(const LogP::SendEndTask&, TaskQueue&)
  {
  }

  virtual void accept(const LogP::SendGapEndTask&, TaskQueue&)
  {
  }

  virtual void accept(const LogP::RecvStartTask&, TaskQueue&)
  {
  }

  virtual void accept(const LogP::RecvEndTask&, TaskQueue&)
  {
  }

  virtual void accept(const LogP::RecvGapEndTask&, TaskQueue&)
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
