#include <gtest/gtest.h>
#include "graphlib/coloring.h"
#include <vector>
#include <set>

using namespace graphlib;

TEST(ColoringTest, Bipartite) {
    // Square 0-1-2-3-0
    // 2 colors needed.
    Graph g(4);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 3); g.add_edge(3, 2);
    g.add_edge(3, 0); g.add_edge(0, 3);
    
    std::vector<int> colors = greedy_coloring(g);
    
    // Check valid
    int n = g.vertex_count();
    int max_c = 0;
    for(int i=0; i<n; ++i) {
        EXPECT_GE(colors[i], 0);
        max_c = std::max(max_c, colors[i]);
        Edge* e = g.get_edges(i);
        while(e) {
            EXPECT_NE(colors[i], colors[e->to]);
            e = e->next;
        }
    }
    EXPECT_EQ(max_c, 1); // 0 and 1
    
    EXPECT_EQ(chromatic_number(g), 2);
}

TEST(ColoringTest, CompleteGraph) {
    // K4 needs 4 colors
    Graph g(4);
    for(int i=0; i<4; ++i) {
        for(int j=i+1; j<4; ++j) {
            g.add_edge(i, j);
            g.add_edge(j, i);
        }
    }
    
    EXPECT_EQ(chromatic_number(g), 4);
    std::vector<int> c = greedy_coloring(g);
    int max_c = 0;
    for(int x : c) max_c = std::max(max_c, x);
    EXPECT_EQ(max_c, 3); // 4 colors (0..3)
}

TEST(ColoringTest, OddCycle) {
    // Triangle needs 3 colors
    Graph g(3);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    
    EXPECT_EQ(chromatic_number(g), 3);
}
