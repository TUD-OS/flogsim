#include <vector>

#include "counter.hpp"

std::vector<std::shared_ptr<int>> Counter::counters;

std::shared_ptr<int> Counter::allocate_counter()
{
  counters.emplace_back(std::make_unique<int>(0));
  return counters.back();
}

void Counter::reset_counters()
{
  for(auto counter_ptr : counters) {
    (*counter_ptr) = 0;
  }
}

Counter::Counter()
  : _counter(allocate_counter())
{}
