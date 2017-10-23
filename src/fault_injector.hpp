#pragma once

#include "task.hpp"
#include "configuration.hpp"

class Task;

class FaultInjector
{
public:
  virtual bool failure(std::shared_ptr<Task> task) = 0;

  // Factory method, which creates fault injector based on
  // configuration.
  static std::unique_ptr<FaultInjector> create();
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

  UniformFaults(int F);

  bool failure(std::shared_ptr<Task>) override final;
};
