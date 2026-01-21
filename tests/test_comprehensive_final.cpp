#include <gtest/gtest.h>
#include <graphlib/shortest_path.h>
#include <graphlib/graph_core.h>
#include <vector>
#include <cmath>
#include <limits>

using namespace graphlib;

class ComprehensiveFinalTest : public ::testing::Test {
protected:
    long long inf = std::numeric_limits<long long>::max();
};

// 1. Johnson's Algorithm Test
TEST_F(ComprehensiveFinalTest, JohnsonNegativeWeightsNoCycle) {
    int n = 4;
    ShortestPath sp(n);
    // 0 -> 1 (-2)
    // 1 -> 2 (-1)
    // 2 -> 3 (1)
    // 0 -> 3 (5)
    sp.add_edge(0, 1, -2);
    sp.add_edge(1, 2, -1);
    sp.add_edge(2, 3, 1);
    sp.add_edge(0, 3, 5);

    bool has_neg_cycle = false;
    auto dists = sp.johnson(inf, has_neg_cycle);

    ASSERT_FALSE(has_neg_cycle);
    // Distances from 0:
    // 0: 0
    // 1: -2
    // 2: -3
    // 3: -2
    EXPECT_EQ(dists[0][0], 0);
    EXPECT_EQ(dists[0][1], -2);
    EXPECT_EQ(dists[0][2], -3);
    EXPECT_EQ(dists[0][3], -2);
    
    // Distances from 1:
    // 1: 0
    // 2: -1
    // 3: 0
    EXPECT_EQ(dists[1][2], -1);
    EXPECT_EQ(dists[1][3], 0);
}

TEST_F(ComprehensiveFinalTest, JohnsonNegativeCycleDetection) {
    int n = 3;
    ShortestPath sp(n);
    sp.add_edge(0, 1, 1);
    sp.add_edge(1, 2, -5);
    sp.add_edge(2, 0, 1); // Cycle 0-1-2-0 cost: 1 - 5 + 1 = -3

    bool has_neg_cycle = false;
    sp.johnson(inf, has_neg_cycle);
    EXPECT_TRUE(has_neg_cycle);
}

// 2. Minimum Mean Cycle Test
TEST_F(ComprehensiveFinalTest, MinimumMeanCycleSimple) {
    int n = 4;
    ShortestPath sp(n);
    // Cycle 1: 0-1-0, weight 1+1=2, len 2, mean 1
    // Cycle 2: 2-3-2, weight 10+10=20, len 2, mean 10
    sp.add_edge(0, 1, 1);
    sp.add_edge(1, 0, 1);
    sp.add_edge(2, 3, 10);
    sp.add_edge(3, 2, 10);
    sp.add_edge(1, 2, 100); // Connector

    bool has_cycle = false;
    long double mean = sp.minimum_mean_cycle(has_cycle);
    
    EXPECT_TRUE(has_cycle);
    EXPECT_NEAR(mean, 1.0, 1e-9);
}

TEST_F(ComprehensiveFinalTest, MinimumMeanCycleNoCycle) {
    int n = 3;
    ShortestPath sp(n);
    sp.add_edge(0, 1, 1);
    sp.add_edge(1, 2, 1);
    
    bool has_cycle = false;
    sp.minimum_mean_cycle(has_cycle);
    EXPECT_FALSE(has_cycle);
}

// 3. A* Search Test
TEST_F(ComprehensiveFinalTest, AStarGrid) {
    // 3x3 Grid, move right/down/diag
    // 0 1 2
    // 3 4 5
    // 6 7 8
    int n = 9;
    ShortestPath sp(n);
    
    // Edges with weight 10 (cardinal) and 14 (diagonal)
    auto add_bi = [&](int u, int v, int w) {
        sp.add_edge(u, v, w);
        sp.add_edge(v, u, w);
    };
    
    add_bi(0, 1, 10); add_bi(1, 2, 10);
    add_bi(3, 4, 10); add_bi(4, 5, 10);
    add_bi(6, 7, 10); add_bi(7, 8, 10);
    
    add_bi(0, 3, 10); add_bi(3, 6, 10);
    add_bi(1, 4, 10); add_bi(4, 7, 10);
    add_bi(2, 5, 10); add_bi(5, 8, 10);

    // Heuristic: Manhattan distance * 10
    std::vector<long long> h(n);
    int target = 8;
    int tx = 2, ty = 2;
    for(int i=0; i<n; ++i) {
        int x = i % 3;
        int y = i / 3;
        h[i] = (std::abs(x - tx) + std::abs(y - ty)) * 10;
    }
    
    auto dist = sp.a_star(0, 8, h, inf);
    // Shortest path: 0->1->2->5->8 or 0->3->6->7->8 or 0->1->4->7->8 etc.
    // Cost: 10+10+10+10 = 40
    EXPECT_EQ(dist[8], 40);
}

// 4. Multi-source Dijkstra
TEST_F(ComprehensiveFinalTest, MultiSourceDijkstra) {
    int n = 6;
    ShortestPath sp(n);
    // Source A: 0
    // Source B: 1
    // 0 -> 2 (10)
    // 1 -> 3 (5)
    // 2 -> 4 (10) -> cost from 0 is 20
    // 3 -> 4 (20) -> cost from 1 is 25
    // 4 -> 5 (1)
    
    sp.add_edge(0, 2, 10);
    sp.add_edge(1, 3, 5);
    sp.add_edge(2, 4, 10);
    sp.add_edge(3, 4, 20);
    sp.add_edge(4, 5, 1);
    
    std::vector<int> sources = {0, 1};
    auto dist = sp.multi_source_dijkstra(sources, inf);
    
    EXPECT_EQ(dist[0], 0);
    EXPECT_EQ(dist[1], 0);
    EXPECT_EQ(dist[2], 10); // from 0
    EXPECT_EQ(dist[3], 5);  // from 1
    EXPECT_EQ(dist[4], 20); // from 0 (10+10=20) vs from 1 (5+20=25) -> pick 20
    EXPECT_EQ(dist[5], 21);
}

// 5. 0-1 BFS
TEST_F(ComprehensiveFinalTest, ZeroOneBFS) {
    int n = 3;
    ShortestPath sp(n);
    sp.add_edge(0, 1, 0);
    sp.add_edge(1, 2, 1);
    sp.add_edge(0, 2, 1); // Alternative
    
    auto dist = sp.zero_one_bfs(0, inf);
    EXPECT_EQ(dist[1], 0);
    EXPECT_EQ(dist[2], 1);
}

// 6. Path Reconstruction
TEST_F(ComprehensiveFinalTest, PathReconstruction) {
    std::vector<int> parent = {-1, 0, 1, 2, 3}; // 0->1->2->3->4
    auto path = reconstruct_path(0, 4, parent);
    ASSERT_EQ(path.size(), 5);
    EXPECT_EQ(path[0], 0);
    EXPECT_EQ(path[4], 4);
    
    // No path
    parent = {-1, -1, -1};
    path = reconstruct_path(0, 2, parent);
    EXPECT_TRUE(path.empty());
}
