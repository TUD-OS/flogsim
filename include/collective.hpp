#pragma once

#include <memory>
#include <functional>
#include <map>
#include <string>
#include <type_traits>
#include <assert.h>

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

class Collective
{
public:
  Collective()
  {}

  virtual void accept(const SendStartTask&, TaskQueue&)
  {
  }

  virtual void accept(const SendEndTask&, TaskQueue&)
  {
  }

  virtual void accept(const RecvStartTask&, TaskQueue&)
  {
  }

  virtual void accept(const RecvEndTask&, TaskQueue&)
  {
  }

  virtual void accept(const MsgTask&, TaskQueue&)
  {
  }

  virtual void accept(const IdleTask&, TaskQueue&)
  {
  }

  virtual void accept(const FinishTask&, TaskQueue&)
  {
  }

  virtual void accept(const TimerTask&, TaskQueue&)
  {
  }

  virtual void accept(const InitTask&, TaskQueue&)
  {
  }

  virtual void accept(const FailureTask&, TaskQueue&)
  {
  }

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
