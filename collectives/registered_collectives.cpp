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
        CombinerPhase::PhaseVec phases;

        phases.push_back(
          std::make_unique<ExclusivePhase>
          (rn, std::make_unique<BinomialTreePhase>(rn)));
        phases.push_back(
          std::make_unique<OpportunisticCorrectionPhase<false>>(rn));
        return std::make_unique<CombinerPhase>(rn, std::move(phases));
      },
      "phased_corrected_binomial_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        CombinerPhase::PhaseVec phases;

        phases.push_back(
          std::make_unique<BinomialTreePhase>(rn));
        phases.push_back(
          std::make_unique<OpportunisticCorrectionPhase<false>>(rn));
        return std::make_unique<CombinerPhase>(rn, std::move(phases));
      },
      "corrected_binomial_bcast"
    },

    {
      [](ReachedNodes &rn)
      {
        CombinerPhase::PhaseVec phases;

        phases.push_back(
          std::make_unique<ExclusivePhase>
          (rn, std::make_unique<BinomialTreePhase>(rn)));
        phases.push_back(
          std::make_unique<CheckedCorrectionPhase<true>>(rn));
        return std::make_unique<CombinerPhase>(rn, std::move(phases));
      },
      "phased_checked_corrected_binomial_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        CombinerPhase::PhaseVec phases;

        phases.push_back(
          std::make_unique<BinomialTreePhase>(rn));
        phases.push_back(
          std::make_unique<CheckedCorrectionPhase<false>>(rn));
        return std::make_unique<CombinerPhase>(rn, std::move(phases));
      },
      "checked_corrected_binomial_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        CombinerPhase::PhaseVec phases;

        phases.push_back(
          std::make_unique<OptimalTreePhase>(rn));
        phases.push_back(
          std::make_unique<CheckedCorrectionPhase<false>>(rn));
        return std::make_unique<CombinerPhase>(rn, std::move(phases));
      },
      "checked_corrected_optimal_bcast"
    },

    {
      [](ReachedNodes &rn)
      {
        CombinerPhase::PhaseVec phases;

        phases.push_back(
            std::make_unique<GossipPhase>(rn));
        phases.push_back(
          std::make_unique<OpportunisticCorrectionPhase<true>>(rn));
        return std::make_unique<CombinerPhase>(rn, std::move(phases));
      },
      "opportunistic_corrected_gossip_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        CombinerPhase::PhaseVec phases;

        phases.push_back(
          std::make_unique<GossipPhase>(rn));
        phases.push_back(
          std::make_unique<CheckedCorrectionPhase<true>>(rn));
        return std::make_unique<CombinerPhase>(rn, std::move(phases));
      },
      "checked_corrected_gossip_bcast"
    },
  }
};
}
