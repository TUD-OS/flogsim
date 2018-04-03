#pragma once

#include "integer.hpp"

enum class Tag
{
  INIT       = 1,
  TREE       = 2,
  GOSSIP     = 3,
  GATHER     = 4,
  RING_LEFT  = 10,
  RING_RIGHT = 11,
  INTERNAL   = 20,
  EXCLUSIVE  = 50,
};

std::ostream& operator<<(std::ostream &os, const Tag& t);
