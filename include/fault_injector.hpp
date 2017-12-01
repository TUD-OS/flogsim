#pragma once

#include <memory>
#include <vector>
#include <iostream>

#include "model.hpp"

class Task;

enum class Fault
{
  OK,
  SKIP,
  FAILURE
};

class FaultInjector
{
protected:
  virtual void print(std::ostream &os) const = 0;

public:
  FaultInjector()
  {}

  virtual Fault failure(Task* task) = 0;
  virtual int fault_count() { return 0; }

  // Factory method, which creates fault injector based on
  // configuration.
  static std::unique_ptr<FaultInjector> create();

  friend std::ostream &operator<<(std::ostream &os, const FaultInjector &fi)
  {
    fi.print(os);
    return os;
  }
};

class NoFaults : public FaultInjector
{
  virtual void print(std::ostream &) const override final
  {}

public:
  using FaultInjector::FaultInjector;

  Fault failure(Task *) override final
  {
    return Fault::OK;
  }

  static bool match(const std::string &fault_injector)
  {
    return fault_injector == "none";
  }
};

class ListFaults : public FaultInjector
{
protected:
  int P; // Number of nodes
  int F; // Number of offline failures
  std::vector<int> failed_nodes;

  void print(std::ostream &os) const override;
public:
  ListFaults();
  // Class to set up deterministic faults for testing
  ListFaults(const std::vector<int> &);

  Fault failure(Task *task) override final;
  virtual int fault_count() { return F; }
};

class ExplicitListFaults : public ListFaults
{
public:
  ExplicitListFaults();

  static bool match(const std::string &fault_injector);
};

class UniformFaults : public ListFaults
{
public:
  UniformFaults();

  static bool match(const std::string &fault_injector)
  {
    return fault_injector == "uniform";
  }
};

