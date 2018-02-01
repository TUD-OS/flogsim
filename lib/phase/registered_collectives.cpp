#include <memory>

#include "collective.hpp"
#include "collective_registry.hpp"
#include "phase.hpp"

#include "phase/gossip.hpp"
#include "phase/kary_tree.hpp"
#include "phase/binomial_tree.hpp"
#include "phase/lame_tree.hpp"
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
        return std::make_unique<KAryTree>(rn);
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
        return std::make_unique<LameTree>(rn);
      },
      "lame_bcast"
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
          add_phase<OpportunisticCorrection<false, false>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "phased_opportunistic_corrected_binomial_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto phases = Combiner::Phases(rn).
          add_phase<Exclusive>(std::make_unique<BinomialTree>(rn)).
          add_phase<OpportunisticCorrection<false, true>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "phased_optimised_corrected_binomial_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto phases = Combiner::Phases(rn).
          add_phase<BinomialTree>().
          add_phase<OpportunisticCorrection<false, false>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "corrected_binomial_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto phases = Combiner::Phases(rn).
          add_phase<BinomialTree>().
          add_phase<OpportunisticCorrection<false, true>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "optimised_corrected_binomial_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto phases = Combiner::Phases(rn).
          add_phase<Exclusive>(std::make_unique<KAryTree>(rn)).
          add_phase<CheckedCorrection<true>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "phased_checked_corrected_kary_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto phases = Combiner::Phases(rn).
          add_phase<KAryTree>().
          add_phase<CheckedCorrection<false>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "checked_corrected_kary_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto tree = std::make_unique<KAryTree>(rn);
        tree->forward_unexpected_message();

        auto phases = Combiner::Phases(rn).
          add_phase(std::move(tree)).
          add_phase<CheckedCorrection<false>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "checked_always_corrected_kary_bcast"
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
        auto tree = std::make_unique<BinomialTree>(rn);
        tree->forward_unexpected_message();

        auto phases = Combiner::Phases(rn).
          add_phase(std::move(tree)).
          add_phase<CheckedCorrection<false>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "checked_always_corrected_binomial_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto correction = std::make_unique<CheckedCorrection<false>>(rn);
        correction->throttled();

        auto phases = Combiner::Phases(rn).
          add_phase<BinomialTree>().
          add_phase(std::move(correction));

        return std::make_unique<Combiner>(std::move(phases));
      },
      "throttled_checked_corrected_binomial_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto phases = Combiner::Phases(rn).
          add_phase<LameTree>().
          add_phase<CheckedCorrection<false>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "checked_corrected_lame_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto tree = std::make_unique<LameTree>(rn);
        tree->forward_unexpected_message();

        auto phases = Combiner::Phases(rn).
          add_phase(std::move(tree)).
          add_phase<CheckedCorrection<false>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "checked_always_corrected_lame_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto phases = Combiner::Phases(rn).
          add_phase<Exclusive>(std::make_unique<LameTree>(rn)).
          add_phase<CheckedCorrection<true>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "phased_checked_corrected_lame_bcast"
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
        auto tree = std::make_unique<OptimalTree>(rn);
        tree->forward_unexpected_message();

        auto phases = Combiner::Phases(rn).
          add_phase(std::move(tree)).
          add_phase<CheckedCorrection<false>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "checked_always_corrected_optimal_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto tree = std::make_unique<OptimalTree>(rn);
        tree->forward_unexpected_message();

        auto phases = Combiner::Phases(rn).
          add_phase(std::move(tree)).
          add_phase<CheckedCorrection<true>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "checked_wraped_always_corrected_optimal_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto phases = Combiner::Phases(rn).
          add_phase<Exclusive>(std::make_unique<OptimalTree>(rn)).
          add_phase<CheckedCorrection<true>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "phased_checked_corrected_optimal_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto phases = Combiner::Phases(rn).
          add_phase<OptimalTree>().
          add_phase<Gossip>().
          add_phase<CheckedCorrection<false>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "checked_gossip_corrected_optimal_bcast"
    },

    {
      [](ReachedNodes &rn)
      {
        auto phases = Combiner::Phases(rn).
          add_phase<BinomialTree>().
          add_phase<Gossip>().
          add_phase<CheckedCorrection<false>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "checked_gossip_corrected_binomial_bcast"
    },

    {
      [](ReachedNodes &rn)
      {
        auto tree = std::make_unique<KAryTree>(rn);
        tree->forward_unexpected_message();

        auto phases = Combiner::Phases(rn).
          add_phase(std::move(tree)).
          add_phase<OpportunisticCorrection<false, false>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "opportunistic_corrected_kary_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto tree = std::make_unique<KAryTree>(rn);
        tree->forward_unexpected_message();

        auto phases = Combiner::Phases(rn).
          add_phase(std::move(tree)).
          add_phase<OpportunisticCorrection<true, true>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "optimized_opportunistic_corrected_kary_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto tree = std::make_unique<BinomialTree>(rn);
        tree->forward_unexpected_message();

        auto phases = Combiner::Phases(rn).
          add_phase(std::move(tree)).
          add_phase<OpportunisticCorrection<false, false>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "opportunistic_corrected_binomial_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto tree = std::make_unique<BinomialTree>(rn);
        tree->forward_unexpected_message();

        auto phases = Combiner::Phases(rn).
          add_phase(std::move(tree)).
          add_phase<OpportunisticCorrection<true, true>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "optimized_opportunistic_corrected_binomial_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto tree = std::make_unique<LameTree>(rn);
        tree->forward_unexpected_message();

        auto phases = Combiner::Phases(rn).
          add_phase(std::move(tree)).
          add_phase<OpportunisticCorrection<false, false>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "opportunistic_corrected_lame_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto tree = std::make_unique<LameTree>(rn);
        tree->forward_unexpected_message();

        auto phases = Combiner::Phases(rn).
          add_phase(std::move(tree)).
          add_phase<OpportunisticCorrection<true, true>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "optimized_opportunistic_corrected_lame_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto tree = std::make_unique<OptimalTree>(rn);
        tree->forward_unexpected_message();

        auto phases = Combiner::Phases(rn).
          add_phase(std::move(tree)).
          add_phase<OpportunisticCorrection<false, false>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "opportunistic_corrected_optimal_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto tree = std::make_unique<OptimalTree>(rn);
        tree->forward_unexpected_message();

        auto phases = Combiner::Phases(rn).
          add_phase(std::move(tree)).
          add_phase<OpportunisticCorrection<true, true>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "optimized_opportunistic_corrected_optimal_bcast"
    },

    {
      [](ReachedNodes &rn)
      {
        auto tree = std::make_unique<KAryTree>(rn);
        tree->forward_unexpected_message();

        auto phases = Combiner::Phases(rn).
          add_phase(std::move(tree)).
          add_phase<OpportunisticCorrection<false, false>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "opportunistic_always_corrected_kary_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto tree = std::make_unique<KAryTree>(rn);
        tree->forward_unexpected_message();

        auto phases = Combiner::Phases(rn).
          add_phase(std::move(tree)).
          add_phase<OpportunisticCorrection<true, true>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "optimized_opportunistic_always_corrected_kary_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto tree = std::make_unique<BinomialTree>(rn);
        tree->forward_unexpected_message();

        auto phases = Combiner::Phases(rn).
          add_phase(std::move(tree)).
          add_phase<OpportunisticCorrection<false, false>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "opportunistic_always_corrected_binomial_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto tree = std::make_unique<BinomialTree>(rn);
        tree->forward_unexpected_message();

        auto phases = Combiner::Phases(rn).
          add_phase(std::move(tree)).
          add_phase<OpportunisticCorrection<true, true>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "optimized_opportunistic_always_corrected_binomial_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto tree = std::make_unique<LameTree>(rn);
        tree->forward_unexpected_message();

        auto phases = Combiner::Phases(rn).
          add_phase(std::move(tree)).
          add_phase<OpportunisticCorrection<false, false>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "opportunistic_always_corrected_lame_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto tree = std::make_unique<LameTree>(rn);
        tree->forward_unexpected_message();

        auto phases = Combiner::Phases(rn).
          add_phase(std::move(tree)).
          add_phase<OpportunisticCorrection<true, true>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "optimized_opportunistic_always_corrected_lame_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto tree = std::make_unique<OptimalTree>(rn);
        tree->forward_unexpected_message();

        auto phases = Combiner::Phases(rn).
          add_phase(std::move(tree)).
          add_phase<OpportunisticCorrection<false, false>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "opportunistic_always_corrected_optimal_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto tree = std::make_unique<OptimalTree>(rn);
        tree->forward_unexpected_message();

        auto phases = Combiner::Phases(rn).
          add_phase(std::move(tree)).
          add_phase<OpportunisticCorrection<true, true>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "optimized_opportunistic_always_corrected_optimal_bcast"
    },

    {
      [](ReachedNodes &rn)
      {
        auto phases = Combiner::Phases(rn).
          add_phase<Gossip>().
          add_phase<OpportunisticCorrection<true, false>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "opportunistic_corrected_gossip_bcast"
    },
    {
      [](ReachedNodes &rn)
      {
        auto phases = Combiner::Phases(rn).
          add_phase<Gossip>().
          add_phase<OpportunisticCorrection<true, true>>();

        return std::make_unique<Combiner>(std::move(phases));
      },
      "optimised_opportunistic_corrected_gossip_bcast"
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
