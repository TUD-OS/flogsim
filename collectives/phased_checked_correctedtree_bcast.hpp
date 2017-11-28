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
  virtual Time correction_phase_start(int k) override
  {
    auto &model = Globals::get().model();
    auto o = model.o;
    auto g = model.g;
    auto P = model.P;

    int levels = int(std::floor(std::log(P) / std::log(k)));
    auto send_time = Time(std::max(o, g) * k);
    auto recv_time = o;
    return (send_time + Time(model.L) + recv_time) * levels;
  }
};
