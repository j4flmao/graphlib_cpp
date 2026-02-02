#include <gtest/gtest.h>
#include "graphlib/shortest_path.h"
#include "graphlib/graph_core.h"
#include <vector>

using namespace graphlib;

TEST(ShortestPathTest, NegativeCycleFinding) {
    // 0 -> 1 (-1)
    // 1 -> 2 (-1)
    // 2 -> 0 (-1)
    // Cycle 0-1-2-0 cost -3.
    // Plus isolated node 3.
    ShortestPath sp(4);
    sp.add_edge(0, 1, -1);
    sp.add_edge(1, 2, -1);
    sp.add_edge(2, 0, -1);
    
    std::vector<int> cycle = sp.find_negative_cycle(100);
    EXPECT_FALSE(cycle.empty());
    // Should be at least 0, 1, 2, 0 (size 4)
    EXPECT_GE(cycle.size(), 4);
    EXPECT_EQ(cycle.front(), cycle.back());
}

TEST(ShortestPathTest, NoNegativeCycle) {
    // 0 -> 1 (1)
    // 1 -> 2 (1)
    // 2 -> 0 (1)
    // Cycle 0-1-2-0 cost 3 (positive).
    ShortestPath sp(3);
    sp.add_edge(0, 1, 1);
    sp.add_edge(1, 2, 1);
    sp.add_edge(2, 0, 1);
    
    std::vector<int> cycle = sp.find_negative_cycle(100);
    EXPECT_TRUE(cycle.empty());
}

TEST(ShortestPathTest, TopologicalSort) {
    // 0->1, 0->2, 1->3, 2->3
    // Valid: 0, 1, 2, 3 or 0, 2, 1, 3.
    Graph g(4);
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(1, 3);
    g.add_edge(2, 3);
    
    std::vector<int> order = topological_sort(g);
    ASSERT_EQ(order.size(), 4);
    EXPECT_EQ(order[0], 0);
    EXPECT_EQ(order[3], 3);
}

TEST(ShortestPathTest, TopologicalSortCycle) {
    // 0->1->0
    Graph g(2);
    g.add_edge(0, 1);
    g.add_edge(1, 0);
    
    std::vector<int> order = topological_sort(g);
    EXPECT_TRUE(order.empty());
}

TEST(ShortestPathTest, BidirectionalBFS_Basic) {
    // 0 -- 1 -- 2 -- 3
    ShortestPath sp(4);
    sp.add_edge(0, 1, 1);
    sp.add_edge(1, 0, 1);
    sp.add_edge(1, 2, 1);
    sp.add_edge(2, 1, 1);
    sp.add_edge(2, 3, 1);
    sp.add_edge(3, 2, 1);
    
    auto [dist, path] = sp.bidirectional_bfs(0, 3);
    EXPECT_EQ(dist, 3);
    ASSERT_EQ(path.size(), 4);
    EXPECT_EQ(path[0], 0);
    EXPECT_EQ(path[3], 3);
}

TEST(ShortestPathTest, BidirectionalBFS_SameNode) {
    ShortestPath sp(3);
    sp.add_edge(0, 1, 1);
    sp.add_edge(1, 2, 1);
    
    auto [dist, path] = sp.bidirectional_bfs(1, 1);
    EXPECT_EQ(dist, 0);
    ASSERT_EQ(path.size(), 1);
    EXPECT_EQ(path[0], 1);
}

TEST(ShortestPathTest, BidirectionalBFS_NoPath) {
    ShortestPath sp(3);
    sp.add_edge(0, 1, 1);
    // No path from 0 to 2
    
    auto [dist, path] = sp.bidirectional_bfs(0, 2);
    EXPECT_EQ(dist, -1);
    EXPECT_TRUE(path.empty());
}

TEST(ShortestPathTest, BidirectionalBFS_DirectConnection) {
    ShortestPath sp(2);
    sp.add_edge(0, 1, 1);
    sp.add_edge(1, 0, 1);
    
    auto [dist, path] = sp.bidirectional_bfs(0, 1);
    EXPECT_EQ(dist, 1);
    ASSERT_EQ(path.size(), 2);
    EXPECT_EQ(path[0], 0);
    EXPECT_EQ(path[1], 1);
}
