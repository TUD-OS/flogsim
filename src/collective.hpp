#pragma once

#include <memory>

class SendStartTask;
class SendEndTask;
class SendGapEndTask;
class RecvStartTask;
class RecvEndTask;
class RecvGapEndTask;
class MsgTask;
class FinishTask;
class FailureTask;

class TaskQueue;

class Collective
{
public:
  virtual void populate(TaskQueue &eq) = 0;

  virtual void accept(const SendStartTask&, TaskQueue&)
  {
  }

  virtual void accept(const SendEndTask&, TaskQueue&)
  {
  }

  virtual void accept(const SendGapEndTask&, TaskQueue&)
  {
  }

  virtual void accept(const RecvStartTask&, TaskQueue&)
  {
  }

  virtual void accept(const RecvEndTask&, TaskQueue&)
  {
  }

  virtual void accept(const RecvGapEndTask&, TaskQueue&)
  {
  }

  virtual void accept(const MsgTask&, TaskQueue&)
  {
  }

  virtual void accept(const FinishTask&, TaskQueue&)
  {
  }

  virtual void accept(const FailureTask&, TaskQueue&)
  {
  }

  // Factory method, which creates collectives based on
  // configuration.
  static std::unique_ptr<Collective> create();
};
