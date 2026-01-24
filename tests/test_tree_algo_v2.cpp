#include <gtest/gtest.h>
#include "graphlib/graphlib.h"
#include "graphlib/tree_algo.h"
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace graphlib;

class TreeAlgoV2Test : public ::testing::Test {
protected:
    void SetUp() override {
        srand(42);
    }
};

TEST_F(TreeAlgoV2Test, LCALineGraph) {
    // 0-1-2-3-4
    int n = 5;
    Graph g(n, false);
    for(int i=0; i<n-1; ++i) g.add_edge(i, i+1);

    LCA lca_solver(g, 0);

    EXPECT_EQ(lca_solver.query(0, 4), 0);
    EXPECT_EQ(lca_solver.query(1, 4), 1);
    EXPECT_EQ(lca_solver.query(2, 3), 2);
    EXPECT_EQ(lca_solver.query(3, 3), 3);
    
    EXPECT_EQ(lca_solver.dist(0, 4), 4);
    EXPECT_EQ(lca_solver.dist(1, 4), 3);
    EXPECT_EQ(lca_solver.dist(2, 4), 2);
}

TEST_F(TreeAlgoV2Test, LCABinaryTree) {
    //       0
    //     /   \
    //    1     2
    //   / \   / \
    //  3   4 5   6
    int n = 7;
    Graph g(n, false);
    g.add_edge(0, 1); g.add_edge(0, 2);
    g.add_edge(1, 3); g.add_edge(1, 4);
    g.add_edge(2, 5); g.add_edge(2, 6);

    LCA lca_solver(g, 0);

    EXPECT_EQ(lca_solver.query(3, 4), 1);
    EXPECT_EQ(lca_solver.query(5, 6), 2);
    EXPECT_EQ(lca_solver.query(3, 6), 0);
    EXPECT_EQ(lca_solver.query(1, 5), 0);
    EXPECT_EQ(lca_solver.query(3, 1), 1);

    EXPECT_EQ(lca_solver.kth_ancestor(3, 1), 1);
    EXPECT_EQ(lca_solver.kth_ancestor(3, 2), 0);
    EXPECT_EQ(lca_solver.kth_ancestor(6, 1), 2);
}

TEST_F(TreeAlgoV2Test, HLDPathIntervals) {
    //       0
    //      / 
    //     1   
    //    / \  
    //   2   3 
    //      / \
    //     4   5
    int n = 6;
    Graph g(n, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(1, 3);
    g.add_edge(3, 4);
    g.add_edge(3, 5);

    HLD hld_solver(g, 0);

    // Check LCA through HLD
    EXPECT_EQ(hld_solver.lca(2, 4), 1);
    EXPECT_EQ(hld_solver.lca(4, 5), 3);

    // Check Path Intervals
    // Path 2 -> 5: 2->1->3->5
    // Segments depend on heavy edges. 
    // Let's verify that the union of intervals has size equal to path length (number of vertices).
    auto intervals = hld_solver.get_path_intervals(2, 5);
    int total_len = 0;
    for (auto p : intervals) {
        total_len += (p.second - p.first + 1); // Intervals are inclusive [l, r]
    }
    // Path: 2, 1, 3, 5 -> 4 vertices
    EXPECT_EQ(total_len, 4);
}

TEST_F(TreeAlgoV2Test, HLDSubtree) {
    //       0
    //     /   \
    //    1     2
    //   / \   
    //  3   4 
    int n = 5;
    Graph g(n, false);
    g.add_edge(0, 1); g.add_edge(0, 2);
    g.add_edge(1, 3); g.add_edge(1, 4);

    HLD hld_solver(g, 0);

    auto interval = hld_solver.get_subtree_interval(1);
    // Subtree of 1 has size 3 (1, 3, 4)
    EXPECT_EQ(interval.second - interval.first + 1, 3);
    
    // 3 and 4 should be inside the interval
    int pos3 = hld_solver.get_pos(3);
    int pos4 = hld_solver.get_pos(4);
    
    EXPECT_GE(pos3, interval.first);
    EXPECT_LE(pos3, interval.second);
    EXPECT_GE(pos4, interval.first);
    EXPECT_LE(pos4, interval.second);
}

TEST_F(TreeAlgoV2Test, RandomStressLCAvsHLD) {
    int n = 100;
    Graph g(n, false);
    
    // Generate random tree: for i=1..n-1, connect i to rand(0..i-1)
    for (int i = 1; i < n; ++i) {
        int p = rand() % i;
        g.add_edge(i, p);
    }

    LCA lca_solver(g, 0);
    HLD hld_solver(g, 0);

    for (int iter = 0; iter < 1000; ++iter) {
        int u = rand() % n;
        int v = rand() % n;

        int lca1 = lca_solver.query(u, v);
        int lca2 = hld_solver.lca(u, v);

        EXPECT_EQ(lca1, lca2) << "LCA mismatch for u=" << u << ", v=" << v;

        int dist1 = lca_solver.dist(u, v);
        int dist2 = hld_solver.dist(u, v);

        EXPECT_EQ(dist1, dist2) << "Dist mismatch for u=" << u << ", v=" << v;
    }
}
