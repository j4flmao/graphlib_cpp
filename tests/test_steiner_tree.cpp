#include <gtest/gtest.h>
#include "graphlib/steiner_tree.h"

using namespace graphlib;

TEST(SteinerTreeTest, BasicStar) {
    // 0 is center. 1, 2, 3 are leaves connected to 0 with weight 1.
    // Terminals: 1, 2, 3.
    // MST of terminals using 0 is edges (0,1), (0,2), (0,3). Total weight 3.
    // If we just used edges between 1-2, 2-3 etc (if they existed) it might differ.
    // Here only path is through 0.
    Graph g(4);
    g.add_edge(0, 1, 1); g.add_edge(1, 0, 1);
    g.add_edge(0, 2, 1); g.add_edge(2, 0, 1);
    g.add_edge(0, 3, 1); g.add_edge(3, 0, 1);
    
    std::vector<int> terminals = {1, 2, 3};
    long long cost = steiner_tree(g, terminals);
    EXPECT_EQ(cost, 3);
}

TEST(SteinerTreeTest, Path) {
    // 0-1-2-3-4. Weights 1.
    // Terminals {0, 4}.
    // Cost should be 4.
    Graph g(5);
    g.add_edge(0, 1, 1); g.add_edge(1, 0, 1);
    g.add_edge(1, 2, 1); g.add_edge(2, 1, 1);
    g.add_edge(2, 3, 1); g.add_edge(3, 2, 1);
    g.add_edge(3, 4, 1); g.add_edge(4, 3, 1);
    
    std::vector<int> terminals = {0, 4};
    long long cost = steiner_tree(g, terminals);
    EXPECT_EQ(cost, 4);
    
    // Add terminal 2. Cost same (2 is on path).
    terminals = {0, 2, 4};
    cost = steiner_tree(g, terminals);
    EXPECT_EQ(cost, 4);
}

TEST(SteinerTreeTest, TriangleMetric) {
    // 0, 1, 2. All connected weight 2.
    // Terminals {0, 1, 2}.
    // MST is 2 edges of weight 2 = 4.
    Graph g(3);
    g.add_edge(0, 1, 2); g.add_edge(1, 0, 2);
    g.add_edge(1, 2, 2); g.add_edge(2, 1, 2);
    g.add_edge(2, 0, 2); g.add_edge(0, 2, 2);
    
    long long cost = steiner_tree(g, {0, 1, 2});
    EXPECT_EQ(cost, 4);
}
