#include "binary_bcast.hpp"
#include "simple_tree_bcast.hpp"
#include "fixed_correctedtree_bcast.hpp"
#include "checked_correctedtree_bcast.hpp"
#include "phased_checked_correctedtree_bcast.hpp"

static CollectiveRegistrator<BinaryBroadcast> bb("binary_bcast");
static CollectiveRegistrator<SimpleTreeBroadcast> st("simple_tree_bcast");
static CollectiveRegistrator<CorrectedTreeBroadcast> ctb("fixed_correctedtree_bcast");
static CollectiveRegistrator<CheckedCorrectedTreeBroadcast> cctb("checked_correctedtree_bcast");
static CollectiveRegistrator<PhasedCheckedCorrectedTreeBroadcast> pcctb("phased_checked_correctedtree_bcast");
