#pragma once

#include "collective.hpp"

class FaultInjector;

template<typename TOPOLOGY>
class Reduction : public Collective
{
  public:
    Reduction(FaultInjector *faults);
    Reduction(std::initializer_list<int> selected,
              FaultInjector *faults);
};
