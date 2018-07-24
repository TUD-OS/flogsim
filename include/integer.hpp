#pragma once

#include <ostream>

template<typename T>
struct Integer
{

  bool operator<(const T &other) const
  {
    return (static_cast<const T*>(this)->get() < other.get());
  }

  bool operator>(const T &other) const
  {
    return other < *static_cast<const T*>(this);
  }

  bool operator==(const T &other) const
  {
    return !(*static_cast<const T*>(this) < other) &&
      !(other < *static_cast<const T*>(this));
  }

  bool operator!=(const T &other) const
  {
    return !(*static_cast<const T*>(this) == other);
  }

  bool operator<=(const T &other) const
  {
    return !(*static_cast<const T*>(this) > other);
  }

  bool operator>=(const T &other) const
  {
    return !(*static_cast<const T*>(this) < other);
  }

  friend std::ostream& operator<<(std::ostream &os, const Integer& t)
  {
    os << t._get();
    return os;
  }

private:
  auto _get() const { return (*static_cast<const T*>(this)).get(); }
};
