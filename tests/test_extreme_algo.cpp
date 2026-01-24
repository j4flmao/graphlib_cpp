#include <gtest/gtest.h>
#include "graphlib/graphlib.h"
#include <vector>
#include <algorithm>

using namespace graphlib;

// -----------------------------------------------------------------------------
// Tree Center & Isomorphism Tests
// -----------------------------------------------------------------------------

TEST(TreeAlgoTest, TreeCenterLine) {
    // 0-1-2-3-4
    // Center should be {2}
    Graph g(5, false);
    g.add_edge(0, 1); g.add_edge(1, 2); g.add_edge(2, 3); g.add_edge(3, 4);
    
    std::vector<int> centers = tree_center(g);
    ASSERT_EQ(centers.size(), 1);
    EXPECT_EQ(centers[0], 2);
}

TEST(TreeAlgoTest, TreeCenterLineEven) {
    // 0-1-2-3
    // Centers should be {1, 2}
    Graph g(4, false);
    g.add_edge(0, 1); g.add_edge(1, 2); g.add_edge(2, 3);
    
    std::vector<int> centers = tree_center(g);
    ASSERT_EQ(centers.size(), 2);
    // Sort to compare set
    std::sort(centers.begin(), centers.end());
    EXPECT_EQ(centers[0], 1);
    EXPECT_EQ(centers[1], 2);
}

TEST(TreeAlgoTest, IsomorphismRooted) {
    // Tree 1: 0 -> (1, 2)
    Graph t1(3, false);
    t1.add_edge(0, 1); t1.add_edge(0, 2);

    // Tree 2: 1 -> (0, 2) (same structure, different root indices)
    Graph t2(3, false);
    t2.add_edge(1, 0); t2.add_edge(1, 2);

    // Rooted at 0 in t1 vs Rooted at 1 in t2
    EXPECT_TRUE(are_trees_isomorphic(t1, 0, t2, 1));
    
    // Rooted at 1 in t1 vs Rooted at 1 in t2 (Different structure: 1 is leaf in t1)
    EXPECT_FALSE(are_trees_isomorphic(t1, 1, t2, 1));
}

TEST(TreeAlgoTest, IsomorphismUnrooted) {
    // Star graph with 3 leaves
    Graph t1(4, false);
    t1.add_edge(0, 1); t1.add_edge(0, 2); t1.add_edge(0, 3);

    // Same star graph but permuted indices
    // 3 is center, 0,1,2 are leaves
    Graph t2(4, false);
    t2.add_edge(3, 0); t2.add_edge(3, 1); t2.add_edge(3, 2);

    EXPECT_TRUE(are_unrooted_trees_isomorphic(t1, t2));

    // Line graph 0-1-2-3
    Graph t3(4, false);
    t3.add_edge(0, 1); t3.add_edge(1, 2); t3.add_edge(2, 3);

    EXPECT_FALSE(are_unrooted_trees_isomorphic(t1, t3));
}

// -----------------------------------------------------------------------------
// Steiner Tree Tests
// -----------------------------------------------------------------------------

TEST(SteinerTreeTest, Triangle) {
    // Triangle 0-1 (10), 1-2 (10), 2-0 (10)
    // Terminals {0, 1, 2}
    // Min tree is 2 edges, cost 20.
    Graph g(3, false);
    g.add_edge(0, 1, 10);
    g.add_edge(1, 2, 10);
    g.add_edge(2, 0, 10);

    EXPECT_EQ(steiner_tree(g, {0, 1, 2}), 20);
}

TEST(SteinerTreeTest, StarGraph) {
    // 0 is center. Leaves 1, 2, 3.
    // Terminals {1, 2, 3}.
    // Best path: 1-0, 2-0, 3-0. Cost 1+1+1 = 3.
    Graph g(4, false);
    g.add_edge(0, 1, 1);
    g.add_edge(0, 2, 1);
    g.add_edge(0, 3, 1);

    EXPECT_EQ(steiner_tree(g, {1, 2, 3}), 3);
}

TEST(SteinerTreeTest, IntermediateNodeRequired) {
    // 1 --(10)-- 2
    // |          |
    // (5)       (5)
    // |          |
    // 0 --(2)--- 3
    // 
    // Terminals {1, 2}.
    // Direct edge 1-2 is 10.
    // Path 1-0-3-2 is 5+2+5 = 12.
    // Wait, let's make it better to use intermediate.
    // 1 --(20)-- 2
    // 1 --(5)-- 0 --(5)-- 2
    // Terminals {1, 2}. Steiner tree should be 10.
    
    Graph g(3, false);
    g.add_edge(1, 2, 20);
    g.add_edge(1, 0, 5);
    g.add_edge(0, 2, 5);
    
    EXPECT_EQ(steiner_tree(g, {1, 2}), 10);
}

TEST(SteinerTreeTest, SingleTerminal) {
    Graph g(2, false);
    g.add_edge(0, 1, 10);
    EXPECT_EQ(steiner_tree(g, {0}), 0);
}

TEST(SteinerTreeTest, TwoTerminalsIsShortestPath) {
    Graph g(4, false);
    g.add_edge(0, 1, 1);
    g.add_edge(1, 2, 2);
    g.add_edge(2, 3, 3);
    g.add_edge(0, 3, 100);

    // 0 -> 3 shortest path is 0-1-2-3 cost 6.
    EXPECT_EQ(steiner_tree(g, {0, 3}), 6);
}

TEST(SteinerTreeTest, Disconnected) {
    Graph g(4, false);
    g.add_edge(0, 1, 1);
    g.add_edge(2, 3, 1);
    
    // 0 and 2 are disconnected
    long long res = steiner_tree(g, {0, 2});
    EXPECT_EQ(res, -1); 
}
