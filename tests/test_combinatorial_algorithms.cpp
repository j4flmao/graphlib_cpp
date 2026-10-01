#include <gtest/gtest.h>
#include "graphlib/combinatorial_algorithms.h"
using namespace graphlib;
TEST(CombinatorialAlgorithms, FunctionalGraph){auto r=analyze_functional_graph({1,2,0,2,3});ASSERT_EQ(r.cycle_lengths.size(),1u);EXPECT_EQ(r.cycle_lengths[0],3);EXPECT_EQ(r.distance_to_cycle[4],2);}
TEST(CombinatorialAlgorithms, GraphicalSequence){EXPECT_TRUE(is_graphical_degree_sequence({3,3,2,2,2}));EXPECT_FALSE(is_graphical_degree_sequence({3,3,3,1}));auto e=realize_degree_sequence({2,2,2});EXPECT_EQ(e.size(),3u);}
