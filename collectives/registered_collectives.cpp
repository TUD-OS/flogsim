#include <memory>

#include "collective.hpp"
#include "phase.hpp"

#include "gossip_phase.hpp"
#include "kary_tree.hpp"
#include "binomial_tree.hpp"
#include "optimal_tree.hpp"

#include "exclusive_phase.hpp"
#include "combiner_phase.hpp"

#include "correction_phase.hpp"

namespace
{
std::vector<CollectiveRegistrator> _{
  {
    {
      [](ReachedNodes &rn)
      {
        return std::make_unique<KAryTreePhase<true>>(rn);
      },
      "kary_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        return std::make_unique<BinomialTreePhase>(rn);
      },
      "binomial_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        return std::make_unique<OptimalTreePhase>(rn);
      },
      "optimal_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        return std::make_unique<GossipPhase>(rn);
      },
      "gossip_bcast"
    },

    {
      [](ReachedNodes &rn)
      {
        auto phases = CombinerPhase::Phases(rn).
          add_phase<ExclusivePhase>(std::make_unique<BinomialTreePhase>(rn)).
          add_phase<OpportunisticCorrectionPhase<false>>();

        return std::make_unique<CombinerPhase>(std::move(phases));
      },
      "phased_corrected_binomial_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto phases = CombinerPhase::Phases(rn).
          add_phase<BinomialTreePhase>().
          add_phase<OpportunisticCorrectionPhase<false>>();

        return std::make_unique<CombinerPhase>(std::move(phases));
      },
      "corrected_binomial_bcast"
    },

    {
      [](ReachedNodes &rn)
      {
        auto phases = CombinerPhase::Phases(rn).
          add_phase<ExclusivePhase>(std::make_unique<BinomialTreePhase>(rn)).
          add_phase<CheckedCorrectionPhase<true>>();

        return std::make_unique<CombinerPhase>(std::move(phases));
      },
      "phased_checked_corrected_binomial_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto phases = CombinerPhase::Phases(rn).
          add_phase<BinomialTreePhase>().
          add_phase<CheckedCorrectionPhase<false>>();

        return std::make_unique<CombinerPhase>(std::move(phases));
      },
      "checked_corrected_binomial_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto phases = CombinerPhase::Phases(rn).
          add_phase<OptimalTreePhase>().
          add_phase<CheckedCorrectionPhase<false>>();

        return std::make_unique<CombinerPhase>(std::move(phases));
      },
      "checked_corrected_optimal_bcast"
    },

    {
      [](ReachedNodes &rn)
      {
        auto phases = CombinerPhase::Phases(rn).
          add_phase<GossipPhase>().
          add_phase<OpportunisticCorrectionPhase<true>>();

        return std::make_unique<CombinerPhase>(std::move(phases));
      },
      "opportunistic_corrected_gossip_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto phases = CombinerPhase::Phases(rn).
          add_phase<GossipPhase>().
          add_phase<CheckedCorrectionPhase<true>>();

        return std::make_unique<CombinerPhase>(std::move(phases));
      },
      "checked_corrected_gossip_bcast"
    },
  }
};
}
