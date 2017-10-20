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

class UniformFaults : public FaultInjector
{
  int P; // Number of nodes
  int F; // Number of offline failures
  std::vector<int> failed_nodes;
public:

  UniformFaults(int P, int F);

  bool failure(std::shared_ptr<Task>) override final;
};
