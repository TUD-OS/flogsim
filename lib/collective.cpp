#include <cmath>
#include <memory>

#include "collective.hpp"
#include "configuration.hpp"

CollectiveRegistry &CollectiveRegistry::get()
{
  static CollectiveRegistry reg;
  return reg;
}

void CollectiveRegistry::declare(CollectiveRegistry::create_fun_t &create_fun,
                                  const std::string &name)
{
  get().data[name] = create_fun;
}

std::unique_ptr<Collective> CollectiveRegistry::create(const Configuration& conf)
{
  const std::string &name = conf.collective;
  try {
    create_fun_t &create_fun = get().data.at(name);
    return create_fun(conf);
  } catch(const std::out_of_range &e) {
    throw std::invalid_argument("Collective does not exist:" +
                                name);
  }

}
