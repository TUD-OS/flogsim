#include "gtest/gtest.h"

#include "simple_tree_bcast.hpp"
#include "globals.hpp"


#define ALGORITHM SimpleTreeBroadcast
#include "Wrappers.hpp"

namespace
{

TEST(ALGORITHM, Functional)
{

	MAKE_NOFAULT_TESTCASE(1,1,1, 4, 3, 5)
	MAKE_NOFAULT_TESTCASE(1,1,1,13, 3,10)
	MAKE_NOFAULT_TESTCASE(1,1,1, 5, 4, 6)

	MAKE_UNIFORMFAULT_TESTCASE(1,1,1,4,3,{0}, 0,3)
	MAKE_UNIFORMFAULT_TESTCASE(1,1,1,4,3,{1}, 4,0)

}

}
