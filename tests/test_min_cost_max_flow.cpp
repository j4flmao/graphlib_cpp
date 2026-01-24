#include "graphlib/max_flow.h"
#include <gtest/gtest.h>
#include <vector>

using namespace graphlib;

TEST(MinCostMaxFlowTest, BasicCase) {
    // S -> 0 (cap 2, cost 1)
    // S -> 1 (cap 2, cost 2)
    // 0 -> T (cap 1, cost 1)
    // 1 -> T (cap 1, cost 1)
    // S=2, T=3
    
    // Max flow is 2.
    // Paths:
    // S->0->T (cost 1+1=2), flow 1
    // S->1->T (cost 2+1=3), flow 1
    // Total cost: 2 + 3 = 5.
    
    int S = 2;
    int T = 3;
    MaxFlow mf(4);
    mf.add_edge(S, 0, 2, 1);
    mf.add_edge(S, 1, 2, 2);
    mf.add_edge(0, T, 1, 1);
    mf.add_edge(1, T, 1, 1);
    
    auto result = mf.min_cost_max_flow(S, T);
    EXPECT_EQ(result.first, 2);
    EXPECT_EQ(result.second, 5);
}

TEST(MinCostMaxFlowTest, NegativeCost) {
    // S -> 0 (cap 1, cost 10)
    // S -> 1 (cap 1, cost 10)
    // 0 -> 1 (cap 1, cost -5)
    // 0 -> T (cap 1, cost 10)
    // 1 -> T (cap 1, cost 10)
    
    // Paths:
    // S->0->T (cost 20)
    // S->1->T (cost 20)
    // S->0->1->T (cost 10 - 5 + 10 = 15)
    
    // Max flow 2.
    // Optimal:
    // 1 unit S->0->1->T (cost 15)
    // 1 unit S->1->T (cost 20)
    // Total flow 2, cost 35.
    
    // Wait, S->0 cap is 1. S->1 cap is 1.
    // Path S->0->1->T uses S->0, 0->1, 1->T.
    // Remaining cap: S->1 (1).
    // Can send 1 unit S->1->T?
    // 1->T cap is 1. But S->0->1->T used 1 unit of 1->T?
    // No, S->0->1->T uses 1 unit of 1->T.
    // So 1->T is full.
    // Can we send more?
    // S->0 used. 0->1 used. 1->T used.
    // S->1 unused.
    // Path S->1->T? 1->T is full.
    // Path S->1 -> ...
    // S->1 (cost 10) -> (rev 0->1 cost 5) -> 0 -> T (cost 10). Total 10+5+10 = 25.
    // So send 1 unit S->1->0->T.
    // Total flow 2.
    // Cost: 15 (S->0->1->T) + 25 (S->1->0->T) = 40.
    
    // Let's verify costs:
    // Edge (S,0): 10
    // Edge (S,1): 10
    // Edge (0,1): -5
    // Edge (0,T): 10
    // Edge (1,T): 10
    
    // Flow 1: S->0->1->T. Cost 10 - 5 + 10 = 15.
    // Residual:
    // (S,0) 0, (0,S) 1 (-10)
    // (0,1) 0, (1,0) 1 (5)
    // (1,T) 0, (T,1) 1 (-10)
    // (S,1) 1 (10)
    // (0,T) 1 (10)
    
    // Shortest path in residual?
    // S -> 1 (10) -> 0 (5) -> T (10). Cost 25.
    // Flow 1.
    // Total cost 15 + 25 = 40.
    
    int S = 2, T = 3;
    MaxFlow mf(4);
    mf.add_edge(S, 0, 1, 10);
    mf.add_edge(S, 1, 1, 10);
    mf.add_edge(0, 1, 1, -5);
    mf.add_edge(0, T, 1, 10);
    mf.add_edge(1, T, 1, 10);
    
    auto result = mf.min_cost_max_flow(S, T);
    EXPECT_EQ(result.first, 2);
    EXPECT_EQ(result.second, 40);
}

TEST(MinCostMaxFlowTest, LargeCost) {
    int n = 4, S = 0, T = 3;
    MaxFlow mf(n);
    long long high_cost = 1000000;
    mf.add_edge(S, 1, 1, high_cost);
    mf.add_edge(1, T, 1, high_cost);
    mf.add_edge(S, 2, 1, 1);
    mf.add_edge(2, T, 1, 1);
    
    // Should pick S->2->T first (cost 2)
    // Then S->1->T (cost 2*high_cost)
    
    auto result = mf.min_cost_max_flow(S, T);
    EXPECT_EQ(result.first, 2);
    EXPECT_EQ(result.second, 2 + 2*high_cost);
}
