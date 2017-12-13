#pragma once

#include <memory>
#include <functional>
#include <map>
#include <string>
#include <type_traits>

#include "reached_nodes.hpp"
#include "timeline.hpp"

class SendStartTask;
class SendEndTask;
class RecvStartTask;
class RecvEndTask;
class MsgTask;
class IdleTask;
class FinishTask;
class TimerTask;
class InitTask;
class FailureTask;

class TaskQueue;
class Phase;
class FaultInjector;

/* Act as a collective and forward relevant Tasks to individual nodes */
//
// This class is responsible to set up reach_nodes. It either accepts
// explicit list of reached nodes as an initializer list (for example
// only root: {0}) or it enables everybody.
class Collective
{
private:
  void forward(const auto &t, TaskQueue &tq, const int node_id);

  std::unique_ptr<Phase> phase;
public:
  ReachedNodes reached_nodes;
  std::unique_ptr<FaultInjector> faults;

  virtual void accept(const InitTask &t, TaskQueue &tq);
  virtual void accept(const TimerTask &t, TaskQueue &tq);
  virtual void accept(const IdleTask &t, TaskQueue &tq);

  virtual void accept(const SendStartTask &t, TaskQueue &tq);
  virtual void accept(const SendEndTask &t, TaskQueue &tq);

  virtual void accept(const RecvStartTask &t, TaskQueue &tq);
  virtual void accept(const RecvEndTask &t, TaskQueue &tq);

  virtual void accept(const MsgTask &t, TaskQueue &tq);
  virtual void accept(const FinishTask &t, TaskQueue &tq);
  virtual void accept(const FailureTask &t, TaskQueue &tq);

  Timeline run(std::unique_ptr<Phase> &&);

  Collective();
  Collective(std::initializer_list<int> selected);
  // Factory method, which creates collectives based on
  // configuration.
  static std::unique_ptr<Collective> create();
};

class CollectiveRegistry
{
public:
  typedef std::function<std::unique_ptr<Collective>()> create_fun_t;

  static void declare(create_fun_t &create_fun, const std::string_view &name);
  static std::unique_ptr<Collective> create();

private:
  std::map<std::string_view, create_fun_t> data;
  static CollectiveRegistry &get();
};

template<typename T>
class CollectiveRegistrator
{
public:

  static std::unique_ptr<T> create()
  {
    return std::make_unique<T>();
  }

  CollectiveRegistrator()
  {
    auto create_fun = static_cast<CollectiveRegistry::create_fun_t>(&create);
    CollectiveRegistry::declare(create_fun, T::name);
  }
};
