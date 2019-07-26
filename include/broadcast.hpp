#pragma once

#include "collective.hpp"

class FaultInjector;

template<typename TOPOLOGY>
class Broadcast : public Collective
{
  public:
    Broadcast(FaultInjector *faults);
    Broadcast(std::initializer_list<int> selected,
              FaultInjector *faults);
};
