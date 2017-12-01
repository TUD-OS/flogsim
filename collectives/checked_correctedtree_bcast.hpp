#include <algorithm>

#include <cmath>

#include "collective.hpp"
#include "task_queue.hpp"

#include "corrected_tree_broadcast.hpp"

class CheckedCorrectedTreeBroadcast
  : public CorrectedTreeBroadcast<false>
{
public:
  static constexpr char name[] = "checked_correctedtree_bcast";
};
