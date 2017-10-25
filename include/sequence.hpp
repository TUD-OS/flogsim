#pragma once

#include "integer.hpp"

// class for sequence numbers
struct Sequence : public Integer<Sequence>
{
  int id;

  int get() const { return id; }

  explicit Sequence(int id)
    : id(id)
  {}
  Sequence(const Sequence &other) = default;

  static Sequence next();
};
