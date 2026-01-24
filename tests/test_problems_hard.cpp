#include "graphlib/graph_core.h"
#include "graphlib/shortest_path.h"
#include "graphlib/general_matching.h"
#include "graphlib/np_hard.h"
#include <gtest/gtest.h>
#include <vector>
#include <numeric>
#include <limits>
#include <iostream>
#include <stdexcept>

using namespace graphlib;

TEST(ChinesePostmanTest, EulerianGraph) {
    Graph g(3, false);
    g.add_edge(0, 1, 5);
    g.add_edge(1, 2, 5);
    g.add_edge(2, 0, 5);
    // All degrees 2. Eulerian. Cost = 15.
    EXPECT_EQ(chinese_postman(g), 15);
}

TEST(ChinesePostmanTest, TwoOddVertices) {
    // 0-1-2-3
    // Edges: (0,1,2), (1,2,3), (2,3,4)
    // Odd: 0, 3.
    // Path 0-3 needs to be doubled.
    // Shortest path 0-3 is 2+3+4=9.
    // Total cost = (2+3+4) + 9 = 18.
    Graph g(4, false);
    g.add_edge(0, 1, 2);
    g.add_edge(1, 2, 3);
    g.add_edge(2, 3, 4);
    EXPECT_EQ(chinese_postman(g), 18);
}

TEST(ChinesePostmanTest, KonigsbergBridges) {
    // 4 nodes, 7 edges.
    // 0 has 3 edges to 1, 2 edges to 2?
    // Let's make a simplified version.
    // 0 connected to 1 (w=1), 1 connected to 2 (w=1), 2 connected to 0 (w=1).
    // Plus 0-3 (w=10).
    // Odd degrees:
    // 0: (0,1), (0,2), (0,3) -> deg 3
    // 1: (1,0), (1,2) -> deg 2
    // 2: (2,0), (2,1) -> deg 2
    // 3: (3,0) -> deg 1
    // Odd vertices: 0, 3.
    // Shortest path 0-3 is 10.
    // Total weight = 1+1+1+10 = 13.
    // Add path 0-3 (cost 10).
    // Total = 13 + 10 = 23.
    Graph g(4, false);
    g.add_edge(0, 1, 1);
    g.add_edge(1, 2, 1);
    g.add_edge(2, 0, 1);
    g.add_edge(0, 3, 10);
    EXPECT_EQ(chinese_postman(g), 23);
}

TEST(ChinesePostmanTest, FourOddVertices) {
    // Square with diagonals
    // 0-1 (1), 1-2 (1), 2-3 (1), 3-0 (1)
    // 0-2 (10), 1-3 (10)
    // Degrees:
    // 0: 3 (1, 3, 2)
    // 1: 3 (0, 2, 3)
    // 2: 3 (1, 3, 0)
    // 3: 3 (2, 0, 1)
    // All 4 are odd.
    // Need to match pairs to minimize cost.
    // Distances:
    // 0-1: 1
    // 0-2: 2 (0-1-2 or 0-3-2), direct is 10.
    // 0-3: 1
    // 1-2: 1
    // 1-3: 2 (1-0-3 or 1-2-3), direct is 10.
    // 2-3: 1
    
    // Possible matchings of {0, 1, 2, 3}:
    // 1. (0,1) and (2,3). Cost 1 + 1 = 2.
    // 2. (0,2) and (1,3). Cost 2 + 2 = 4.
    // 3. (0,3) and (1,2). Cost 1 + 1 = 2.
    // Min added cost is 2.
    // Total original weight: 4*1 (cycle) + 2*10 (diagonals) = 24.
    // Total CPP cost = 24 + 2 = 26.
    
    Graph g(4, false);
    g.add_edge(0, 1, 1);
    g.add_edge(1, 2, 1);
    g.add_edge(2, 3, 1);
    g.add_edge(3, 0, 1);
    g.add_edge(0, 2, 10);
    g.add_edge(1, 3, 10);
    
    EXPECT_EQ(chinese_postman(g), 26);
}

TEST(TSPTest, SmallCompleteGraph) {
    Graph g(4);
    // 0--10--1
    // |      |
    // 20     20
    // |      |
    // 3--10--2
    // diagonals: sqrt(500) approx 22 -> let's say 25
    
    g.add_edge(0, 1, 10); g.add_edge(1, 0, 10);
    g.add_edge(1, 2, 20); g.add_edge(2, 1, 20);
    g.add_edge(2, 3, 10); g.add_edge(3, 2, 10);
    g.add_edge(3, 0, 20); g.add_edge(0, 3, 20);
    
    // Add expensive diagonals
    g.add_edge(0, 2, 25); g.add_edge(2, 0, 25);
    g.add_edge(1, 3, 25); g.add_edge(3, 1, 25);
    
    // Optimal: 0-1-2-3-0 cost 10+20+10+20 = 60
    EXPECT_EQ(tsp_bitmask(g), 60);
}

TEST(TSPTest, DisconnectedGraph) {
    Graph g(4);
    g.add_edge(0, 1, 1); g.add_edge(1, 0, 1);
    g.add_edge(2, 3, 1); g.add_edge(3, 2, 1);
    // 0-1 and 2-3 are disconnected
    EXPECT_EQ(tsp_bitmask(g), -1);
}

TEST(TSPTest, SingleVertex) {
    Graph g(1);
    EXPECT_EQ(tsp_bitmask(g), 0);
}

TEST(TSPTest, Triangle) {
    Graph g(3);
    g.add_edge(0, 1, 10); g.add_edge(1, 0, 10);
    g.add_edge(1, 2, 15); g.add_edge(2, 1, 15);
    g.add_edge(2, 0, 20); g.add_edge(0, 2, 20);
    EXPECT_EQ(tsp_bitmask(g), 45);
}

TEST(MaxCliqueTest, K4) {
    Graph g(4, false);
    // Complete graph K4
    for(int i=0; i<4; ++i)
        for(int j=i+1; j<4; ++j)
            g.add_edge(i, j, 1);
            
    EXPECT_EQ(max_clique_bron_kerbosch(g), 4);
}

TEST(MaxCliqueTest, K3_plus_isolated) {
    Graph g(5, false);
    // 0-1-2 triangle
    g.add_edge(0, 1, 1);
    g.add_edge(1, 2, 1);
    g.add_edge(2, 0, 1);
    // 3-4 edge
    g.add_edge(3, 4, 1);
    
    EXPECT_EQ(max_clique_bron_kerbosch(g), 3);
}

TEST(MaxCliqueTest, EmptyGraph) {
    Graph g(5, false);
    EXPECT_EQ(max_clique_bron_kerbosch(g), 1); // Single node is a clique of size 1
}

TEST(MaxCliqueTest, ZeroNodes) {
    EXPECT_THROW({
        Graph g(0, false);
    }, std::invalid_argument);
}
