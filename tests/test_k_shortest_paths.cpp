#include <gtest/gtest.h>
#include "graphlib/k_shortest_paths.h"
#include "graphlib/graph_core.h"
#include <iostream>

using namespace graphlib;

class KShortestPathsComprehensiveTest : public ::testing::Test {
protected:
    Graph g;

    void SetUp() override {
        // Create a simple graph
        // 0 -> 1 (4)
        // 0 -> 2 (2)
        // 1 -> 3 (1)
        // 2 -> 1 (1)
        // 2 -> 3 (5)
        // 2 -> 4 (3)
        // 4 -> 3 (1)
        g = Graph(5, true);
        g.add_edge(0, 1, 4);
        g.add_edge(0, 2, 2);
        g.add_edge(1, 3, 1);
        g.add_edge(2, 1, 1);
        g.add_edge(2, 3, 5);
        g.add_edge(2, 4, 3);
        g.add_edge(4, 3, 1);
    }
};

TEST_F(KShortestPathsComprehensiveTest, FindsCorrectPaths) {
    // Paths from 0 to 3:
    // 1. 0 -> 2 -> 1 -> 3 (Cost: 2 + 1 + 1 = 4)
    // 2. 0 -> 1 -> 3 (Cost: 4 + 1 = 5)
    // 3. 0 -> 2 -> 4 -> 3 (Cost: 2 + 3 + 1 = 6)
    // 4. 0 -> 2 -> 3 (Cost: 2 + 5 = 7)

    auto paths = yen_k_shortest_paths(g, 0, 3, 3);
    
    ASSERT_EQ(paths.size(), 3);

    // Path 1
    std::vector<int> p1 = {0, 2, 1, 3};
    EXPECT_EQ(paths[0], p1);

    // Path 2
    std::vector<int> p2 = {0, 1, 3};
    EXPECT_EQ(paths[1], p2);

    // Path 3
    std::vector<int> p3 = {0, 2, 4, 3};
    EXPECT_EQ(paths[2], p3);
}

TEST_F(KShortestPathsComprehensiveTest, FewerThanKPaths) {
    // Only 4 paths exist. Ask for 10.
    auto paths = yen_k_shortest_paths(g, 0, 3, 10);
    ASSERT_EQ(paths.size(), 4);
}

TEST_F(KShortestPathsComprehensiveTest, NoPath) {
    Graph disconnected(3, true);
    disconnected.add_edge(0, 1, 1);
    auto paths = yen_k_shortest_paths(disconnected, 0, 2, 3);
    EXPECT_TRUE(paths.empty());
}

TEST_F(KShortestPathsComprehensiveTest, SinglePath) {
    Graph linear(3, true);
    linear.add_edge(0, 1, 1);
    linear.add_edge(1, 2, 1);
    auto paths = yen_k_shortest_paths(linear, 0, 2, 5);
    ASSERT_EQ(paths.size(), 1);
    std::vector<int> p = {0, 1, 2};
    EXPECT_EQ(paths[0], p);
}
