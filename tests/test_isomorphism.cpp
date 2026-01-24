#include "graphlib/isomorphism.h"
#include "graphlib/graph_generator.h"
#include <gtest/gtest.h>
#include <algorithm>

using namespace graphlib;

TEST(IsomorphismTest, SimpleIsomorphism) {
    Graph g1(3, false); // Triangle
    g1.add_edge(0, 1);
    g1.add_edge(1, 2);
    g1.add_edge(2, 0);

    Graph g2(3, false); // Triangle with permuted indices
    g2.add_edge(0, 2);
    g2.add_edge(2, 1);
    g2.add_edge(1, 0);

    EXPECT_TRUE(is_isomorphic(g1, g2));

    Graph g3(3, false); // Line
    g3.add_edge(0, 1);
    g3.add_edge(1, 2);

    EXPECT_FALSE(is_isomorphic(g1, g3));
}

TEST(IsomorphismTest, SubgraphIsomorphism) {
    Graph pattern(3, false); // Triangle
    pattern.add_edge(0, 1);
    pattern.add_edge(1, 2);
    pattern.add_edge(2, 0);

    Graph target(4, false); // K4
    target.add_edge(0, 1); target.add_edge(0, 2); target.add_edge(0, 3);
    target.add_edge(1, 2); target.add_edge(1, 3);
    target.add_edge(2, 3);

    EXPECT_TRUE(is_subgraph_isomorphic(pattern, target));
    
    // Triangle is NOT subgraph isomorphic to Square (C4)
    Graph square(4, false);
    square.add_edge(0, 1);
    square.add_edge(1, 2);
    square.add_edge(2, 3);
    square.add_edge(3, 0);
    
    EXPECT_FALSE(is_subgraph_isomorphic(pattern, square));
}

TEST(IsomorphismTest, FindAllMappings) {
    Graph pattern(2, false); // Edge 0-1
    pattern.add_edge(0, 1);

    Graph target(3, false); // Triangle 0-1-2-0
    target.add_edge(0, 1);
    target.add_edge(1, 2);
    target.add_edge(2, 0);

    auto mappings = find_all_subgraph_isomorphisms(pattern, target);
    // Expect 6 mappings: (0->0, 1->1), (0->0, 1->2), (0->1, 1->0), (0->1, 1->2), etc.
    // Each edge in target (3 edges) can be mapped in 2 directions.
    // Total 6.
    EXPECT_EQ(mappings.size(), 6);
    
    for (const auto& m : mappings) {
        EXPECT_EQ(m.size(), 2);
        int u = m[0];
        int v = m[1];
        // Must correspond to an edge in target
        bool has_edge = false;
        for (Edge* e = target.get_edges(u); e != nullptr; e = e->next) {
            if (e->to == v) has_edge = true;
        }
        EXPECT_TRUE(has_edge);
    }
}

TEST(IsomorphismTest, PetersenGraphSelfIsomorphism) {
    // Petersen graph is isomorphic to itself
    // And is vertex transitive
    Graph petersen(10, false);
    // Outer cycle
    petersen.add_edge(0, 1); petersen.add_edge(1, 2); petersen.add_edge(2, 3); petersen.add_edge(3, 4); petersen.add_edge(4, 0);
    // Inner star
    petersen.add_edge(5, 7); petersen.add_edge(7, 9); petersen.add_edge(9, 6); petersen.add_edge(6, 8); petersen.add_edge(8, 5);
    // Spokes
    petersen.add_edge(0, 5); petersen.add_edge(1, 6); petersen.add_edge(2, 7); petersen.add_edge(3, 8); petersen.add_edge(4, 9);
    
    EXPECT_TRUE(is_isomorphic(petersen, petersen));
    
    // Remove one edge
    Graph petersen_minus(10, false);
    // Copy edges except one
    // ... simpler way: copy and remove edge? Graph doesn't support remove_edge easily yet?
    // Let's reconstruct.
    // Actually, just verify self-isomorphism works efficiently.
}

TEST(IsomorphismTest, IsomorphismCheckWithMapping) {
    Graph g1(4, false); // 0-1-2-3
    g1.add_edge(0, 1);
    g1.add_edge(1, 2);
    g1.add_edge(2, 3);
    
    Graph g2(4, false); // 3-0-1-2 (renamed)
    g2.add_edge(3, 0);
    g2.add_edge(0, 1);
    g2.add_edge(1, 2);
    
    std::vector<int> mapping;
    EXPECT_TRUE(is_isomorphic(g1, g2, &mapping));
    
    // Verify mapping
    EXPECT_EQ(mapping.size(), 4);
    // Check if mapping preserves adjacency
    for (int u = 0; u < 4; ++u) {
        for (Edge* e1 = g1.get_edges(u); e1 != nullptr; e1 = e1->next) {
            int v = e1->to;
            int u_mapped = mapping[u];
            int v_mapped = mapping[v];
            
            // Check if edge exists in g2
            bool found = false;
            for (Edge* e2 = g2.get_edges(u_mapped); e2 != nullptr; e2 = e2->next) {
                if (e2->to == v_mapped) found = true;
            }
            EXPECT_TRUE(found);
        }
    }
}
