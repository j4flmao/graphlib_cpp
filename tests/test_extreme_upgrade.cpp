#include "graphlib/k_shortest_paths.h"
#include "graphlib/max_flow.h"
#include "graphlib/np_hard.h"
#include "graphlib/graph_generator.h"
#include <gtest/gtest.h>

// -----------------------------------------------------------------------------
// Tests for Yen's K-Shortest Paths
// -----------------------------------------------------------------------------

class KShortestPathsTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(KShortestPathsTest, BasicGraph) {
    // Graph from Wikipedia Yen's Algorithm example
    // C=0, D=1, E=2, F=3, G=4, H=5
    // Edges:
    // C->D: 3, C->E: 2
    // D->F: 4
    // E->D: 1, E->F: 2, E->G: 3
    // F->G: 2, F->H: 1
    // G->H: 2
    
    // Mapped:
    // 0->1 (3), 0->2 (2)
    // 1->3 (4)
    // 2->1 (1), 2->3 (2), 2->4 (3)
    // 3->4 (2), 3->5 (1)
    // 4->5 (2)
    
    // Start: C(0), End: H(5)
    
    graphlib::Graph g(6, true);
    g.add_edge(0, 1, 3);
    g.add_edge(0, 2, 2);
    g.add_edge(1, 3, 4);
    g.add_edge(2, 1, 1);
    g.add_edge(2, 3, 2);
    g.add_edge(2, 4, 3);
    g.add_edge(3, 4, 2);
    g.add_edge(3, 5, 1);
    g.add_edge(4, 5, 2);
    
    // Expected paths:
    // 1. C-E-F-H (0-2-3-5) Cost: 2+2+1 = 5
    // 2. C-E-G-H (0-2-4-5) Cost: 2+3+2 = 7
    // 3. C-D-F-H (0-1-3-5) Cost: 3+4+1 = 8
    
    auto paths = graphlib::yen_k_shortest_paths(g, 0, 5, 3);
    
    ASSERT_EQ(paths.size(), 3);
    
    // Check path 1
    std::vector<int> p1 = {0, 2, 3, 5};
    EXPECT_EQ(paths[0], p1);
    
    // Check path 2
    std::vector<int> p2 = {0, 2, 4, 5};
    EXPECT_EQ(paths[1], p2);
    
    // Check path 3
    std::vector<int> p3 = {0, 1, 3, 5};
    EXPECT_EQ(paths[2], p3);
}

TEST_F(KShortestPathsTest, Disconnected) {
    graphlib::Graph g(4, true);
    g.add_edge(0, 1, 1);
    g.add_edge(2, 3, 1);
    
    auto paths = graphlib::yen_k_shortest_paths(g, 0, 3, 3);
    EXPECT_TRUE(paths.empty());
}

// -----------------------------------------------------------------------------
// Tests for Push-Relabel Max Flow
// -----------------------------------------------------------------------------

class PushRelabelTest : public ::testing::Test {};

TEST_F(PushRelabelTest, SimpleFlow) {
    // 0 -> 1 (10)
    // 0 -> 2 (10)
    // 1 -> 3 (4)
    // 1 -> 4 (8)
    // 2 -> 4 (9)
    // 3 -> 5 (10)
    // 4 -> 5 (10)
    // 4 -> 3 (6)
    
    graphlib::MaxFlow mf(6);
    mf.add_edge(0, 1, 10);
    mf.add_edge(0, 2, 10);
    mf.add_edge(1, 3, 4);
    mf.add_edge(1, 4, 8);
    mf.add_edge(2, 4, 9);
    mf.add_edge(3, 5, 10);
    mf.add_edge(4, 5, 10);
    mf.add_edge(4, 3, 6);
    
    // Expected max flow: 19
    EXPECT_EQ(mf.push_relabel(0, 5), 19);
}

// -----------------------------------------------------------------------------
// Tests for DSatur Coloring
// -----------------------------------------------------------------------------

class DSaturTest : public ::testing::Test {};

TEST_F(DSaturTest, CycleOdd) {
    // C5: 5 nodes cycle. Chromatic number 3.
    graphlib::Graph g = graphlib::generator::cycle_graph(5, false);
    
    std::vector<int> colors;
    int num_colors = graphlib::dsatur_coloring(g, colors);
    
    EXPECT_EQ(num_colors, 3);
    // Verify valid coloring
    for(int u=0; u<5; ++u) {
        auto* e = g.get_edges(u);
        while(e) {
            EXPECT_NE(colors[u], colors[e->to]);
            e = e->next;
        }
    }
}

TEST_F(DSaturTest, Bipartite) {
    // K3,3 is bipartite, so 2 colors.
    graphlib::Graph g(6, false);
    for(int i=0; i<3; ++i) {
        for(int j=3; j<6; ++j) {
            g.add_edge(i, j);
        }
    }
    
    std::vector<int> colors;
    int num_colors = graphlib::dsatur_coloring(g, colors);
    
    EXPECT_EQ(num_colors, 2);
}

// -----------------------------------------------------------------------------
// Tests for Maximum Independent Set
// -----------------------------------------------------------------------------

class MaxIndependentSetTest : public ::testing::Test {};

TEST_F(MaxIndependentSetTest, Cycle5) {
    // C5: 0-1-2-3-4-0. Max Independent Set size is 2 (e.g., {0, 2}).
    graphlib::Graph g = graphlib::generator::cycle_graph(5, false);
    EXPECT_EQ(graphlib::max_independent_set(g), 2);
}

TEST_F(MaxIndependentSetTest, Complete5) {
    // K5: Max Independent Set size is 1.
    graphlib::Graph g = graphlib::generator::complete_graph(5, false);
    EXPECT_EQ(graphlib::max_independent_set(g), 1);
}

TEST_F(MaxIndependentSetTest, Path4) {
    // P4: 0-1-2-3. MIS is {0, 2} or {0, 3} or {1, 3}. Size 2.
    graphlib::Graph g = graphlib::generator::path_graph(4, false);
    EXPECT_EQ(graphlib::max_independent_set(g), 2);
}
