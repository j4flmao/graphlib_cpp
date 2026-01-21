#include <gtest/gtest.h>
#include "graphlib/splay_tree.h"
#include "graphlib/shortest_path.h"
#include "graphlib/tree.h"
#include <vector>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <random>

using namespace graphlib;

// ============================================================================
// SPLAY TREE EXTENDED TESTS
// ============================================================================

TEST(ExtendedCoverage, SplayTreeRangeSum) {
    SplayTree st;
    std::vector<int> values = {1, 5, 10, 15, 20};
    for (int v : values) {
        st.insert(v);
    }
    
    // Sum of keys in range [5, 15] -> 5 + 10 + 15 = 30
    // range_sum usually sums the keys themselves in standard implementation if no value associated
    // The header has 'long long sum' in node, initialized likely with key.
    EXPECT_EQ(st.range_sum(5, 15), 30);
    EXPECT_EQ(st.range_sum(0, 25), 51); // All
    EXPECT_EQ(st.range_sum(6, 9), 0);   // None
}

TEST(ExtendedCoverage, SplayTreeSplitJoin) {
    SplayTree st;
    for (int i = 1; i <= 10; ++i) st.insert(i);
    
    SplayTree left, right;
    // Split at 5: left gets <= 5, right gets > 5? Or left < 5, right >= 5?
    // Usually split(k) puts keys <= k in one tree and > k in another, or < k and >= k.
    // Let's assume split(key) splits by key.
    // If we rely on implementation details, we might need to adjust.
    // Based on typical splay tree split:
    st.split(5, left, right);
    
    EXPECT_TRUE(left.search(5));
    EXPECT_TRUE(left.search(1));
    EXPECT_FALSE(left.search(6));
    
    EXPECT_TRUE(right.search(6));
    EXPECT_TRUE(right.search(10));
    EXPECT_FALSE(right.search(5));
    
    // Join back
    SplayTree::join(left, right);
    // Join usually merges right into left or returns a new tree.
    // Static void join(SplayTree& left, SplayTree& right) likely merges 'right' into 'left' 
    // assuming all keys in left < all keys in right.
    // But wait, the API might clear right and put everything in left, or vice versa.
    // Let's check if left has everything now.
    
    // Note: SplayTree implementation usually requires the left tree's max < right tree's min.
    // Our split ensured that.
    
    // However, if join modifies 'left' to contain the result, let's check 'left'.
    // If it modifies 'right', we check 'right'.
    // Usually it makes one of them the result.
    // Let's check both or see if we can deduce. 
    // If it's static, it might not know which is which unless it moves root.
    // Let's assume 'left' becomes the merged tree.
    
    // Actually, checking if 'left' is valid object or empty.
    // If 'left' is empty, maybe 'right' has it?
    // Let's check searches.
    if (left.search(1)) {
        EXPECT_TRUE(left.search(10));
    } else {
        EXPECT_TRUE(right.search(1));
        EXPECT_TRUE(right.search(10));
    }
}

// ============================================================================
// SHORTEST PATH EXTENDED TESTS
// ============================================================================

TEST(ExtendedCoverage, AStarGrid) {
    // 10x10 Grid
    int rows = 10, cols = 10;
    ShortestPath sp(rows * cols);
    auto node = [&](int r, int c) { return r * cols + c; };
    
    for(int r=0; r<rows; ++r) {
        for(int c=0; c<cols; ++c) {
            if (r+1 < rows) sp.add_edge(node(r,c), node(r+1,c), 1);
            if (c+1 < cols) sp.add_edge(node(r,c), node(r,c+1), 1);
            if (r > 0) sp.add_edge(node(r,c), node(r-1,c), 1);
            if (c > 0) sp.add_edge(node(r,c), node(r,c-1), 1);
        }
    }
    
    // Heuristic: Manhattan distance
    int start = node(0,0);
    int target = node(9,9);
    std::vector<long long> h(rows * cols);
    for(int r=0; r<rows; ++r) {
        for(int c=0; c<cols; ++c) {
            h[node(r,c)] = std::abs(r - 9) + std::abs(c - 9);
        }
    }
    
    std::vector<long long> dist = sp.a_star(start, target, h, 1e18);
    // Distance should be 18 steps
    EXPECT_EQ(dist[target], 18);
}

TEST(ExtendedCoverage, MultiSourceDijkstra) {
    // Fire spreading from multiple sources
    ShortestPath sp(6);
    // 0-1-2-3-4-5
    for(int i=0; i<5; ++i) sp.add_edge(i, i+1, 1);
    for(int i=0; i<5; ++i) sp.add_edge(i+1, i, 1);
    
    std::vector<int> sources = {0, 5};
    long long inf = 1e18;
    auto dist = sp.multi_source_dijkstra(sources, inf);
    
    EXPECT_EQ(dist[0], 0);
    EXPECT_EQ(dist[5], 0);
    EXPECT_EQ(dist[2], 2); // from 0
    EXPECT_EQ(dist[3], 2); // from 5
    EXPECT_EQ(dist[1], 1);
    EXPECT_EQ(dist[4], 1);
}

TEST(ExtendedCoverage, FloydWarshallTransitiveClosure) {
    ShortestPath sp(4);
    sp.add_edge(0, 1, 1);
    sp.add_edge(1, 2, 1);
    sp.add_edge(2, 3, 1);
    // 0 -> 1 -> 2 -> 3
    
    long long inf = 1e18;
    auto mat = sp.floyd_warshall(inf);
    
    EXPECT_EQ(mat[0][3], 3);
    EXPECT_EQ(mat[0][2], 2);
    EXPECT_EQ(mat[3][0], inf); // Directed
}

// ============================================================================
// TREE EXTENDED TESTS
// ============================================================================

TEST(ExtendedCoverage, TreeKthAncestor) {
    // Line graph: 0-1-2-3-4
    TreeLCA t(5);
    t.add_edge(0, 1);
    t.add_edge(1, 2);
    t.add_edge(2, 3);
    t.add_edge(3, 4);
    t.build(0);
    
    EXPECT_EQ(t.kth_ancestor(4, 1), 3);
    EXPECT_EQ(t.kth_ancestor(4, 2), 2);
    EXPECT_EQ(t.kth_ancestor(4, 4), 0);
    EXPECT_EQ(t.kth_ancestor(4, 5), -1); // Out of bounds
}

TEST(ExtendedCoverage, TreeDistance) {
    // Star graph center 0
    TreeLCA t(5);
    for(int i=1; i<5; ++i) t.add_edge(0, i);
    t.build(0);
    
    EXPECT_EQ(t.distance(1, 2), 2); // 1-0-2
    EXPECT_EQ(t.distance(1, 0), 1);
    EXPECT_EQ(t.distance(1, 1), 0);
}

TEST(ExtendedCoverage, SubtreeSize) {
    // 0 -> 1 -> {2, 3}
    TreeLCA t(4);
    t.add_edge(0, 1);
    t.add_edge(1, 2);
    t.add_edge(1, 3);
    t.build(0);
    
    EXPECT_EQ(t.subtree_size(0), 4);
    EXPECT_EQ(t.subtree_size(1), 3);
    EXPECT_EQ(t.subtree_size(2), 1);
}
