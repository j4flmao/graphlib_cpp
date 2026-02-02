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
