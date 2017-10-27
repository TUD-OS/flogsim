#pragma once

#include <cstdio>
#include <cstdint>
#include <cinttypes>

#include <ostream>

#include "integer.hpp"

struct Time : public Integer<Time>
{
  Time() :
    time(0)
  {}

  explicit Time(auto time)
    : time(time)
  {
  }

  Time operator*(int a) const
  {
    return Time(a * time);
  }
  Time operator+(const Time &other) const
  {
    return Time(time + other.time);
  }
  Time operator-(const Time &other) const
  {
    return Time(time - other.time);
  }

  static Time max()
  {
    return Time(UINT64_MAX - 1);
  }

  auto get() const { return time; }

private:
  uint64_t time;
};
