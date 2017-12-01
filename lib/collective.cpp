#include <cmath>
#include <memory>

#include "collective.hpp"
#include "configuration.hpp"
#include "globals.hpp"

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

std::unique_ptr<Collective> CollectiveRegistry::create()
{
  const std::string &name = Globals::get().conf().collective;
  try {
    create_fun_t &create_fun = get().data.at(name);
    return create_fun();
  } catch(const std::out_of_range &e) {
    throw std::invalid_argument("Collective does not exist:" +
                                name);
  }

}
