#include "broadcast.hpp"
#include "collective.hpp"
#include "topology/topology.hpp"
#include "globals.hpp"

template<typename T>
Broadcast<T>::Broadcast(FaultInjector *faults) :
    Collective(faults, T(Globals::get().model().P, NodeOrder::INTERLEAVED))
{
    reached_nodes[0] = true;
}

template<typename T>
Broadcast<T>::Broadcast(std::initializer_list<int> selected,
          FaultInjector *faults) :
    Collective(selected, faults)
{}
