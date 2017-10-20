#pragma once

#include "task.hpp"

class Task;

class FaultInjector
{
public:
  virtual bool failure(std::shared_ptr<Task> task) = 0;
};

class NoFaults : public FaultInjector
{
public:
  bool failure(std::shared_ptr<Task>) override final
  {
    return false;
  }
};
