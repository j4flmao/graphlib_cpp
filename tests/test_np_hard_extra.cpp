#include <gtest/gtest.h>
#include <graphlib/graphlib.h>
#include <vector>
#include <algorithm>
#include <numeric>
#include <set>

using namespace graphlib;

// -----------------------------------------------------------------------------
// Graph Coloring (Greedy with Welsh-Powell Heuristic)
// -----------------------------------------------------------------------------

TEST(ColoringTest, BipartiteIs2Colorable) {
    Graph g(4, false); // Square 0-1-2-3-0
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(3, 0);
    
    std::vector<int> colors;
    int k = greedy_coloring(g, colors);
    
    EXPECT_LE(k, 2); // Should be exactly 2
    for(int i=0; i<4; ++i) EXPECT_GE(colors[i], 0);
    EXPECT_NE(colors[0], colors[1]);
    EXPECT_NE(colors[1], colors[2]);
    EXPECT_NE(colors[2], colors[3]);
    EXPECT_NE(colors[3], colors[0]);
}

TEST(ColoringTest, K4Is4Colorable) {
    Graph g(4, false);
    for(int i=0; i<4; ++i)
        for(int j=i+1; j<4; ++j)
            g.add_edge(i, j);
            
    std::vector<int> colors;
    int k = greedy_coloring(g, colors);
    
    EXPECT_EQ(k, 4);
    std::set<int> distinct_colors(colors.begin(), colors.end());
    EXPECT_EQ(distinct_colors.size(), 4);
}

TEST(ColoringTest, OddCycleIs3Colorable) {
    Graph g(5, false); // C5
    g.add_edge(0, 1); g.add_edge(1, 2);
    g.add_edge(2, 3); g.add_edge(3, 4);
    g.add_edge(4, 0);
    
    std::vector<int> colors;
    int k = greedy_coloring(g, colors);
    
    EXPECT_EQ(k, 3);
}

// -----------------------------------------------------------------------------
// Disjoint Paths (Menger's Theorem Validation)
// -----------------------------------------------------------------------------

TEST(DisjointPathsTest, EdgeDisjointPaths) {
    // Graph with bottleneck
    // S -> A, S -> B
    // A -> T, B -> T
    // Plus A -> B
    // Max flow S->T should be 2.
    
    MaxFlow mf(4);
    mf.add_edge(0, 1, 1); // S->A
    mf.add_edge(0, 2, 1); // S->B
    mf.add_edge(1, 3, 1); // A->T
    mf.add_edge(2, 3, 1); // B->T
    mf.add_edge(1, 2, 1); // A->B
    
    long long flow = mf.dinic(0, 3);
    EXPECT_EQ(flow, 2);
}

TEST(DisjointPathsTest, VertexDisjointPaths) {
    // S -> A, S -> B
    // A -> C, B -> C
    // C -> T
    // Vertex C is bottleneck. Max vertex disjoint paths should be 1.
    // To model vertex capacities, split node C into C_in -> C_out with cap 1.
    
    // Nodes: S=0, T=5
    // A=1, B=2, C_in=3, C_out=4
    
    MaxFlow mf(6);
    
    // Edges with infinite capacity (or large enough)
    long long INF = 100;
    
    // S -> A, S -> B
    mf.add_edge(0, 1, INF);
    mf.add_edge(0, 2, INF);
    
    // A -> C_in, B -> C_in
    mf.add_edge(1, 3, INF);
    mf.add_edge(2, 3, INF);
    
    // C_in -> C_out (Vertex capacity 1)
    mf.add_edge(3, 4, 1);
    
    // C_out -> T
    mf.add_edge(4, 5, INF);
    
    long long flow = mf.dinic(0, 5);
    EXPECT_EQ(flow, 1);
}

// -----------------------------------------------------------------------------
// Max Weight Clique Tests
// -----------------------------------------------------------------------------

TEST(MaxCliqueTest, MaxWeightClique) {
    // Triangle with weights
    // Vertices weights: 0: 5, 1: 5, 2: 5
    // Clique {0,1,2} weight = 15.
    
    Graph g(3, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);
    
    std::vector<long long> weights = {5, 5, 5};
    long long mw = max_weight_clique(g, weights);
    
    EXPECT_EQ(mw, 15);
}

TEST(MaxCliqueTest, MaxWeightClique_Independent) {
    // 0-1, 2 (isolated)
    // Weights: 0: 10, 1: 10, 2: 30
    // Max weight clique is {2} with weight 30.
    
    Graph g(3, false);
    g.add_edge(0, 1);
    
    std::vector<long long> weights = {10, 10, 30};
    long long mw = max_weight_clique(g, weights);
    
    EXPECT_EQ(mw, 30);
}
