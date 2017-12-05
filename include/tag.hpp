#pragma once

#include "integer.hpp"

enum class Tag
{
  GENERIC    = 1,
  TREE       = 2,
  GOSSIP     = 3,
  RING_LEFT  = 10,
  RING_RIGHT = 11
};

std::ostream& operator<<(std::ostream &os, const Tag& t);
