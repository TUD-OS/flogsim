#include <chrono>
#include "entropy.hpp"

Entropy::Entropy(unsigned seed)
  : seed(seed),
    generator(std::default_random_engine(seed))
{}

Entropy::Entropy()
  : Entropy(std::chrono::system_clock::now().time_since_epoch().count())
{}

namespace
{
unsigned create_seed(unsigned seed)
{
  if (seed == 0) {
    return std::chrono::system_clock::now().time_since_epoch().count();
  }

  return seed;
}
}

Entropy::Entropy(const Configuration &conf)
  : Entropy(create_seed(conf.seed))
{}
