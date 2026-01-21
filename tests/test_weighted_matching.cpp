#include <gtest/gtest.h>
#include "graphlib/general_matching.h"
#include <vector>
#include <tuple>

using namespace graphlib;

TEST(WeightedMatching, PathSelectsHeavyMiddle) {
    // 0 --(10)-- 1 --(100)-- 2 --(10)-- 3
    // Max weight matching should be (1, 2) with weight 100.
    // Maximal matching could be (0,1) and (2,3) with weight 20.
    // We want Maximum Weight.
    
    GeneralMatching g(4);
    g.add_edge(0, 1, 10);
    g.add_edge(1, 2, 100);
    g.add_edge(2, 3, 10);
    
    EXPECT_EQ(g.maximum_weight_matching(), 100);
}

TEST(WeightedMatching, PathSelectsEnds) {
    // 0 --(100)-- 1 --(10)-- 2 --(100)-- 3
    // Max weight: (0,1) and (2,3) -> 200.
    
    GeneralMatching g(4);
    g.add_edge(0, 1, 100);
    g.add_edge(1, 2, 10);
    g.add_edge(2, 3, 100);
    
    EXPECT_EQ(g.maximum_weight_matching(), 200);
}

TEST(WeightedMatching, TriangleMaxEdge) {
    // Triangle with weights 10, 20, 30.
    // Can pick only 1 edge. Should pick 30.
    
    GeneralMatching g(3);
    g.add_edge(0, 1, 10);
    g.add_edge(1, 2, 20);
    g.add_edge(2, 0, 30);
    
    EXPECT_EQ(g.maximum_weight_matching(), 30);
}

TEST(WeightedMatching, BlossomGraphWeighted) {
    // A 5-cycle (blossom)
    // 0-1 (10)
    // 1-2 (10)
    // 2-3 (10)
    // 3-4 (10)
    // 4-0 (10)
    // Max matching is 2 edges, weight 20.
    
    GeneralMatching g(5);
    g.add_edge(0, 1, 10);
    g.add_edge(1, 2, 10);
    g.add_edge(2, 3, 10);
    g.add_edge(3, 4, 10);
    g.add_edge(4, 0, 10);
    
    EXPECT_EQ(g.maximum_weight_matching(), 20);
}

TEST(WeightedMatching, NegativeWeights) {
    // If implementation supports negative weights, it should ignore them if they reduce total weight 
    // (unless it's perfect matching, but usually general max weight matching allows leaving nodes unmatched).
    // Let's assume it maximizes total weight of selected edges.
    
    GeneralMatching g(2);
    g.add_edge(0, 1, -10);
    
    // Should select 0 edges if negative edges are allowed but not forced.
    // If the library forces max cardinality then max weight, it might pick it.
    // Based on standard "Maximum Weight Matching", usually we only pick positive edges.
    // Let's verify behavior. If it returns -10, it forces maximal matching?
    // If 0, it optimizes sum.
    
    // To be safe, let's test a case where a negative edge is better than nothing? No, 0 is better.
    // Let's test mixed.
    // 0 --(10)-- 1 --(-5)-- 2
    // Should pick 10.
    
    GeneralMatching g2(3);
    g2.add_edge(0, 1, 10);
    g2.add_edge(1, 2, -5);
    
    EXPECT_EQ(g2.maximum_weight_matching(), 10);
}

TEST(WeightedMatching, K4_VariousWeights) {
    // 0-1 (10), 0-2 (5), 0-3 (5)
    // 1-2 (5), 1-3 (5)
    // 2-3 (10)
    // Best: (0,1) and (2,3) -> 10 + 10 = 20.
    
    GeneralMatching g(4);
    g.add_edge(0, 1, 10);
    g.add_edge(0, 2, 5);
    g.add_edge(0, 3, 5);
    g.add_edge(1, 2, 5);
    g.add_edge(1, 3, 5);
    g.add_edge(2, 3, 10);
    
    EXPECT_EQ(g.maximum_weight_matching(), 20);
}
