#include <gtest/gtest.h>
#include "graphlib/np_hard.h"
#include <cmath>

using namespace graphlib;

TEST(TSPTest, ExactSmall4) {
    // Square 2x1
    Graph g(4);
    g.add_edge(0, 1, 1); g.add_edge(1, 0, 1);
    g.add_edge(1, 2, 2); g.add_edge(2, 1, 2);
    g.add_edge(2, 3, 1); g.add_edge(3, 2, 1);
    g.add_edge(3, 0, 2); g.add_edge(0, 3, 2);
    // Diagonals for completeness (Metric)
    // sqrt(1^2 + 2^2) = 2.23 -> 2 or 3 integer?
    // Let's make it integer compatible 3-4-5 triangle?
    // 0(0,0), 1(0,3), 2(4,3), 3(4,0)
    // 0-1: 3
    // 1-2: 4
    // 2-3: 3
    // 3-0: 4
    // 0-2: 5
    // 1-3: 5
    
    // Clear graph
    Graph grid(4);
    grid.add_edge(0, 1, 3); grid.add_edge(1, 0, 3);
    grid.add_edge(1, 2, 4); grid.add_edge(2, 1, 4);
    grid.add_edge(2, 3, 3); grid.add_edge(3, 2, 3);
    grid.add_edge(3, 0, 4); grid.add_edge(0, 3, 4);
    grid.add_edge(0, 2, 5); grid.add_edge(2, 0, 5);
    grid.add_edge(1, 3, 5); grid.add_edge(3, 1, 5);
    
    // Optimal: 3+4+3+4 = 14
    long long cost = tsp_bitmask(grid);
    EXPECT_EQ(cost, 14);
}

TEST(TSPTest, ChristofidesOptimality) {
    // 3-4-5 Rectangle
    Graph grid(4);
    // Complete graph required for Christofides (Metric)
    grid.add_edge(0, 1, 3); grid.add_edge(1, 0, 3);
    grid.add_edge(1, 2, 4); grid.add_edge(2, 1, 4);
    grid.add_edge(2, 3, 3); grid.add_edge(3, 2, 3);
    grid.add_edge(3, 0, 4); grid.add_edge(0, 3, 4);
    grid.add_edge(0, 2, 5); grid.add_edge(2, 0, 5);
    grid.add_edge(1, 3, 5); grid.add_edge(3, 1, 5);
    
    auto res = tsp_christofides(grid);
    EXPECT_EQ(res.first, 14);
    EXPECT_EQ(res.second.size(), 4);
}

TEST(TSPTest, BitmaskDisconnect) {
    Graph g(3);
    g.add_edge(0, 1, 1);
    // 2 is isolated
    EXPECT_EQ(tsp_bitmask(g), -1);
}

TEST(ChinesePostmanTest, Triangle) {
    // Triangle: All degrees 2 (Even). Already Eulerian.
    // Total weight = 1+1+1 = 3.
    // Result should be 3.
    Graph g(3);
    g.add_edge(0, 1, 1); g.add_edge(1, 0, 1);
    g.add_edge(1, 2, 1); g.add_edge(2, 1, 1);
    g.add_edge(2, 0, 1); g.add_edge(0, 2, 1);
    
    EXPECT_EQ(chinese_postman(g), 3);
}

TEST(ChinesePostmanTest, Path) {
    // 0-1-2-3 (Weights 1, 1, 1)
    // Degrees: 0(1), 1(2), 2(2), 3(1).
    // Odds: 0, 3.
    // Distance 0-3 is 3.
    // Must double path 0-3.
    // Total edges weight = 3.
    // Added path weight = 3.
    // Result = 6.
    // Walk: 0-1-2-3-2-1-0
    Graph g(4);
    g.add_edge(0, 1, 1); g.add_edge(1, 0, 1);
    g.add_edge(1, 2, 1); g.add_edge(2, 1, 1);
    g.add_edge(2, 3, 1); g.add_edge(3, 2, 1);
    
    EXPECT_EQ(chinese_postman(g), 6);
}
