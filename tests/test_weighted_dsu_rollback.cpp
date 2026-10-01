#include <gtest/gtest.h>
#include "graphlib/weighted_dsu_rollback.h"
using namespace graphlib;
TEST(WeightedDsuRollback, ConstraintsAndRollback){WeightedDsuRollback d(3);EXPECT_TRUE(d.unite(0,1,4));EXPECT_TRUE(d.unite(1,2,3));EXPECT_EQ(d.difference(0,2),7);int s=d.snapshot();EXPECT_TRUE(d.unite(0,2,7));d.rollback_to(s);EXPECT_TRUE(d.connected(0,2));EXPECT_FALSE(d.unite(0,2,8));}
