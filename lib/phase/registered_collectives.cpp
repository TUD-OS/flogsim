#include <memory>

#include "collective.hpp"
#include "collective_registry.hpp"
#include "phase.hpp"

#include "phase/gossip.hpp"
#include "phase/kary_tree.hpp"
#include "phase/binomial_tree.hpp"
#include "phase/optimal_tree.hpp"

#include "phase/exclusive.hpp"
#include "phase/combiner.hpp"

#include "phase/correction.hpp"

namespace
{
std::vector<CollectiveRegistrator> _{
  {
    {
      [](ReachedNodes &rn)
      {
        return std::make_unique<KAryTree<true>>(rn);
      },
      "kary_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        return std::make_unique<BinomialTree>(rn);
      },
      "binomial_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        return std::make_unique<OptimalTree>(rn);
      },
      "optimal_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        return std::make_unique<Gossip>(rn);
      },
      "gossip_bcast"
    },

    {
      [](ReachedNodes &rn)
      {
        auto phases = Combiner::Phases(rn).
          add_phase<Exclusive>(std::make_unique<BinomialTree>(rn)).
          add_phase<OpportunisticCorrection<false>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "phased_corrected_binomial_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto phases = Combiner::Phases(rn).
          add_phase<BinomialTree>().
          add_phase<OpportunisticCorrection<false>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "corrected_binomial_bcast"
    },

    {
      [](ReachedNodes &rn)
      {
        auto phases = Combiner::Phases(rn).
          add_phase<Exclusive>(std::make_unique<BinomialTree>(rn)).
          add_phase<CheckedCorrection<true>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "phased_checked_corrected_binomial_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto phases = Combiner::Phases(rn).
          add_phase<BinomialTree>().
          add_phase<CheckedCorrection<false>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "checked_corrected_binomial_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto phases = Combiner::Phases(rn).
          add_phase<OptimalTree>().
          add_phase<CheckedCorrection<false>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "checked_corrected_optimal_bcast"
    },

    {
      [](ReachedNodes &rn)
      {
        auto phases = Combiner::Phases(rn).
          add_phase<Gossip>().
          add_phase<OpportunisticCorrection<true>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "opportunistic_corrected_gossip_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto phases = Combiner::Phases(rn).
          add_phase<Gossip>().
          add_phase<CheckedCorrection<true>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "checked_corrected_gossip_bcast"
    },
  }
};
}
