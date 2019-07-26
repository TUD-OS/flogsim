#include "reduction.hpp"
#include "collective.hpp"

template<typename T>
Reduction<T>::Reduction(FaultInjector *faults) :
    Collective(faults, T(Globals::get().model().P, NodeOrder::INTERLEAVED))
{
  for (auto node : topology.get_leaves()) {
    reached_nodes[node.get()] = true;
  }
}

template<typename T>
Reduction<T>::Reduction(std::initializer_list<int> selected,
          FaultInjector *faults) :
    Collective(selected, faults)
{}
