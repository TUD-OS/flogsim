#pragma once

#include <memory>
#include <functional>
#include <map>
#include <string>

class SendStartTask;
class SendEndTask;
class RecvStartTask;
class RecvEndTask;
class MsgTask;
class FinishTask;
class TimerTask;
class FailureTask;

class TaskQueue;
class Configuration;

class Collective
{
protected:
  const Configuration &conf;
public:
  Collective(const Configuration &conf)
    : conf(conf)
  {}

  virtual void populate(TaskQueue &eq) = 0;

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

  virtual void accept(const FinishTask&, TaskQueue&)
  {
  }

  virtual void accept(const TimerTask&, TaskQueue&)
  {
  }

  virtual void accept(const FailureTask&, TaskQueue&)
  {
  }

  // Factory method, which creates collectives based on
  // configuration.
  static std::unique_ptr<Collective> create(const Configuration &);
};

class CollectiveRegistry
{
public:
  typedef std::function<std::unique_ptr<Collective>(const Configuration &)> create_fun_t;

  static void declare(create_fun_t &create_fun, const std::string &name);
  static std::unique_ptr<Collective> create(const Configuration&);

private:
  std::map<std::string, create_fun_t> data;
  static CollectiveRegistry &get();
};

template<typename T>
class CollectiveRegistrator
{
public:

  static std::unique_ptr<T> create(const Configuration &conf)
  {
    return std::make_unique<T>(conf);
  }

  CollectiveRegistrator(const std::string &name)
  {
    auto create_fun = static_cast<CollectiveRegistry::create_fun_t>(&create);
    CollectiveRegistry::declare(create_fun, name);
  }
};
