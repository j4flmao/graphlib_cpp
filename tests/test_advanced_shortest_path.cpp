#include <gtest/gtest.h>
#include <graphlib/shortest_path.h>
#include <vector>
#include <cmath>
#include <limits>

using graphlib::ShortestPath;

TEST(AdvancedShortestPath, AStar_EuclideanHeuristic) {
    // Grid 3x3
    // 0 1 2
    // 3 4 5
    // 6 7 8
    // Start 0, Target 8.
    // Euclidean distance on grid.
    // Heuristic h(u) = distance(u, 8)
    
    int n = 9;
    ShortestPath sp(n);
    
    // Add edges (grid connectivity)
    auto add_bi = [&](int u, int v) {
        sp.add_edge(u, v, 10); // cost 10 per step
        sp.add_edge(v, u, 10);
    };
    
    add_bi(0, 1); add_bi(1, 2);
    add_bi(3, 4); add_bi(4, 5);
    add_bi(6, 7); add_bi(7, 8);
    
    add_bi(0, 3); add_bi(3, 6);
    add_bi(1, 4); add_bi(4, 7);
    add_bi(2, 5); add_bi(5, 8);
    
    // Heuristic: Manhattan distance * 10 (admissible)
    std::vector<long long> h(n);
    auto get_pos = [](int u) { return std::make_pair(u / 3, u % 3); };
    int tx = 2, ty = 2; // Target 8
    
    for(int i=0; i<n; ++i) {
        auto [r, c] = get_pos(i);
        h[i] = (std::abs(r - tx) + std::abs(c - ty)) * 10;
    }
    
    long long inf = std::numeric_limits<long long>::max();
    auto dists = sp.a_star(0, 8, h, inf);
    
    // Shortest path is 0->1->2->5->8 or 0->3->6->7->8 etc. Length 4 steps -> 40.
    EXPECT_EQ(dists[8], 40);
    EXPECT_EQ(dists[0], 0);
}

TEST(AdvancedShortestPath, MinimumMeanCycle_Simple) {
    // Cycle 0->1->0 with weights 2, 2. Mean = (2+2)/2 = 2.
    int n = 2;
    ShortestPath sp(n);
    sp.add_edge(0, 1, 2);
    sp.add_edge(1, 0, 2);
    
    bool has_cycle = false;
    long double mean = sp.minimum_mean_cycle(has_cycle);
    
    EXPECT_TRUE(has_cycle);
    EXPECT_DOUBLE_EQ(mean, 2.0);
}

TEST(AdvancedShortestPath, MinimumMeanCycle_Disjoint) {
    // 0->1 (1)
    // 1->2 (1)
    // 2->0 (1)  Mean = 1
    // 3->4 (10)
    // 4->3 (10) Mean = 10
    // Min mean cycle should be 1.
    int n = 5;
    ShortestPath sp(n);
    sp.add_edge(0, 1, 1);
    sp.add_edge(1, 2, 1);
    sp.add_edge(2, 0, 1);
    
    sp.add_edge(3, 4, 10);
    sp.add_edge(4, 3, 10);
    
    bool has_cycle = false;
    long double mean = sp.minimum_mean_cycle(has_cycle);
    
    EXPECT_TRUE(has_cycle);
    EXPECT_DOUBLE_EQ(mean, 1.0);
}

TEST(AdvancedShortestPath, MinimumMeanCycle_NoCycle) {
    int n = 3;
    ShortestPath sp(n);
    sp.add_edge(0, 1, 1);
    sp.add_edge(1, 2, 1);
    
    bool has_cycle = false;
    sp.minimum_mean_cycle(has_cycle);
    EXPECT_FALSE(has_cycle);
}
