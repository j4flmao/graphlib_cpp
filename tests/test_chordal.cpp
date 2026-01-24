#include "gtest/gtest.h"
#include "graphlib/chordal.h"
#include "graphlib/graph_core.h"
#include <algorithm>
#include <vector>

using namespace graphlib;

class ChordalTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}

    // Helper to add undirected edge
    void add_undirected_edge(Graph& g, int u, int v) {
        g.add_edge(u, v, 1);
    }
};

TEST_F(ChordalTest, SimpleChordal) {
    Graph g(4, false);
    // 0-1, 1-2, 2-3, 3-0, 0-2 (chord)
    add_undirected_edge(g, 0, 1);
    add_undirected_edge(g, 1, 2);
    add_undirected_edge(g, 2, 3);
    add_undirected_edge(g, 3, 0);
    add_undirected_edge(g, 0, 2);

    std::vector<int> peo;
    EXPECT_TRUE(is_chordal(g, &peo));
    EXPECT_EQ(peo.size(), 4);
}

TEST_F(ChordalTest, SimpleNonChordal) {
    Graph g(4, false);
    // Square cycle without chord: 0-1-2-3-0
    add_undirected_edge(g, 0, 1);
    add_undirected_edge(g, 1, 2);
    add_undirected_edge(g, 2, 3);
    add_undirected_edge(g, 3, 0);

    EXPECT_FALSE(is_chordal(g));
}

TEST_F(ChordalTest, TreeIsChordal) {
    Graph g(5, false);
    // 0-1, 0-2, 1-3, 1-4
    add_undirected_edge(g, 0, 1);
    add_undirected_edge(g, 0, 2);
    add_undirected_edge(g, 1, 3);
    add_undirected_edge(g, 1, 4);

    EXPECT_TRUE(is_chordal(g));
}

TEST_F(ChordalTest, CompleteGraphIsChordal) {
    Graph g(5, false);
    for (int i = 0; i < 5; ++i) {
        for (int j = i + 1; j < 5; ++j) {
            add_undirected_edge(g, i, j);
        }
    }
    EXPECT_TRUE(is_chordal(g));
}

TEST_F(ChordalTest, MaxCliqueChordal) {
    Graph g(4, false);
    // 0-1, 1-2, 2-3, 3-0, 0-2 (chord)
    // Cliques: {0,1,2} size 3, {0,2,3} size 3
    add_undirected_edge(g, 0, 1);
    add_undirected_edge(g, 1, 2);
    add_undirected_edge(g, 2, 3);
    add_undirected_edge(g, 3, 0);
    add_undirected_edge(g, 0, 2);

    std::vector<int> clique = maximum_clique_chordal(g);
    EXPECT_EQ(clique.size(), 3);
    
    // Check if it is a valid clique
    bool is_clique = true;
    for (size_t i = 0; i < clique.size(); ++i) {
        for (size_t j = i + 1; j < clique.size(); ++j) {
            // Brute force check adjacency
            bool adj = false;
            for (auto* e = g.get_edges(clique[i]); e; e = e->next) {
                if (e->to == clique[j]) {
                    adj = true;
                    break;
                }
            }
            if (!adj) {
                is_clique = false;
                break;
            }
        }
    }
    EXPECT_TRUE(is_clique);
}

TEST_F(ChordalTest, ChromaticNumberChordal) {
    Graph g(4, false);
    // 0-1, 1-2, 2-3, 3-0, 0-2 (chord)
    // Max clique is 3, so chromatic number should be 3 for perfect graphs (chordal graphs are perfect)
    add_undirected_edge(g, 0, 1);
    add_undirected_edge(g, 1, 2);
    add_undirected_edge(g, 2, 3);
    add_undirected_edge(g, 3, 0);
    add_undirected_edge(g, 0, 2);

    std::vector<int> coloring;
    int chi = chromatic_number_chordal(g, &coloring);
    EXPECT_EQ(chi, 3);
    EXPECT_EQ(coloring.size(), 4);

    // Verify valid coloring
    for (int u = 0; u < 4; ++u) {
        for (auto* e = g.get_edges(u); e; e = e->next) {
            EXPECT_NE(coloring[u], coloring[e->to]);
        }
    }
}

TEST_F(ChordalTest, MaxIndependentSetChordal) {
    Graph g(5, false);
    // 0-1, 1-2, 2-3, 3-4, 4-0
    // 5-cycle is NOT chordal.
    // Let's make a chordal graph.
    // Path: 0-1-2-3-4
    add_undirected_edge(g, 0, 1);
    add_undirected_edge(g, 1, 2);
    add_undirected_edge(g, 2, 3);
    add_undirected_edge(g, 3, 4);

    // MIS should be {0, 2, 4} size 3
    std::vector<int> mis = maximum_independent_set_chordal(g);
    EXPECT_EQ(mis.size(), 3);
    
    // Check independence
    for (size_t i = 0; i < mis.size(); ++i) {
        for (size_t j = i + 1; j < mis.size(); ++j) {
            int u = mis[i];
            int v = mis[j];
            bool adj = false;
            for (auto* e = g.get_edges(u); e; e = e->next) {
                if (e->to == v) {
                    adj = true;
                    break;
                }
            }
            EXPECT_FALSE(adj) << "Nodes " << u << " and " << v << " in MIS are adjacent!";
        }
    }
}

TEST_F(ChordalTest, DisconnectedChordal) {
    Graph g(6, false);
    // Triangle 0-1-2
    add_undirected_edge(g, 0, 1);
    add_undirected_edge(g, 1, 2);
    add_undirected_edge(g, 2, 0);

    // Edge 3-4
    add_undirected_edge(g, 3, 4);

    // Isolated 5

    EXPECT_TRUE(is_chordal(g));
    
    std::vector<int> clique = maximum_clique_chordal(g);
    EXPECT_EQ(clique.size(), 3); // The triangle

    int chi = chromatic_number_chordal(g);
    EXPECT_EQ(chi, 3); // Determined by the triangle
}
