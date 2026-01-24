#include <gtest/gtest.h>
#include <graphlib/graphlib.h>
#include <graphlib/graph_measures.h>
#include <vector>
#include <cmath>

using namespace graphlib;

class GraphMeasuresV2Test : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

TEST_F(GraphMeasuresV2Test, BetweennessCentralityKite) {
    // Krackhardt Kite Graph
    // Nodes 0-9.
    // 0-1, 0-2, 0-3, 0-5
    // 1-3, 1-4, 1-6
    // 2-3, 2-5
    // 3-4, 3-5, 3-6
    // 4-6
    // 5-6, 5-7
    // 6-7
    // 7-8
    // 8-9
    
    // Simplified Kite (5 nodes)
    // 0-1, 1-2, 2-3, 3-4 (Line) + 1-3?
    // Let's use a Bull Graph: Triangle 0-1-2 with horns 0-3 and 1-4.
    // 0-1, 1-2, 2-0.
    // 0-3
    // 1-4
    
    Graph g(5, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);
    g.add_edge(0, 3);
    g.add_edge(1, 4);
    
    std::vector<double> bet = betweenness_centrality(g);
    
    // 2 is tip of triangle. Paths 3-4 go 3-0-1-4 (via 0,1). 
    // Or 3-0-2-1-4 (longer).
    // Shortest path 3-4 is 3-0-1-4. 0 and 1 are on it.
    
    // Node 2: 
    // 3->2: 3-0-2.
    // 4->2: 4-1-2.
    // 0->1: direct.
    // ...
    // Node 0 and 1 should have high betweenness.
    // Node 2 should have lower.
    // Nodes 3 and 4 (endpoints) should be 0.
    
    EXPECT_NEAR(bet[3], 0.0, 1e-6);
    EXPECT_NEAR(bet[4], 0.0, 1e-6);
    EXPECT_GT(bet[0], bet[2]);
    EXPECT_GT(bet[1], bet[2]);
    EXPECT_NEAR(bet[0], bet[1], 1e-6); // Symmetric
}

TEST_F(GraphMeasuresV2Test, ClosenessCentralityDisconnected) {
    // 0-1-2   3-4
    Graph g(5, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(3, 4);
    
    std::vector<double> clos = closeness_centrality(g);
    
    // 0: reachable {1,2}. d(0,1)=1, d(0,2)=2. Sum=3.
    // N=5. Formula (N-1)/Sum = 4/3 = 1.333...
    EXPECT_NEAR(clos[0], 4.0/3.0, 1e-6);
    
    // 1: reachable {0,2}. d(1,0)=1, d(1,2)=1. Sum=2.
    // Formula 4/2 = 2.0.
    EXPECT_NEAR(clos[1], 2.0, 1e-6);
    
    // 3: reachable {4}. d(3,4)=1. Sum=1.
    // Formula 4/1 = 4.0?
    // Wait, this standard formula penalizes disconnectedness poorly if we just sum reachable.
    // But our implementation uses (n-1)/sum_dist.
    // If sum_dist is small (few reachable nodes), closeness explodes?
    // Let's check implementation behavior.
    // If we only sum distances to reachable nodes, then isolated pairs look very "close".
    // Usually closeness for disconnected graphs uses: Sum ( (n-1)/d(u,v) ) ? No.
    // Or: (reachable_count - 1) / sum_dist * (reachable_count - 1) / (n-1).
    // Our implementation currently does: (n-1) / sum_dist_of_reachable.
    // So for 3-4, sum_dist=1. Closeness = 4/1 = 4.
    // For 0-1-2 (center 1), sum_dist=2. Closeness = 4/2 = 2.
    // This makes the small component nodes seem MORE central.
    // This is a known issue with the simple formula.
    // We will just verify it behaves as implemented.
    
    EXPECT_NEAR(clos[3], 4.0, 1e-6);
}

TEST_F(GraphMeasuresV2Test, PageRankSinkNode) {
    // 0 -> 1 -> 2
    // 2 is a sink.
    Graph g(3, true);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    
    // Sink handling should distribute 2's rank to everyone.
    std::vector<double> pr = pagerank(g, 0.85, 100);
    
    double sum = 0;
    for(double p : pr) sum += p;
    EXPECT_NEAR(sum, 1.0, 1e-4);
    
    // 2 should have highest rank because it accumulates.
    EXPECT_GT(pr[2], pr[0]);
    EXPECT_GT(pr[2], pr[1]);
}

TEST_F(GraphMeasuresV2Test, ClusteringCoefficientDetailed) {
    // 0 connected to 1, 2, 3.
    // 1 connected to 2.
    // 3 is isolated from 1 and 2.
    // Neighbors of 0: {1, 2, 3}. k=3.
    // Possible edges among neighbors: (1,2), (1,3), (2,3). Total 3.
    // Existing: (1,2). Count = 1.
    // CC(0) = 1 / 3.
    
    Graph g(4, false);
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(0, 3);
    g.add_edge(1, 2);
    
    std::vector<double> cc = clustering_coefficient(g);
    EXPECT_NEAR(cc[0], 1.0/3.0, 1e-6);
    
    // 1 connected to 0, 2. Neighbors {0, 2}. Edge (0,2) exists.
    // k=2. Possible 1. Existing 1. CC(1) = 1.
    EXPECT_NEAR(cc[1], 1.0, 1e-6);
    
    // 3 connected to 0. k=1. CC(3) = 0.
    EXPECT_NEAR(cc[3], 0.0, 1e-6);
}

TEST_F(GraphMeasuresV2Test, CoreNumberComplex) {
    // Graph with different core levels.
    // K4 (0,1,2,3) -> Core 3
    // Attached to 4. 4 connected to 0.
    // 4 connected to 5.
    
    Graph g(6, false);
    // K4
    g.add_edge(0, 1); g.add_edge(0, 2); g.add_edge(0, 3);
    g.add_edge(1, 2); g.add_edge(1, 3);
    g.add_edge(2, 3);
    
    // 4 connected to 0
    g.add_edge(4, 0);
    
    // 5 connected to 4
    g.add_edge(5, 4);
    
    // Core numbers:
    // 5: deg 1 -> core 1.
    // 4: deg 2 (0,5). After 5 removed, deg 1. Core 1?
    // Wait. 
    // Shell 1: 5 (deg 1). Remove 5.
    // Graph: K4 + {4}, edge (4,0).
    // 4 has deg 1 (to 0). Remove 4. Core 1.
    // Graph: K4. Min deg 3. Remove all. Core 3.
    
    // So 0,1,2,3 should be 3.
    // 4 should be 1.
    // 5 should be 1.
    
    std::vector<int> cn = core_number(g);
    EXPECT_EQ(cn[0], 3);
    EXPECT_EQ(cn[1], 3);
    EXPECT_EQ(cn[2], 3);
    EXPECT_EQ(cn[3], 3);
    EXPECT_EQ(cn[4], 1);
    EXPECT_EQ(cn[5], 1);
}

TEST_F(GraphMeasuresV2Test, EigenvectorCentralityStar) {
    // Star graph: 0 center, 1,2,3,4 leaves.
    // Undirected: 0 connected to 1,2,3,4.
    Graph g(5, false);
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(0, 3);
    g.add_edge(0, 4);
    
    std::vector<double> ec = eigenvector_centrality(g);
    
    // Center should be highest.
    double max_val = ec[0];
    for(int i=1; i<5; ++i) {
        EXPECT_GT(max_val, ec[i]);
    }
}

TEST_F(GraphMeasuresV2Test, HITSSimple) {
    // 0 -> 1, 0 -> 2
    // 3 -> 1, 3 -> 2
    // 0, 3 are pure hubs.
    // 1, 2 are pure authorities.
    Graph g(4, true);
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(3, 1);
    g.add_edge(3, 2);
    
    auto [hubs, auths] = hits(g);
    
    // Hubs: 0 and 3 should be high (equal).
    // 1 and 2 should be 0 (no outgoing edges).
    EXPECT_NEAR(hubs[0], hubs[3], 1e-6);
    EXPECT_NEAR(hubs[1], 0.0, 1e-6);
    EXPECT_NEAR(hubs[2], 0.0, 1e-6);
    EXPECT_GT(hubs[0], 0.0);
    
    // Authorities: 1 and 2 should be high (equal).
    // 0 and 3 should be 0 (no incoming edges).
    EXPECT_NEAR(auths[1], auths[2], 1e-6);
    EXPECT_NEAR(auths[0], 0.0, 1e-6);
    EXPECT_NEAR(auths[3], 0.0, 1e-6);
    EXPECT_GT(auths[1], 0.0);
}

TEST_F(GraphMeasuresV2Test, KatzCentralityStar) {
    // Star graph: 0 center, 1,2,3,4 leaves.
    // Undirected: 0 connected to 1,2,3,4.
    Graph g(5, false);
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(0, 3);
    g.add_edge(0, 4);
    
    // alpha should be < 1/lambda_max.
    // lambda_max = 2. So alpha < 0.5.
    // Let's use alpha = 0.1, beta = 1.0.
    std::vector<double> kc = katz_centrality(g, 0.1, 1.0);
    
    // Center should be highest.
    double max_val = kc[0];
    for(int i=1; i<5; ++i) {
        EXPECT_GT(max_val, kc[i]);
    }
}

TEST_F(GraphMeasuresV2Test, JaccardIndexBasic) {
    // 0 -> 2, 3
    // 1 -> 2, 3
    // 0 and 1 share 2 and 3.
    // N(0) = {2, 3}, N(1) = {2, 3}.
    // Intersection = 2. Union = 2. Jaccard = 1.0.
    Graph g(4, true);
    g.add_edge(0, 2);
    g.add_edge(0, 3);
    g.add_edge(1, 2);
    g.add_edge(1, 3);
    
    EXPECT_NEAR(jaccard_index(g, 0, 1), 1.0, 1e-6);
    
    // Add extra neighbor to 0: 0 -> 4
    // N(0) = {2, 3, 4}, N(1) = {2, 3}
    // Intersection = {2, 3} (size 2). Union = {2, 3, 4} (size 3).
    // Jaccard = 2/3 = 0.666...
    Graph g2(5, true);
    g2.add_edge(0, 2);
    g2.add_edge(0, 3);
    g2.add_edge(0, 4);
    g2.add_edge(1, 2);
    g2.add_edge(1, 3);
    
    EXPECT_NEAR(jaccard_index(g2, 0, 1), 2.0/3.0, 1e-6);
}

TEST_F(GraphMeasuresV2Test, AdamicAdarBasic) {
    // 0 -> 2, 3
    // 1 -> 2, 3
    // Common neighbors: 2, 3.
    // Degree of 2: In-degree? get_edges counts outgoing.
    // Wait, get_edges counts outgoing edges from 2.
    // In our implementation of Adamic-Adar, we use get_edges(w) to find degree.
    // If the graph is directed as above (0->2, 1->2), 2 has no outgoing edges!
    // So degree(2) = 0. log(0) undefined.
    
    // Let's use undirected graph for Link Prediction tests as standard.
    Graph g(4, false);
    g.add_edge(0, 2); // 0-2
    g.add_edge(0, 3); // 0-3
    g.add_edge(1, 2); // 1-2
    g.add_edge(1, 3); // 1-3
    
    // 2 is connected to 0, 1. Degree = 2.
    // 3 is connected to 0, 1. Degree = 2.
    
    // AA(0, 1) = 1/log(deg(2)) + 1/log(deg(3))
    //          = 1/log(2) + 1/log(2)
    //          = 2/log(2) approx 2/0.693 = 2.885
    
    double aa = adamic_adar_index(g, 0, 1);
    EXPECT_NEAR(aa, 2.0 / std::log(2.0), 1e-6);
}

TEST_F(GraphMeasuresV2Test, LabelPropagationCommunities) {
    // Two cliques of size 5 connected by a single edge.
    // 0-4 and 5-9. Connect 4-5.
    Graph g(10, false);
    
    // Clique 1: 0,1,2,3,4
    for(int i=0; i<5; ++i) {
        for(int j=i+1; j<5; ++j) {
            g.add_edge(i, j);
        }
    }
    
    // Clique 2: 5,6,7,8,9
    for(int i=5; i<10; ++i) {
        for(int j=i+1; j<10; ++j) {
            g.add_edge(i, j);
        }
    }
    
    // Bridge
    g.add_edge(4, 5);
    
    auto communities = label_propagation_communities(g, 100, 42);
    
    // Should find 2 communities.
    // Nodes 0-3 should definitely be same community.
    // Nodes 6-9 should definitely be same community.
    // 4 and 5 might be in either, but usually they stick to their clique.
    
    int c0 = communities[0];
    for(int i=1; i<4; ++i) {
        EXPECT_EQ(communities[i], c0);
    }
    
    int c6 = communities[6];
    for(int i=7; i<10; ++i) {
        EXPECT_EQ(communities[i], c6);
    }
    
    EXPECT_NE(c0, c6); // Should be different communities
    
    // Check modularity
    double mod = modularity(g, communities);
    // Modularity for this structure should be high (close to 0.5).
    EXPECT_GT(mod, 0.3);
}

TEST_F(GraphMeasuresV2Test, ModularitySimple) {
    // 4 nodes, 0-1 and 2-3. Disconnected.
    // Ideal communities: {0,1}, {2,3}.
    Graph g(4, false);
    g.add_edge(0, 1);
    g.add_edge(2, 3);
    
    std::vector<int> comms = {0, 0, 1, 1};
    double Q = modularity(g, comms);
    
    // Total edges m = 2. 2m = 4.
    // Community 0:
    // in = 2 (edge 0-1 count as 2 directed).
    // tot = deg(0)+deg(1) = 1+1 = 2.
    // Term 0 = (2/4) - (2/4)^2 = 0.5 - 0.25 = 0.25.
    
    // Community 1:
    // in = 2.
    // tot = 2.
    // Term 1 = 0.25.
    
    // Q = 0.5.
    EXPECT_NEAR(Q, 0.5, 1e-6);
    
    // Bad communities: {0,2}, {1,3} (no edges inside)
    std::vector<int> bad_comms = {0, 1, 0, 1};
    double Q_bad = modularity(g, bad_comms);
    // Comm 0 (nodes 0, 2):
    // in = 0.
    // tot = 2.
    // Term = 0 - (2/4)^2 = -0.25.
    // Comm 1 (nodes 1, 3): Term = -0.25.
    // Q = -0.5.
    EXPECT_NEAR(Q_bad, -0.5, 1e-6);
}

TEST_F(GraphMeasuresV2Test, WeisfeilerLehmanHash) {
    // G1: Path 0-1-2-3
    Graph g1(4, false);
    g1.add_edge(0, 1);
    g1.add_edge(1, 2);
    g1.add_edge(2, 3);
    
    // G2: Path 2-0-3-1 (Renamed)
    // Edges: 2-0, 0-3, 3-1.
    // Isomorphic to G1.
    Graph g2(4, false);
    g2.add_edge(2, 0);
    g2.add_edge(0, 3);
    g2.add_edge(3, 1);
    
    std::string h1 = weisfeiler_lehman_hash(g1, 3);
    std::string h2 = weisfeiler_lehman_hash(g2, 3);
    
    EXPECT_EQ(h1, h2);
    
    // G3: Triangle + Isolated node. 0-1, 1-2, 2-0. 3 isolated.
    Graph g3(4, false);
    g3.add_edge(0, 1);
    g3.add_edge(1, 2);
    g3.add_edge(2, 0);
    
    std::string h3 = weisfeiler_lehman_hash(g3, 3);
    
    EXPECT_NE(h1, h3);
    
    // G4: Star graph (center 0, leaves 1,2,3).
    // Degs: 3, 1, 1, 1.
    Graph g4(4, false);
    g4.add_edge(0, 1);
    g4.add_edge(0, 2);
    g4.add_edge(0, 3);
    
    std::string h4 = weisfeiler_lehman_hash(g4, 3);
    EXPECT_NE(h1, h4);
    EXPECT_NE(h3, h4);
}
