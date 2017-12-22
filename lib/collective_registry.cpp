#include <map>

#include "globals.hpp"
#include "collective_registry.hpp"
#include "phase.hpp"

CollectiveRegistry &CollectiveRegistry::get()
{
  static CollectiveRegistry reg;
  return reg;
}

void CollectiveRegistry::declare(CollectiveRegistry::create_fun_t &create_fun,
                                 const std::string_view &name)
{
  get().data[name] = create_fun;
}

std::unique_ptr<Phase> CollectiveRegistry::create(ReachedNodes &reached_nodes)
{
  const std::string &name = Globals::get().conf().collective;
  try {
    create_fun_t &create_fun = get().data.at(name);
    return create_fun(reached_nodes);
  } catch(const std::out_of_range &e) {
    throw std::invalid_argument("Collective does not exist:" +
                                name);
  }

}

std::vector<std::string_view> CollectiveRegistry::list()
{
  std::vector<std::string_view> names;
  for(auto const &coll: get().data) {
    names.push_back(coll.first);
  }
  return names;
}
