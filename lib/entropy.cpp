#include <chrono>
#include "entropy.hpp"


void Entropy::reset_seed(unsigned seed)
{
  if (seed == 0) {
    this->seed = std::chrono::system_clock::now().time_since_epoch().count();
  } else {
    this->seed = seed;
  }

  generator.seed(this->seed);
}

Entropy::Entropy(unsigned seed)
{
  reset_seed(seed);
}

Entropy::Entropy()
  : Entropy(0)
{}

Entropy::Entropy(const Configuration &conf)
  : Entropy(conf.seed)
{}
