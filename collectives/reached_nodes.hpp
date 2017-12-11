#pragma once

#include <vector>

// Wrapper class around vector of reached nodes.  It's purpose to let
// other classes to access the list of the reached nodes, but not
// allow to copy it.
class ReachedNodes
{
  std::vector<bool> reach;
public:

  ReachedNodes(const ReachedNodes&) = delete;

  bool operator[](size_t i) const
  {
    return reach[i];
  }

  std::vector<bool>::reference operator[](size_t i)
  {
    return reach[i];
  }

  auto size() const
  {
    return reach.size();
  }

  void assign(decltype(reach)::size_type count, const bool &value)
  {
    reach.assign(count, value);
  }

  ReachedNodes(const int P)
    : reach(P)
  {}

  auto begin() { return reach.begin(); }
  auto begin() const { return reach.begin(); }

  auto end() { return reach.end(); }
  auto end() const { return reach.end(); }
};
