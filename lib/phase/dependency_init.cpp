#include "phase/dependency_init.hpp"

#include "topology/kary.hpp"
#include "topology/binomial.hpp"
#include "topology/lame.hpp"

using Result = Phase::Result;

Result BcastDepInit::dispatch(const InitTask &, TaskQueue &, int node_id)
{
  return Result::DONE_PHASE;
}

template<typename T>
Result ReductionDepInit<T>::dispatch(const InitTask &, TaskQueue &, int node_id)
{
  return Result::DONE_PHASE;
}

// explicit instantiation
template class ReductionDepInit<Lame>;
template class ReductionDepInit<KAry>;
template class ReductionDepInit<Binomial>;
