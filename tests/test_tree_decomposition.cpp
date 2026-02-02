#include <gtest/gtest.h>
#include "graphlib/tree_decomposition.h"
#include <set>

using namespace graphlib;

TEST(TreeDecompositionTest, Triangle) {
    // Triangle (K3). Treewidth 2.
    // One bag {0,1,2} usually.
    Graph g(3);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    
    TreeDecomposition td = compute_tree_decomposition(g);
    
    // Check width
    // Width = max_bag_size - 1
    // For K3, should be 3 - 1 = 2.
    EXPECT_GE(td.width, 2);
    
    // Check properties
    std::set<int> covered_vertices;
    for (const auto& bag : td.bags) {
        for (int v : bag.vertices) covered_vertices.insert(v);
    }
    EXPECT_EQ(covered_vertices.size(), 3);
    
    // Check edges
    for (int u = 0; u < 3; ++u) {
        Edge* e = g.get_edges(u);
        while (e) {
            if (u < e->to) {
                bool found = false;
                for (const auto& bag : td.bags) {
                    bool has_u = false, has_v = false;
                    for (int x : bag.vertices) {
                        if (x == u) has_u = true;
                        if (x == e->to) has_v = true;
                    }
                    if (has_u && has_v) {
                        found = true;
                        break;
                    }
                }
                EXPECT_TRUE(found) << "Edge " << u << "-" << e->to << " not covered by any bag";
            }
            e = e->next;
        }
    }
}

TEST(TreeDecompositionTest, Tree) {
    // A tree has treewidth 1.
    // 0-1-2-3
    Graph g(4);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 3); g.add_edge(3, 2);
    
    TreeDecomposition td = compute_tree_decomposition(g);
    // Min-degree heuristic is optimal for trees (leaves have degree 1).
    // Width should be 1.
    EXPECT_EQ(td.width, 1);
}
