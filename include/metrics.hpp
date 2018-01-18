#pragma once

#include <map>
#include <string>

#include "configuration.hpp"

class Globals;

// Metrics is a class which allows collectives to store some
// information about their execution.
class Metrics
{
  using value_t = long int;
  using metrics_map = std::map<std::string, value_t>;
  metrics_map map;

  static Metrics &get();

  friend Globals;
public:
  value_t &operator[](const std::string &name);

  auto cbegin() const
  {
    return map.cbegin();
  }

  auto cend() const
  {
    return map.cend();
  }
};
