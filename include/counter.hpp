#pragma once

#include <memory>

// This is a special class which allows to count whatever you want,
// but it keeps the data in a shared data structure, so that you can
// reset all counters at once all together, without a risk that you
// forget some counter.
//
// This class is used, in particular, to count task creations and
// reschedulings per task type.
class Counter
{
  std::shared_ptr<int> _counter;

  static std::vector<std::shared_ptr<int>> counters;
  static std::shared_ptr<int> allocate_counter();
public:
  // Reset all existing counters
  static void reset_counters();

  Counter();

  void inc() { (*_counter)++; }
  int get() const { return *_counter; }
};
