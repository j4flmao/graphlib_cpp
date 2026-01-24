#include "gtest/gtest.h"
#include "graphlib/bipartite.h"
#include "graphlib/graph_core.h"
#include <vector>
#include <algorithm>

using namespace graphlib;

class BipartiteTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}

    void add_undirected_edge(Graph& g, int u, int v) {
        g.add_edge(u, v, 1);
        g.add_edge(v, u, 1);
    }
};

TEST_F(BipartiteTest, IsBipartiteSimple) {
    Graph g(4, false);
    // Square: 0-1-2-3-0
    add_undirected_edge(g, 0, 1);
    add_undirected_edge(g, 1, 2);
    add_undirected_edge(g, 2, 3);
    add_undirected_edge(g, 3, 0);

    std::vector<int> colors = is_bipartite(g);
    EXPECT_FALSE(colors.empty());
    EXPECT_NE(colors[0], colors[1]);
    EXPECT_NE(colors[1], colors[2]);
    EXPECT_NE(colors[2], colors[3]);
    EXPECT_NE(colors[3], colors[0]);
}

TEST_F(BipartiteTest, NotBipartiteOddCycle) {
    Graph g(3, false);
    // Triangle
    add_undirected_edge(g, 0, 1);
    add_undirected_edge(g, 1, 2);
    add_undirected_edge(g, 2, 0);

    std::vector<int> colors = is_bipartite(g);
    EXPECT_TRUE(colors.empty());
}

TEST_F(BipartiteTest, MatchingDFS) {
    Graph g(4, false);
    // 0-1, 2-3
    add_undirected_edge(g, 0, 1);
    add_undirected_edge(g, 2, 3);
    
    // We need to ensure partitions are set.
    // 0(0) - 1(1)
    // 2(0) - 3(1)
    // Matching size 2.

    std::vector<int> match = max_bipartite_matching_dfs(g);
    int size = 0;
    for (int i=0; i<4; ++i) {
        if (match[i] != -1) size++;
    }
    EXPECT_EQ(size, 4); // 2 edges, 4 vertices matched
    EXPECT_EQ(match[0], 1);
    EXPECT_EQ(match[2], 3);
}

TEST_F(BipartiteTest, HopcroftKarpSimple) {
    Graph g(6, false);
    // 0-3
    // 1-3, 1-4
    // 2-4, 2-5
    // Left: 0, 1, 2
    // Right: 3, 4, 5
    // Max matching size 3: (0,3), (1,4), (2,5)
    
    add_undirected_edge(g, 0, 3);
    add_undirected_edge(g, 1, 3);
    add_undirected_edge(g, 1, 4);
    add_undirected_edge(g, 2, 4);
    add_undirected_edge(g, 2, 5);

    std::vector<int> match = hopcroft_karp_matching(g);
    int size = 0;
    for (int x : match) if (x != -1) size++;
    EXPECT_EQ(size, 6); // 3 pairs
}

TEST_F(BipartiteTest, HopcroftKarpComplex) {
    // A slightly larger graph
    // Left: 0..4, Right: 5..9
    Graph g(10, false);
    
    // Edges
    add_undirected_edge(g, 0, 5);
    add_undirected_edge(g, 0, 6);
    add_undirected_edge(g, 1, 5);
    add_undirected_edge(g, 2, 6);
    add_undirected_edge(g, 2, 7);
    add_undirected_edge(g, 2, 8);
    add_undirected_edge(g, 3, 7);
    add_undirected_edge(g, 3, 8);
    add_undirected_edge(g, 4, 8);
    add_undirected_edge(g, 4, 9);
    
    // Matching:
    // 0-5 (or 6)
    // 1-5 (if 0 matches 6)
    // 2-6 (or 7, 8)
    // ...
    // Let's verify size.
    // 0 can take 6. 1 takes 5.
    // 2 takes 7. 3 takes 8. 4 takes 9.
    // Size 5 matching is possible.
    
    std::vector<int> match = hopcroft_karp_matching(g);
    int size = 0;
    for (int x : match) if (x != -1) size++;
    EXPECT_EQ(size, 10); // 5 pairs
    
    // Verify valid matching
    for (int u = 0; u < 10; ++u) {
        if (match[u] != -1) {
            int v = match[u];
            EXPECT_EQ(match[v], u);
            // Check edge exists
            bool edge_exists = false;
            Edge* e = g.get_edges(u);
            while (e) {
                if (e->to == v) {
                    edge_exists = true;
                    break;
                }
                e = e->next;
            }
            EXPECT_TRUE(edge_exists);
        }
    }
}
