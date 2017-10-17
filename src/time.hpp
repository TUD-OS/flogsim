#pragma once

#include <cstdio>
#include <cstdint>
#include <cinttypes>

struct Time
{
  Time() :
    time(0)
  {}

  explicit Time(auto time)
    : time(time)
  {
  }

  Time operator+(const Time &other) const
  {
    return Time(time + other.time);
  }
  Time operator-(const Time &other) const
  {
    return Time(time - other.time);
  }

  bool operator<(const Time &other) const
  {
    return (time < other.time);
  }

  bool operator==(const Time &other) const
  {
    return !(*this < other) && !(other < *this);
  }

  bool operator!=(const Time &other) const
  {
    return !(*this == other);
  }

  bool operator>(const Time &other) const
  {
    return !(*this < other) && (other != *this);
  }

  void dump() const
  {
    std::printf("%" PRIu64 "", time);
  }

  static Time max()
  {
    return Time(UINT64_MAX);
  }
private:
  uint64_t time;
};
