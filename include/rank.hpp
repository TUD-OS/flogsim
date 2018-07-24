#pragma once

#include <cstdio>
#include <cstdint>
#include <cinttypes>

#include <ostream>

#include "integer.hpp"

struct Rank : public Integer<Rank>
{
  Rank() :
    rank(0)
  {}

  explicit Rank(auto rank)
    : rank(rank)
  {
  }

  friend Rank operator-(const int a, const Rank &other)
  {
    return Rank(a - other.rank);
  }

  friend Rank operator+(const int a, const Rank &other)
  {
    return Rank(a + other.rank);
  }

  Rank operator+(const Rank &other) const
  {
    return Rank(rank + other.rank);
  }

  Rank operator-(const Rank &other) const
  {
    return Rank(rank - other.rank);
  }

  auto get() const { return rank; }

private:
  int rank;
};
