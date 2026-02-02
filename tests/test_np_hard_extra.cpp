#include <gtest/gtest.h>
#include "graphlib/np_hard.h"
#include <numeric>

using namespace graphlib;

TEST(ApproxTest, MaxCutCycle4) {
    // C4: 0-1-2-3-0
    // Max cut is 4 edges (bipartite: {0,2} and {1,3}).
    // Approx might return 2 (greedy worst case?) or 4.
    // Random/Greedy usually good on C4.
    Graph g(4);
    g.add_edge(0, 1, 1); g.add_edge(1, 0, 1);
    g.add_edge(1, 2, 1); g.add_edge(2, 1, 1);
    g.add_edge(2, 3, 1); g.add_edge(3, 2, 1);
    g.add_edge(3, 0, 1); g.add_edge(0, 3, 1);
    
    auto res = max_cut_approx(g);
    // Should be at least total_weight / 2 = 2.
    // Ideally 4.
    EXPECT_GE(res.first, 2);
    // On bipartite, greedy often finds optimal.
    EXPECT_EQ(res.first, 4);
}

TEST(ApproxTest, MaxCutTriangle) {
    // Triangle. Max cut is 2. (Any partition is 1 vs 2 nodes, 2 edges cut, 1 internal).
    Graph g(3);
    g.add_edge(0, 1, 1); g.add_edge(1, 0, 1);
    g.add_edge(1, 2, 1); g.add_edge(2, 1, 1);
    g.add_edge(2, 0, 1); g.add_edge(0, 2, 1);
    
    auto res = max_cut_approx(g);
    EXPECT_GE(res.first, 2); 
    // Expect exactly 2
    EXPECT_EQ(res.first, 2);
}

TEST(ApproxTest, FVS_Triangle) {
    // Triangle. Removing any 1 node breaks cycle.
    Graph g(3);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    
    auto fvs = feedback_vertex_set_approx(g);
    EXPECT_EQ(fvs.size(), 1);
    // Should be valid FVS
    // (Check if acyclic after removal - done implicitly by algorithm until acyclic)
}

TEST(ApproxTest, FVS_TwoTrianglesSharingVertex) {
    // 0-1, 1-2, 2-0
    // 2-3, 3-4, 4-2
    // Removing 2 breaks both. Optimal FVS size 1.
    // Greedy picking max degree in cycle should optimaly pick 2.
    Graph g(5);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    
    g.add_edge(2, 3); g.add_edge(3, 2);
    g.add_edge(3, 4); g.add_edge(4, 3);
    g.add_edge(4, 2); g.add_edge(2, 4);
    
    auto fvs = feedback_vertex_set_approx(g);
    EXPECT_EQ(fvs.size(), 1);
    EXPECT_EQ(fvs[0], 2);
}
