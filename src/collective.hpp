#pragma once

#include "task_queue.hpp"

namespace LogP
{

class SendTask;
class RecvTask;
class MsgTask;
class FinishTask;

}

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
};
