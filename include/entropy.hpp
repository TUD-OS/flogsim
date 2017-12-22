#pragma once

#include <random>

#include "configuration.hpp"

class Entropy
{
  unsigned seed;
  Entropy(unsigned seed);
public:
  void reset_seed(unsigned seed = 0);
  unsigned get_seed() const { return seed; };
  std::default_random_engine generator;

  Entropy();
  Entropy(const Configuration &conf);
};
