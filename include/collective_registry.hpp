#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <string>

class Phase;
class ReachedNodes;

class CollectiveRegistry
{
public:
  typedef std::function<std::unique_ptr<Phase>(ReachedNodes&)> create_fun_t;

  static void declare(create_fun_t &create_fun, const std::string_view &name);
  static std::unique_ptr<Phase> create(ReachedNodes &reached_nodes);
  static std::vector<std::string_view> list();

private:
  std::map<std::string_view, create_fun_t> data;
  static CollectiveRegistry &get();
};

class CollectiveRegistrator
{
  using create_fun_t = CollectiveRegistry::create_fun_t;
public:

  CollectiveRegistrator(create_fun_t create_fun, const char name[])
  {
    CollectiveRegistry::declare(create_fun, name);
  }
};
