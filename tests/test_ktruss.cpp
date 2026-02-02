#include <gtest/gtest.h>
#include "graphlib/community.h"
#include <map>

using namespace graphlib;

TEST(KTrussTest, Triangle) {
    // 0-1, 1-2, 2-0
    // Triangle support is 1 for all edges.
    // k-truss: max k s.t. support >= k-2.
    // k-2 <= 1 => k <= 3.
    // So all edges have trussness 3.
    Graph g(3);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    
    auto truss = k_truss_decomposition(g);
    EXPECT_EQ(truss.size(), 3);
    EXPECT_EQ(truss[{0, 1}], 3);
    EXPECT_EQ(truss[{1, 2}], 3); // Depends on order, test map key
}

TEST(KTrussTest, Butterfly) {
    // 0-1-2-0 and 2-3-4-2 sharing vertex 2.
    // 2 triangles. edge (1,2) support 1 -> trussness 3.
    // edge (2,3) support 1 -> trussness 3.
    // All edges trussness 3.
    Graph g(5);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    
    g.add_edge(2, 3); g.add_edge(3, 2);
    g.add_edge(3, 4); g.add_edge(4, 3);
    g.add_edge(4, 2); g.add_edge(2, 4);
    
    auto truss = k_truss_decomposition(g);
    EXPECT_EQ(truss.size(), 6);
    
    // Check if key exists properly
    int u = 0, v = 1; if(u>v) std::swap(u,v);
    EXPECT_EQ(truss.count({u,v}), 1);
    EXPECT_EQ(truss[{u,v}], 3);
}

TEST(KTrussTest, K4) {
    // Complete graph K4.
    // Each edge is part of 2 triangles.
    // Support = 2.
    // Trussness <= 2 + 2 = 4.
    Graph g(4);
    for(int i=0; i<4; ++i) {
        for(int j=i+1; j<4; ++j) {
            g.add_edge(i, j); g.add_edge(j, i);
        }
    }
    
    auto truss = k_truss_decomposition(g);
    for(auto const& [e, k] : truss) {
        EXPECT_EQ(k, 4);
    }
}

TEST(KTrussTest, Diamond) {
    // 0-1, 0-2, 0-3, 1-2, 1-3. (No 2-3).
    // Edge (1,2) in triangle 0-1-2. Support 1.
    // Edge (1,3) in triangle 0-1-3. Support 1.
    // Edge (0,1) in 0-1-2 and 0-1-3. Support 2.
    // Trussness:
    // Peeling: min support is 1. edges (1,2), (1,3), (0,2), (0,3).
    // Remove edges with support 1 (k=3).
    // Remaining (0,1) loses support, becomes 0.
    // So all have trussness 3.
    Graph g(4);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(0, 2); g.add_edge(2, 0);
    g.add_edge(0, 3); g.add_edge(3, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(1, 3); g.add_edge(3, 1);
    
    auto truss = k_truss_decomposition(g);
    
    // Edge (0,1) initially support 2 (triangles 0-1-2 and 0-1-3).
    // Edge (0,2) support 1 (0-1-2).
    // Edge (1,2) support 1 (0-1-2).
    // Peeling: remove support 1 edges first.
    // (0,2) and (1,2) removed. Degrading (0,1) support by 1.
    // (0,3) and (1,3) removed. Degrading (0,1) support by 1.
    // (0,1) support becomes 0.
    // Max k for (0,1) is it exists in 3-truss?
    // Yes. It has support < 2 eventually.
    // So it's a 3-truss.
    
    for(auto const& [e, k] : truss) {
        EXPECT_EQ(k, 3);
    }
}
