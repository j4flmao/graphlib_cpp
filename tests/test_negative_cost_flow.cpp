#include <gtest/gtest.h>
#include "graphlib/max_flow.h"
#include <vector>

using namespace graphlib;

TEST(MinCostMaxFlow, NegativeCostsNoCycle) {
    // S -> A (cap 1, cost 10)
    // A -> B (cap 1, cost -5)
    // B -> T (cap 1, cost 10)
    // Total cost 15, Flow 1.
    MaxFlow mf(4);
    int S = 0, A = 1, B = 2, T = 3;
    mf.add_edge(S, A, 1, 10);
    mf.add_edge(A, B, 1, -5);
    mf.add_edge(B, T, 1, 10);

    auto result = mf.min_cost_max_flow(S, T);
    EXPECT_EQ(result.first, 1);
    EXPECT_EQ(result.second, 15);
}

TEST(MinCostMaxFlow, NegativeCostsComplex) {
    // S -> A (cap 10, cost 0)
    // S -> B (cap 10, cost 0)
    // A -> C (cap 10, cost 10)
    // B -> C (cap 10, cost 20)
    // C -> D (cap 10, cost -15)
    // D -> T (cap 10, cost 0)
    
    // Path S->A->C->D->T: cost 0 + 10 - 15 + 0 = -5.
    // Path S->B->C->D->T: cost 0 + 20 - 15 + 0 = 5.
    
    // Max flow should prefer A->C.
    
    MaxFlow mf(6);
    int S = 0, A = 1, B = 2, C = 3, D = 4, T = 5;
    
    mf.add_edge(S, A, 10, 0);
    mf.add_edge(S, B, 10, 0);
    mf.add_edge(A, C, 10, 10);
    mf.add_edge(B, C, 10, 20);
    mf.add_edge(C, D, 10, -15);
    mf.add_edge(D, T, 10, 0);
    
    // Flow 10 via S->A... Cost 10 * -5 = -50.
    // Flow 10 via S->B... Cost 10 * 5 = 50. (But capacity of C->D is 10 shared)
    // So only 10 units total can pass C->D.
    // Algorithm should pick S->A path.
    
    auto result = mf.min_cost_max_flow(S, T);
    EXPECT_EQ(result.first, 10);
    EXPECT_EQ(result.second, -50);
}

TEST(MinCostMaxFlow, NegativeCycleDetection) {
    // S -> A (cap 1, cost 1)
    // A -> B (cap 1, cost -5)
    // B -> A (cap 1, cost -5) 
    // A-B-A is negative cycle (-10).
    // B -> T (cap 1, cost 1)
    
    MaxFlow mf(4);
    int S = 0, A = 1, B = 2, T = 3;
    mf.add_edge(S, A, 1, 1);
    mf.add_edge(A, B, 1, -5);
    mf.add_edge(B, A, 1, -5);
    mf.add_edge(B, T, 1, 1);
    
    EXPECT_THROW(mf.min_cost_max_flow(S, T), std::runtime_error);
}
