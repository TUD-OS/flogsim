#pragma once

// class for sequence numbers
struct Sequence
{
  int id;

  explicit Sequence(int id)
    : id(id)
  {}
  Sequence(const Sequence &other) = default;

  static Sequence next();
};
