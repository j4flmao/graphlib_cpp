#include <gtest/gtest.h>
#include "graphlib/hypergraph.h"
#include <vector>
#include <algorithm>

TEST(HypergraphTest, BasicStructure) {
    graphlib::Hypergraph h(5);

    // Edge 0: {0, 1, 2}
    int e0 = h.add_hyperedge({0, 1, 2}, 1.0);
    // Edge 1: {2, 3}
    int e1 = h.add_hyperedge({2, 3}, 2.0);
    
    EXPECT_EQ(h.vertex_count(), 5);
    EXPECT_EQ(h.edge_count(), 2);
    
    // Check degrees
    EXPECT_EQ(h.vertex_degree(2), 2);
    EXPECT_EQ(h.vertex_degree(0), 1);
    EXPECT_EQ(h.vertex_degree(4), 0);
    
    // Check edge sizes
    EXPECT_EQ(h.edge_size(e0), 3);
    EXPECT_EQ(h.edge_size(e1), 2);

    // Check rank
    EXPECT_EQ(h.max_rank(), 3);
}

TEST(HypergraphTest, Algorithms) {
    graphlib::Hypergraph h(6);
    
    // Cycle C3
    // E0: {0, 1}
    // E1: {1, 2}
    // E2: {2, 0}
    
    h.add_hyperedge({0, 1});
    h.add_hyperedge({1, 2});
    h.add_hyperedge({2, 0});
    
    // Transversal
    auto t = h.greedy_transversal();
    // Verify T covers all edges
    for(int i=0; i<3; ++i) {
        bool covered = false;
        const auto& nodes = h.get_edge_nodes(i);
        for(int u : nodes) {
            for(int v : t) if(u==v) covered = true;
        }
        EXPECT_TRUE(covered);
    }
    
    // Matching
    auto m = h.greedy_matching();
    EXPECT_GE(m.size(), 1); // Should be at least 1
    
    // 2-Coloring (Triangle is not 2-colorable)
    std::vector<int> colors;
    bool colorable = h.is_2_colorable(colors);
    EXPECT_FALSE(colorable);
}

TEST(HypergraphTest, Expansions) {
    graphlib::Hypergraph h(3);
    h.add_hyperedge({0, 1, 2}, 10.0);
    
    // Clique expansion should be K3 with weights 10
    auto g_clique = h.to_clique_expansion();
    EXPECT_EQ(g_clique.vertex_count(), 3);
    // Edges (0,1), (0,2), (1,2)
    // Graph core undirected adds edges u->v and v->u logic? 
    // Wait, Graph implementation details matter here.
    // If we assume clique expansion adds edges, degree should be 2 for all.
    // Graph::get_edges(u) usage would confirm.
    
    // Bipartite expansion
    // 3 original vertices + 1 edge vertex = 4 vertices.
    auto g_bip = h.to_bipartite_expansion();
    EXPECT_EQ(g_bip.vertex_count(), 4);
}
