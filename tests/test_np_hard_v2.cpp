#include <gtest/gtest.h>
#include "graphlib/graphlib.h"
#include "graphlib/np_hard.h"
#include <vector>
#include <set>
#include <numeric>

using namespace graphlib;

class NPHardV2Test : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup if needed
    }
};

// Test DSatur Coloring
TEST_F(NPHardV2Test, DSaturColoringBasic) {
    // Triangle: K3
    // Should use 3 colors
    Graph g(3, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(0, 2);

    std::vector<int> colors;
    int num_colors = dsatur_coloring(g, colors);
    
    EXPECT_EQ(num_colors, 3);
    EXPECT_NE(colors[0], colors[1]);
    EXPECT_NE(colors[1], colors[2]);
    EXPECT_NE(colors[0], colors[2]);
}

TEST_F(NPHardV2Test, DSaturColoringWheel) {
    // Wheel graph W_5 (center 0, outer 1-4)
    // 1-2, 2-3, 3-4, 4-1. 0 connected to all.
    // Outer cycle C4 needs 2 colors. Center needs 1 unique color.
    // Total 3 colors.
    int n = 5;
    Graph g(n, false);
    // Outer cycle
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(3, 4);
    g.add_edge(4, 1);
    // Center
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(0, 3);
    g.add_edge(0, 4);

    std::vector<int> colors;
    int num_colors = dsatur_coloring(g, colors);

    EXPECT_EQ(num_colors, 3);
    
    // Verify valid coloring
    for (int u = 0; u < n; ++u) {
        Edge* e = g.get_edges(u);
        while (e) {
            EXPECT_NE(colors[u], colors[e->to]);
            e = e->next;
        }
    }
}

TEST_F(NPHardV2Test, DSaturColoringBipartite) {
    // Bipartite graph K2,3
    // Should use 2 colors
    Graph g(5, false);
    // Set A: 0, 1
    // Set B: 2, 3, 4
    for(int i=0; i<2; ++i) {
        for(int j=2; j<5; ++j) {
            g.add_edge(i, j);
        }
    }

    std::vector<int> colors;
    int num_colors = dsatur_coloring(g, colors);
    EXPECT_EQ(num_colors, 2);
}

// Test Max Independent Set
TEST_F(NPHardV2Test, MaxIndependentSetBasic) {
    // Triangle: K3
    // Max IS is 1
    Graph g(3, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(0, 2);

    EXPECT_EQ(max_independent_set(g), 1);
}

TEST_F(NPHardV2Test, MaxIndependentSetStar) {
    // Star graph S_4 (center 0, leaves 1,2,3,4)
    // Max IS is {1,2,3,4} -> size 4
    int n = 5;
    Graph g(n, false);
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(0, 3);
    g.add_edge(0, 4);

    EXPECT_EQ(max_independent_set(g), 4);
}

TEST_F(NPHardV2Test, MaxIndependentSetCycle5) {
    // C5: 0-1-2-3-4-0
    // Max IS is 2 (e.g., {0, 2} or {0, 3})
    Graph g(5, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(3, 4);
    g.add_edge(4, 0);

    EXPECT_EQ(max_independent_set(g), 2);
}

TEST_F(NPHardV2Test, MaxIndependentSetDisconnected) {
    // Two triangles K3 + K3
    // Max IS is 1 + 1 = 2
    Graph g(6, false);
    // Triangle 1
    g.add_edge(0, 1); g.add_edge(1, 2); g.add_edge(0, 2);
    // Triangle 2
    g.add_edge(3, 4); g.add_edge(4, 5); g.add_edge(3, 5);

    EXPECT_EQ(max_independent_set(g), 2);
}
