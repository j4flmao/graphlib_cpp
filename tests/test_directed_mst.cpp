#include "graphlib/directed_mst.h"
#include <gtest/gtest.h>
#include <vector>

using namespace graphlib;

TEST(DirectedMSTTest, SimpleDAG) {
    int n = 4;
    int root = 0;
    // 0 -> 1 (10)
    // 0 -> 2 (10)
    // 1 -> 3 (5)
    // 2 -> 3 (10)
    // MST should be 0->1, 0->2, 1->3 with cost 10+10+5 = 25
    std::vector<DirectedEdge> edges = {
        {0, 1, 10, 0},
        {0, 2, 10, 1},
        {1, 3, 5, 2},
        {2, 3, 10, 3}
    };
    std::vector<int> res_edges;
    long long cost = directed_mst(n, root, edges, res_edges);
    EXPECT_EQ(cost, 25);
}

TEST(DirectedMSTTest, WithCycle) {
    int n = 3;
    int root = 0;
    // 0 -> 1 (10)
    // 1 -> 2 (10)
    // 2 -> 1 (5)  <-- Cycle 1-2 with weight 10+5=15. Min incoming to cycle is 10 (at 1)
    // 2 -> 3 (10)
    // 0 -> 3 (30)
    
    // Optimal: 0->1 (10), 1->2 (10), 2->3 (10). Total 30.
    // Wait, if we pick 0->1, 1->2, 2->3, cost is 30.
    // Is there a cheaper way?
    // Reach 3: via 2 (cost 10) or via 0 (cost 30).
    // Reach 2: via 1 (cost 10).
    // Reach 1: via 0 (cost 10) or via 2 (cost 5).
    // If we use 2->1, we must reach 2 first. 0->1->2->1... cycle.
    // Valid arborescence must have no cycles.
    // 0->1 (10), 1->2 (10), 2->3 (10). Cost 30.
    
    // Let's try a case where cycle contraction is needed to find optimal.
    // 0 -> 1 (10)
    // 1 -> 2 (10)
    // 2 -> 3 (10)
    // 3 -> 1 (10)  <-- Cycle 1-2-3 cost 30.
    // 0 -> 2 (50)
    // MST: 0->1 (10), 1->2 (10), 2->3 (10). Cost 30.
    // What if we have:
    // 0->1 (100)
    // 0->2 (100)
    // 1->2 (1)
    // 2->1 (1)
    // MST: 0->1 (100), 1->2 (1). Cost 101.
    
    std::vector<DirectedEdge> edges = {
        {0, 1, 100, 0},
        {0, 2, 100, 1},
        {1, 2, 1, 2},
        {2, 1, 1, 3}
    };
    std::vector<int> res_edges;
    long long cost = directed_mst(n, root, edges, res_edges);
    EXPECT_EQ(cost, 101);
}

TEST(DirectedMSTTest, CycleContractionComplex) {
    // Standard Chu-Liu/Edmonds test case
    // Root 0.
    // 0->1 (5)
    // 1->2 (10)
    // 2->3 (10)
    // 3->1 (10)  Cycle 1-2-3, min in-edge to cycle from outside is 0->1 (5).
    // Also 0->2 (20).
    
    // Edges in cycle: (1,2,10), (2,3,10), (3,1,10).
    // Cheapest incoming edges for each node:
    // 1: 0->1 (5) vs 3->1 (10). Min is 5.
    // 2: 1->2 (10) vs 0->2 (20). Min is 10.
    // 3: 2->3 (10). Min is 10.
    
    // Sum of min incoming: 5 + 10 + 10 = 25.
    // Cycle detected: 1-2-3.
    // Contract 1-2-3 into node C.
    // Edges into C:
    // 0->1 becomes 0->C with w' = w - min_in[1] = 5 - 5 = 0.
    // 0->2 becomes 0->C with w' = w - min_in[2] = 20 - 10 = 10.
    // Selected edge 0->C (from 0->1) has cost 0.
    // Total cost = 25 + 0 = 25.
    
    int n = 4;
    int root = 0;
    std::vector<DirectedEdge> edges = {
        {0, 1, 5, 0},
        {0, 2, 20, 1},
        {1, 2, 10, 2},
        {2, 3, 10, 3},
        {3, 1, 10, 4}
    };
    std::vector<int> res_edges;
    long long cost = directed_mst(n, root, edges, res_edges);
    EXPECT_EQ(cost, 25);
}

TEST(DirectedMSTTest, Unreachable) {
    int n = 3;
    int root = 0;
    // 0->1 (10)
    // 2 (isolated)
    std::vector<DirectedEdge> edges = {
        {0, 1, 10, 0}
    };
    std::vector<int> res_edges;
    long long cost = directed_mst(n, root, edges, res_edges);
    EXPECT_EQ(cost, -1);
}

TEST(DirectedMSTTest, DisconnectedComponents) {
    int n = 4;
    int root = 0;
    // 0->1
    // 2->3
    std::vector<DirectedEdge> edges = {
        {0, 1, 10, 0},
        {2, 3, 10, 1}
    };
    std::vector<int> res_edges;
    long long cost = directed_mst(n, root, edges, res_edges);
    EXPECT_EQ(cost, -1);
}

TEST(DirectedMSTTest, SelfLoops) {
    int n = 2;
    int root = 0;
    // 0->1 (10)
    // 1->1 (5)
    std::vector<DirectedEdge> edges = {
        {0, 1, 10, 0},
        {1, 1, 5, 1}
    };
    std::vector<int> res_edges;
    long long cost = directed_mst(n, root, edges, res_edges);
    EXPECT_EQ(cost, 10);
}

TEST(DirectedMSTTest, SimpleCycle) {
    // 0 -> 1 (10)
    // 1 -> 2 (10)
    // 2 -> 0 (10)
    // 0 -> 2 (100)
    // Root 0. MST should be 0->1, 1->2. Cost 20.
    
    std::vector<DirectedEdge> edges = {
        {0, 1, 10, 0},
        {1, 2, 10, 1},
        {2, 0, 10, 2},
        {0, 2, 100, 3}
    };
    
    std::vector<int> res_edges;
    long long cost = directed_mst(3, 0, edges, res_edges);
    EXPECT_EQ(cost, 20);
}
