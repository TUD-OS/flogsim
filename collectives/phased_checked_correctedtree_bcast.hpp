#include <algorithm>

#include <cmath>

#include "collective.hpp"
#include "task_queue.hpp"
#include "globals.hpp"

#include "corrected_tree_broadcast.hpp"

class PhasedCheckedCorrectedTreeBroadcast
  : public CorrectedTreeBroadcast<true>
{
  // This method ensures that the correction phase starts when tree
  // phase ends. It need to compute right moment of time, when
  // correction may start.
  static Time __correction_phase_start(auto L, auto o, auto g, int P, int k)
  {
    if (P <= 1) {
      return Time(0);
    }

    if (P <= k + 1) {
      return std::max(o, g) * P + L;
    }

    Time subtree_max(0);

    for (int i = 1; i <= k; i++) {
      int sub_P = (P - 1) / k + (i <= (P - 1) % k);
      Time sub_time = __correction_phase_start(L, o, g, sub_P, k);
      subtree_max = std::max(subtree_max, std::max(o, g) * (i + 1) + L + sub_time);
    }

    return subtree_max;
  }

  virtual Time correction_phase_start(int k) override
  {
    auto &model = Globals::get().model();
    auto L = model.L;
    auto o = model.o;
    auto g = model.g;
    auto P = model.P;

    return __correction_phase_start(L, o, g, P, k);
  }
public:
  static constexpr char name[] = "phased_checked_correctedtree_bcast";
};
