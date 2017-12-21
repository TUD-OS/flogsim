#pragma once

#include <random>

#include "configuration.hpp"

class Entropy
{
  Entropy(unsigned seed);
public:
  const unsigned seed;
  std::default_random_engine generator;

  Entropy();
  Entropy(const Configuration &conf);
};
