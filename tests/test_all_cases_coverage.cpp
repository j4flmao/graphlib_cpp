#include <gtest/gtest.h>
#include "graphlib/graph_core.h"
#include <vector>

using namespace graphlib;

TEST(AllCases, GridGraphBFS) {
    // 3x3 Grid
    // (0,0)->(0,1)...
    // 0 1 2
    // 3 4 5
    // 6 7 8
    // Dist 0 to 8 is 4.
    Graph g(9, false);
    auto add_grid_edge = [&](int r, int c, int r2, int c2) {
        if(r2>=0 && r2<3 && c2>=0 && c2<3)
            g.add_edge(r*3+c, r2*3+c2);
    };
    for(int r=0; r<3; ++r) {
        for(int c=0; c<3; ++c) {
            add_grid_edge(r, c, r+1, c);
            add_grid_edge(r, c, r, c+1);
        }
    }
    auto dist = bfs_distances(g, 0);
    EXPECT_EQ(dist[8], 4);
}

TEST(AllCases, DisconnectedGraphProperties) {
    Graph g(10, false);
    // 5 pairs of edges
    for(int i=0; i<10; i+=2) g.add_edge(i, i+1);
    
    // Check degrees
    for(int i=0; i<10; ++i) {
        // Degree 1
        int d=0;
        Edge* e = g.get_edges(i);
        while(e) { d++; e=e->next; }
        EXPECT_EQ(d, 1);
    }
}

TEST(AllCases, SelfLoopsAndMultiEdges) {
    Graph g(2, true);
    g.add_edge(0, 0); // Self loop
    g.add_edge(0, 1);
    g.add_edge(0, 1); // Multi edge
    
    int loops = 0;
    int to1 = 0;
    Edge* e = g.get_edges(0);
    while(e) {
        if(e->to == 0) loops++;
        if(e->to == 1) to1++;
        e = e->next;
    }
    EXPECT_EQ(loops, 1);
    EXPECT_EQ(to1, 2);
}

TEST(AllCases, NegativeWeightsInGraph) {
    // Just storage, no algos
    Graph g(2, true);
    g.add_edge(0, 1, -10);
    Edge* e = g.get_edges(0);
    EXPECT_EQ(e->weight, -10);
}
