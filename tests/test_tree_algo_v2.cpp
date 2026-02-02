#include <gtest/gtest.h>
#include "graphlib/graphlib.h"
#include "graphlib/tree.h"
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
    TreeLCA tree(n);
    for(int i=0; i<n-1; ++i) tree.add_edge(i, i+1);
    tree.build(0);

    EXPECT_EQ(tree.lca(0, 4), 0);
    EXPECT_EQ(tree.lca(1, 4), 1);
    EXPECT_EQ(tree.lca(2, 3), 2);
    EXPECT_EQ(tree.lca(3, 3), 3);
    
    EXPECT_EQ(tree.distance(0, 4), 4);
    EXPECT_EQ(tree.distance(1, 4), 3);
    EXPECT_EQ(tree.distance(2, 4), 2);
}

TEST_F(TreeAlgoV2Test, LCABinaryTree) {
    //       0
    //     /   \
    //    1     2
    //   / \   / \
    //  3   4 5   6
    int n = 7;
    TreeLCA tree(n);
    tree.add_edge(0, 1); tree.add_edge(0, 2);
    tree.add_edge(1, 3); tree.add_edge(1, 4);
    tree.add_edge(2, 5); tree.add_edge(2, 6);
    tree.build(0);

    EXPECT_EQ(tree.lca(3, 4), 1);
    EXPECT_EQ(tree.lca(5, 6), 2);
    EXPECT_EQ(tree.lca(3, 6), 0);
    EXPECT_EQ(tree.lca(1, 5), 0);
    EXPECT_EQ(tree.lca(3, 1), 1);

    EXPECT_EQ(tree.kth_ancestor(3, 1), 1);
    EXPECT_EQ(tree.kth_ancestor(3, 2), 0);
    EXPECT_EQ(tree.kth_ancestor(6, 1), 2);
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
    TreeLCA tree(n);
    tree.add_edge(0, 1);
    tree.add_edge(1, 2);
    tree.add_edge(1, 3);
    tree.add_edge(3, 4);
    tree.add_edge(3, 5);
    tree.build(0);

    // Check LCA through HLD
    EXPECT_EQ(tree.lca(2, 4), 1);
    EXPECT_EQ(tree.lca(4, 5), 3);

    // Check Path Intervals
    // Path 2 -> 5: 2->1->3->5
    // Segments depend on heavy edges. 
    // Let's verify that the union of intervals has size equal to path length (number of vertices).
    std::vector<std::pair<int, int>> intervals;
    tree.hld_decompose_path(2, 5, intervals);
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
    TreeLCA tree(n);
    tree.add_edge(0, 1); tree.add_edge(0, 2);
    tree.add_edge(1, 3); tree.add_edge(1, 4);
    tree.build(0);

    // Subtree of 1 has size 3 (1, 3, 4)
    EXPECT_EQ(tree.subtree_size(1), 3);
    
    // 3 and 4 should be in positions after 1 in HLD order
    int pos1 = tree.hld_pos(1);
    int pos3 = tree.hld_pos(3);
    int pos4 = tree.hld_pos(4);
    
    // In HLD, subtree forms a contiguous interval starting at pos(v)
    // Interval is [pos(v), pos(v) + subtree_size(v) - 1]
    int interval_start = pos1;
    int interval_end = pos1 + tree.subtree_size(1) - 1;
    
    EXPECT_GE(pos3, interval_start);
    EXPECT_LE(pos3, interval_end);
    EXPECT_GE(pos4, interval_start);
    EXPECT_LE(pos4, interval_end);
}

TEST_F(TreeAlgoV2Test, RandomStressLCA) {
    int n = 100;
    TreeLCA tree(n);
    
    // Generate random tree: for i=1..n-1, connect i to rand(0..i-1)
    for (int i = 1; i < n; ++i) {
        int p = rand() % i;
        tree.add_edge(i, p);
    }
    tree.build(0);

    for (int iter = 0; iter < 1000; ++iter) {
        int u = rand() % n;
        int v = rand() % n;

        int lca_result = tree.lca(u, v);
        
        // Verify LCA is ancestor of both
        EXPECT_TRUE(tree.is_ancestor(lca_result, u)) << "LCA not ancestor of u=" << u;
        EXPECT_TRUE(tree.is_ancestor(lca_result, v)) << "LCA not ancestor of v=" << v;

        // Verify distance consistency
        int dist = tree.distance(u, v);
        int expected_dist = tree.depth(u) + tree.depth(v) - 2 * tree.depth(lca_result);
        EXPECT_EQ(dist, expected_dist) << "Dist mismatch for u=" << u << ", v=" << v;
    }
}
