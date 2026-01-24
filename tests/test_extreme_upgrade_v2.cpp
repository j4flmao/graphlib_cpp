#include "graphlib/graphlib.h"
#include "graphlib/np_hard.h"
#include "graphlib/max_flow.h"
#include <gtest/gtest.h>
#include <numeric>
#include <algorithm>

using namespace graphlib;

class ExtremeUpgradeV2Test : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// -----------------------------------------------------------------------------
// Metric TSP Approx Tests
// -----------------------------------------------------------------------------

TEST_F(ExtremeUpgradeV2Test, TspMetricApprox_Triangle) {
    // Triangle with edges 3, 4, 5
    // 0-1: 3
    // 1-2: 4
    // 0-2: 5
    // MST: (0,1) weight 3, (1,2) weight 4. Cost 7.
    // DFS on MST (0-1-2) -> Tour 0-1-2-0.
    // Edges: (0,1)=3, (1,2)=4, (2,0)=5. Total 12.
    
    Graph g(3, false); // Undirected
    g.add_edge(0, 1, 3);
    g.add_edge(1, 2, 4);
    g.add_edge(0, 2, 5);
    
    auto result = tsp_metric_approx(g);
    
    EXPECT_EQ(result.first, 12);
    EXPECT_EQ(result.second.size(), 3); // 0, 1, 2 (implicit return to 0)
    
    // Check if it contains all nodes
    std::vector<int> sorted_path = result.second;
    std::sort(sorted_path.begin(), sorted_path.end());
    EXPECT_EQ(sorted_path[0], 0);
    EXPECT_EQ(sorted_path[1], 1);
    EXPECT_EQ(sorted_path[2], 2);
}

TEST_F(ExtremeUpgradeV2Test, TspMetricApprox_Disconnected) {
    Graph g(3, false);
    g.add_edge(0, 1, 10);
    // 2 is isolated
    
    auto result = tsp_metric_approx(g);
    EXPECT_EQ(result.first, -1);
}

// -----------------------------------------------------------------------------
// Vertex Cover Approx Tests
// -----------------------------------------------------------------------------

TEST_F(ExtremeUpgradeV2Test, VertexCoverApprox_Triangle) {
    Graph g(3, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);
    
    auto cover = vertex_cover_approx(g);
    
    // Should pick one edge, e.g., (0,1). Cover {0,1}.
    // Size should be 2.
    EXPECT_EQ(cover.size(), 2);
    
    // Verify it is a valid cover
    // Actually, approx algo produces A valid cover, but not necessarily minimal.
    // But for triangle, OPT=2. Approx returns 2.
}

TEST_F(ExtremeUpgradeV2Test, VertexCoverApprox_Star) {
    // Star graph: 0 center, 1,2,3,4 leaves
    Graph g(5, false);
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(0, 3);
    g.add_edge(0, 4);
    
    auto cover = vertex_cover_approx(g);
    
    // Maximal matching will pick ONE edge, say (0,1).
    // Then 0 is covered. All other edges incident to 0 are covered?
    // Wait. "Mark u and v as covered".
    // "Loop: If edge (u,v) is not covered..."
    // Iteration 1: Pick (0,1). Cover {0,1}. Covered[0]=true, Covered[1]=true.
    // Iteration 2: Check (0,2). 0 is covered. Skip.
    // Iteration 3: Check (0,3). 0 is covered. Skip.
    // ...
    // So result is {0,1}. Size 2.
    // OPT is {0} (size 1).
    // Approx factor 2. Correct.
    
    EXPECT_EQ(cover.size(), 2);
    // Should contain 0 and one leaf
    bool has_zero = false;
    for(int x : cover) if(x==0) has_zero = true;
    EXPECT_TRUE(has_zero);
}

// -----------------------------------------------------------------------------
// Maximum Weight Closure Tests
// -----------------------------------------------------------------------------

TEST_F(ExtremeUpgradeV2Test, MaxWeightClosure_Simple) {
    // 0 (+10), 1 (-5). 0 -> 1.
    std::vector<long long> weights = {10, -5};
    std::vector<std::pair<int, int>> deps = {{0, 1}};
    
    long long result = maximum_weight_closure(weights, deps);
    EXPECT_EQ(result, 5); // {0,1} -> 10-5=5
}

TEST_F(ExtremeUpgradeV2Test, MaxWeightClosure_NegativeDominated) {
    // 0 (+10), 1 (-20). 0 -> 1.
    std::vector<long long> weights = {10, -20};
    std::vector<std::pair<int, int>> deps = {{0, 1}};
    
    long long result = maximum_weight_closure(weights, deps);
    EXPECT_EQ(result, 0); // {} -> 0
}

TEST_F(ExtremeUpgradeV2Test, MaxWeightClosure_Merge) {
    // 0(+10), 1(+10), 2(-15). 0->2, 1->2.
    std::vector<long long> weights = {10, 10, -15};
    std::vector<std::pair<int, int>> deps = {{0, 2}, {1, 2}};
    
    long long result = maximum_weight_closure(weights, deps);
    EXPECT_EQ(result, 5); // {0,1,2} -> 20-15=5
}

TEST_F(ExtremeUpgradeV2Test, MaxWeightClosure_Independent) {
    // 0(+10), 1(-5). No deps.
    std::vector<long long> weights = {10, -5};
    std::vector<std::pair<int, int>> deps = {};
    
    long long result = maximum_weight_closure(weights, deps);
    EXPECT_EQ(result, 10); // {0}
}
