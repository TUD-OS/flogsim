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

  friend Time operator*(const int a, const Time &other)
  {
    return Time(a * other.time);
  }

  friend Time operator-(const int a, const Time &other)
  {
    return Time(a - other.time);
  }

  friend Time operator+(const int a, const Time &other)
  {
    return Time(a + other.time);
  }

  Time operator*(const Time &other) const
  {
    return Time(time * other.time);
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
    return Time(INT64_MAX - 1);
  }

  auto get() const { return time; }

  friend std::ostream &operator<<(std::ostream &os, const Time &t)
  {
    if (t == Time::max()) {
      os << "MAX";
    } else {
      os << t.get();
    }

    return os;
  }

private:
  int64_t time;
};
