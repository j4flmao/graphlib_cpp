#include <gtest/gtest.h>
#include "graphlib/graph_core.h"
#include "graphlib/min_cut.h"
#include "graphlib/general_matching.h"
#include "graphlib/tree.h"
#include "graphlib/splay_tree.h"
#include <vector>
#include <algorithm>
#include <random>

using namespace graphlib;

// -----------------------------------------------------------------------------
// General Matching Tests (Blossom Algorithm)
// -----------------------------------------------------------------------------

TEST(AdvancedAlgorithms2, GeneralMatchingTriangle) {
    // Triangle (C3): Max matching size 1
    GeneralMatching g(3);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);
    EXPECT_EQ(g.maximum_matching(), 1);
}

TEST(AdvancedAlgorithms2, GeneralMatchingSquare) {
    // Square (C4): Max matching size 2
    GeneralMatching g(4);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(3, 0);
    EXPECT_EQ(g.maximum_matching(), 2);
}

TEST(AdvancedAlgorithms2, GeneralMatchingPentagon) {
    // Pentagon (C5): Max matching size 2
    GeneralMatching g(5);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(3, 4);
    g.add_edge(4, 0);
    EXPECT_EQ(g.maximum_matching(), 2);
}

TEST(AdvancedAlgorithms2, GeneralMatchingK4) {
    // Complete Graph K4: Max matching size 2
    GeneralMatching g(4);
    g.add_edge(0, 1); g.add_edge(0, 2); g.add_edge(0, 3);
    g.add_edge(1, 2); g.add_edge(1, 3);
    g.add_edge(2, 3);
    EXPECT_EQ(g.maximum_matching(), 2);
}

TEST(AdvancedAlgorithms2, GeneralMatchingPetersen) {
    // Petersen Graph: 10 vertices, 3-regular, perfect matching size 5
    GeneralMatching g(10);
    // Outer cycle (0-4)
    g.add_edge(0, 1); g.add_edge(1, 2); g.add_edge(2, 3); g.add_edge(3, 4); g.add_edge(4, 0);
    // Inner star (5-9): 5-7, 7-9, 9-6, 6-8, 8-5
    g.add_edge(5, 7); g.add_edge(7, 9); g.add_edge(9, 6); g.add_edge(6, 8); g.add_edge(8, 5);
    // Spokes
    g.add_edge(0, 5); g.add_edge(1, 6); g.add_edge(2, 7); g.add_edge(3, 8); g.add_edge(4, 9);

    EXPECT_EQ(g.maximum_matching(), 5);
}

// -----------------------------------------------------------------------------
// Global Min Cut Tests (Stoer-Wagner)
// -----------------------------------------------------------------------------

TEST(AdvancedAlgorithms2, GlobalMinCutDisconnected) {
    Graph g(4, false);
    g.add_edge(0, 1, 10);
    g.add_edge(2, 3, 10);
    // 0-1 and 2-3 are disconnected components. Min cut should be 0.
    EXPECT_EQ(global_min_cut_undirected(g), 0);
}

TEST(AdvancedAlgorithms2, GlobalMinCutClique) {
    // K4 with weight 1.
    // Min cut separates 1 vertex from the rest.
    // Degree of each vertex is 3. Cut value = 3.
    int n = 4;
    Graph g(n, false);
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            g.add_edge(i, j, 1);
        }
    }
    EXPECT_EQ(global_min_cut_undirected(g), 3);
}

TEST(AdvancedAlgorithms2, GlobalMinCutBridge) {
    // Two K3s connected by a bridge of weight 2.
    // K3 internal edges weight 10.
    // Bridge (0, 3) weight 2.
    // 0-1, 1-2, 2-0 (clique 1)
    // 3-4, 4-5, 5-3 (clique 2)
    Graph g(6, false);
    // Clique 1
    g.add_edge(0, 1, 10); g.add_edge(1, 2, 10); g.add_edge(2, 0, 10);
    // Clique 2
    g.add_edge(3, 4, 10); g.add_edge(4, 5, 10); g.add_edge(5, 3, 10);
    // Bridge
    g.add_edge(0, 3, 2);

    EXPECT_EQ(global_min_cut_undirected(g), 2);
}

// -----------------------------------------------------------------------------
// Splay Tree Corner Cases
// -----------------------------------------------------------------------------

TEST(AdvancedAlgorithms2, SplayTreeEmpty) {
    // Test operations on empty or null trees if applicable via API
    SplayTree st;
    // Insert some
    st.insert(10);
    st.insert(20);
    
    // Find non-existent
    EXPECT_FALSE(st.search(30));
    
    // Erase non-existent
    st.remove(30); // Should be safe
    EXPECT_TRUE(st.search(20));
    
    st.remove(10);
    EXPECT_TRUE(st.search(20));
    EXPECT_FALSE(st.search(10));
    
    st.remove(20);
    EXPECT_FALSE(st.search(20));
}

// -----------------------------------------------------------------------------
// Tree LCA Advanced
// -----------------------------------------------------------------------------

TEST(AdvancedAlgorithms2, TreeLCALinear) {
    // 0-1-2-3-4...-99
    int n = 100;
    TreeLCA t(n);
    for (int i = 0; i < n - 1; ++i) {
        t.add_edge(i, i + 1);
    }
    t.build(0);

    EXPECT_EQ(t.lca(0, 99), 0);
    EXPECT_EQ(t.lca(50, 60), 50);
    EXPECT_EQ(t.distance(0, 99), 99);
    EXPECT_EQ(t.distance(10, 20), 10);
}

TEST(AdvancedAlgorithms2, TreeLCAStar) {
    // Center 0, leaves 1..99
    int n = 100;
    TreeLCA t(n);
    for (int i = 1; i < n; ++i) {
        t.add_edge(0, i);
    }
    t.build(0);

    EXPECT_EQ(t.lca(1, 2), 0);
    EXPECT_EQ(t.lca(1, 99), 0);
    EXPECT_EQ(t.distance(1, 2), 2); // 1-0-2
    EXPECT_EQ(t.distance(0, 50), 1);
}

TEST(AdvancedAlgorithms2, TreeLCABinaryTree) {
    // Complete binary tree structure
    // 0 -> 1, 2
    // 1 -> 3, 4
    // 2 -> 5, 6
    int n = 7;
    TreeLCA t(n);
    t.add_edge(0, 1); t.add_edge(0, 2);
    t.add_edge(1, 3); t.add_edge(1, 4);
    t.add_edge(2, 5); t.add_edge(2, 6);
    t.build(0);

    EXPECT_EQ(t.lca(3, 4), 1);
    EXPECT_EQ(t.lca(5, 6), 2);
    EXPECT_EQ(t.lca(3, 5), 0);
    EXPECT_EQ(t.lca(3, 6), 0);
    EXPECT_EQ(t.distance(3, 6), 4); // 3-1-0-2-6
}
