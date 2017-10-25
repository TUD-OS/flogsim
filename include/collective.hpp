#pragma once

#include <memory>
#include <functional>
#include <map>
#include <string>

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

class CollectiveRegistry
{
public:
  typedef std::function<std::unique_ptr<Collective>()> create_fun_t;

  static void declare(create_fun_t &create_fun, const std::string &name);
  static std::unique_ptr<Collective> create(const std::string &name);

private:
  std::map<std::string, create_fun_t> data;
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

  CollectiveRegistrator(const std::string &name)
  {
    auto create_fun = static_cast<CollectiveRegistry::create_fun_t>(&create);
    CollectiveRegistry::declare(create_fun, name);
  }
};
