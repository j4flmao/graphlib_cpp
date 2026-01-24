#include <gtest/gtest.h>
#include <graphlib/graphlib.h>
#include <graphlib/graph_measures.h>
#include <vector>
#include <cmath>

using namespace graphlib;

TEST(GraphMeasuresTest, PathGraph) {
    // 0-1-2-3-4
    Graph g(5, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(3, 4);
    
    // Eccentricity:
    // 0: 4 (to 4)
    // 1: 3 (to 4)
    // 2: 2 (to 0 or 4)
    // 3: 3 (to 0)
    // 4: 4 (to 0)
    std::vector<int> ecc = eccentricity(g);
    EXPECT_EQ(ecc[0], 4);
    EXPECT_EQ(ecc[1], 3);
    EXPECT_EQ(ecc[2], 2);
    EXPECT_EQ(ecc[3], 3);
    EXPECT_EQ(ecc[4], 4);
    
    EXPECT_EQ(diameter(g), 4);
    EXPECT_EQ(radius(g), 2);
    
    std::vector<int> c = center(g);
    ASSERT_EQ(c.size(), 1);
    EXPECT_EQ(c[0], 2);
    
    std::vector<int> p = periphery(g);
    ASSERT_EQ(p.size(), 2);
    EXPECT_TRUE((p[0] == 0 && p[1] == 4) || (p[0] == 4 && p[1] == 0));
}

TEST(GraphMeasuresTest, CycleGraph) {
    // 0-1-2-3-0
    Graph g(4, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(3, 0);
    
    // All symmetric. Distance max is 2 (opposite).
    std::vector<int> ecc = eccentricity(g);
    for (int e : ecc) EXPECT_EQ(e, 2);
    
    EXPECT_EQ(diameter(g), 2);
    EXPECT_EQ(radius(g), 2);
    EXPECT_EQ(center(g).size(), 4);
}

TEST(GraphMeasuresTest, DisconnectedGraph) {
    Graph g(5, false);
    g.add_edge(0, 1);
    g.add_edge(2, 3);
    // 4 is isolated
    
    std::vector<int> ecc = eccentricity(g);
    EXPECT_EQ(ecc[0], -1);
    EXPECT_EQ(ecc[4], -1);
    
    EXPECT_EQ(diameter(g), -1);
    EXPECT_EQ(radius(g), -1);
}

TEST(GraphMeasuresTest, ClosenessCentrality) {
    // Star graph: 0 center, 1,2,3,4 leaves
    Graph g(5, false);
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(0, 3);
    g.add_edge(0, 4);
    
    std::vector<double> clos = closeness_centrality(g);
    
    // Center 0: dist to others = 1. Sum = 4. N-1 = 4. Closeness = 4/4 = 1.0.
    EXPECT_NEAR(clos[0], 1.0, 1e-6);
    
    // Leaf 1: dist to 0 is 1. dist to 2,3,4 is 2. Sum = 1 + 2+2+2 = 7.
    // Closeness = 4/7 approx 0.5714
    EXPECT_NEAR(clos[1], 4.0/7.0, 1e-6);
}

TEST(GraphMeasuresTest, BetweennessCentrality) {
    // Path: 0-1-2
    Graph g(3, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    
    std::vector<double> bet = betweenness_centrality(g);
    
    // Paths:
    // 0->1: via nothing (direct)
    // 0->2: via 1
    // 1->0: via nothing
    // 1->2: via nothing
    // 2->0: via 1
    // 2->1: via nothing
    
    // Node 0: endpoint. 0.
    // Node 2: endpoint. 0.
    // Node 1: on path 0->2 and 2->0.
    // For undirected, we count pair {0,2}. 1 is on it.
    // Brandes returns directed sum. 
    // s=0: delta[1] = 1 (for 2). CB[1]+=1.
    // s=2: delta[1] = 1 (for 0). CB[1]+=1.
    // Total CB[1] = 2.
    // Normalized for undirected (divide by 2) -> 1.
    
    EXPECT_NEAR(bet[0], 0.0, 1e-6);
    EXPECT_NEAR(bet[2], 0.0, 1e-6);
    EXPECT_NEAR(bet[1], 1.0, 1e-6);
}

TEST(GraphMeasuresTest, PageRankSimple) {
    // 0 -> 1 -> 2 -> 0
    Graph g(3, true);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);

    // Symmetric cycle, all should have equal rank 1/3
    std::vector<double> pr = pagerank(g, 0.85, 100);
    EXPECT_NEAR(pr[0], 1.0/3.0, 1e-4);
    EXPECT_NEAR(pr[1], 1.0/3.0, 1e-4);
    EXPECT_NEAR(pr[2], 1.0/3.0, 1e-4);
}

TEST(GraphMeasuresTest, ClusteringCoefficientBasic) {
    // Triangle K3: 0-1, 1-2, 2-0
    Graph g(3, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);

    // Each node has 2 neighbors connected to each other.
    // 2 edges / (2*1/2) = 1.0
    std::vector<double> cc = clustering_coefficient(g);
    EXPECT_NEAR(cc[0], 1.0, 1e-6);
    EXPECT_NEAR(cc[1], 1.0, 1e-6);
    EXPECT_NEAR(cc[2], 1.0, 1e-6);
    EXPECT_NEAR(average_clustering_coefficient(g), 1.0, 1e-6);
}

TEST(GraphMeasuresTest, ClusteringCoefficientPath) {
    // 0-1-2
    Graph g(3, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);

    // 0: 1 neighbor (k=1) -> CC = 0
    // 2: 1 neighbor (k=1) -> CC = 0
    // 1: 2 neighbors (0, 2). Edge (0,2) does NOT exist. CC = 0.
    std::vector<double> cc = clustering_coefficient(g);
    EXPECT_NEAR(cc[0], 0.0, 1e-6);
    EXPECT_NEAR(cc[1], 0.0, 1e-6);
    EXPECT_NEAR(cc[2], 0.0, 1e-6);
}

TEST(GraphMeasuresTest, CoreNumber) {
    // 0-1, 1-2, 2-0 (Triangle K3) -> all core 2
    // 2-3
    // 3-4, 4-5, 5-3 (Triangle K3) -> all core 2?
    // Wait. 3 connects to 2 (degree 2 in triangle + 1 to 3 = 3).
    // Let's trace.
    // 0: deg 2.
    // 1: deg 2.
    // 2: deg 3 (0,1,3).
    // 3: deg 3 (2,4,5).
    // 4: deg 2.
    // 5: deg 2.
    // Min deg is 2 (nodes 0,1,4,5).
    // Remove them?
    // If we remove 0,1 (part of K3), 2 loses 2 edges.
    // Wait, K-Core definition: maximal subgraph where every vertex has degree >= k within the subgraph.
    // The whole graph has min degree 2. So it is a 2-core?
    // Yes.
    
    Graph g(7, false); // Increased to 7 to accommodate node 6 later
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);
    g.add_edge(2, 3);
    g.add_edge(3, 4);
    g.add_edge(4, 5);
    g.add_edge(5, 3);
    
    std::vector<int> cn = core_number(g);
    for(int i=0; i<6; ++i) EXPECT_EQ(cn[i], 2);
    EXPECT_EQ(cn[6], 0); // Isolated initially
    
    // Add a pendant 6 connected to 5.
    g.add_edge(5, 6);
    // 6 has degree 1. Core number 1.
    // After removing 6, 5 has degree 2 (was 3).
    // Rest is still 2-core.
    cn = core_number(g);
    EXPECT_EQ(cn[6], 1);
    for(int i=0; i<6; ++i) EXPECT_EQ(cn[i], 2);
}
