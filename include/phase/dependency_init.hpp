#pragma once

#include "phase.hpp"
#include "topology/topology.hpp"
#include "topology/kary.hpp"
#include "topology/binomial.hpp"
#include "topology/lame.hpp"
#include "topology/optimal.hpp"

// These classes are responsible for initializing dependencies
// that are specific to each type of operation (Bcast or Reduction).

class BcastDepInit : public Phase
{
    public:
        BcastDepInit(ReachedNodes &reached_nodes) :
            Phase(reached_nodes)
        {
            reached_nodes[0] = true;
        }

        Result dispatch(const InitTask&, TaskQueue&, int node_id) override final;
};

template<typename TOPOLOGY>
class ReductionDepInit : public Phase
{
    protected:
        const TOPOLOGY topology;
    public:
        ReductionDepInit(ReachedNodes &reached_nodes, NodeOrder order = NodeOrder::INTERLEAVED) :
            Phase(reached_nodes),
            topology(reached_nodes.size(), order)
        {
            for (Rank node : topology.leaves()) {
                reached_nodes[node.get()] = true;
            }
        }

        Result dispatch(const InitTask&, TaskQueue&, int node_id) override final;
};
