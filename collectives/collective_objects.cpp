#include "binary_bcast.hpp"
#include "simple_tree_bcast.hpp"
#include "fixed_correctedtree_bcast.hpp"
#include "checked_correctedtree_bcast.hpp"
#include "phased_checked_correctedtree_bcast.hpp"

namespace
{
CollectiveRegistrator<BinaryBroadcast>a{};
CollectiveRegistrator<SimpleTreeBroadcast>b{};
CollectiveRegistrator<FixedCorrectedTreeBroadcast>c{};
CollectiveRegistrator<CheckedCorrectedTreeBroadcast>d{};
CollectiveRegistrator<PhasedCheckedCorrectedTreeBroadcast>e{};
}
