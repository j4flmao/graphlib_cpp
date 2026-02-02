#include <gtest/gtest.h>
#include "graphlib/graphlib.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iostream>

using namespace graphlib;

// Helper to calculate cost
long long path_cost(const Graph& g, const std::vector<int>& path) {
    long long cost = 0;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        Edge* e = g.get_edges(path[i]);
        bool found = false;
        while(e) {
            if (e->to == path[i+1]) {
                cost += e->weight;
                found = true;
                break;
            }
            e = e->next;
        }
        if (!found) return -1;
    }
    return cost;
}

TEST(ExtremeFeatures, BidirectionalDijkstra) {
    Graph g(6, true);
    // 0 -> 1 (4), 0 -> 2 (2)
    // 1 -> 2 (5), 1 -> 3 (10)
    // 2 -> 3 (3)
    // 3 -> 4 (4)
    // 4 -> 5 (11)
    g.add_edge(0, 1, 4);
    g.add_edge(0, 2, 2);
    g.add_edge(1, 2, 5);
    g.add_edge(1, 3, 10);
    g.add_edge(2, 3, 3);
    g.add_edge(3, 4, 4);
    g.add_edge(4, 5, 11);

    auto result = bidirectional_dijkstra(g, 0, 5);
    EXPECT_EQ(result.first, 20); // 0->2->3->4->5: 2+3+4+11 = 20
    
    std::vector<int> expected_path = {0, 2, 3, 4, 5};
    EXPECT_EQ(result.second, expected_path);

    // Unreachable
    Graph g2(3, true);
    g2.add_edge(0, 1, 1);
    auto res2 = bidirectional_dijkstra(g2, 0, 2);
    EXPECT_EQ(res2.first, -1);
    EXPECT_TRUE(res2.second.empty());
}

TEST(ExtremeFeatures, SimulatedAnnealingTSP) {
    // 4 cities on a square 10x10
    // 0:(0,0), 1:(10,0), 2:(10,10), 3:(0,10)
    // Distances: 10 between neighbors, 14 (approx 10*sqrt(2)) diagonal
    Graph g(4, false);
    g.add_edge(0, 1, 10);
    g.add_edge(1, 2, 10);
    g.add_edge(2, 3, 10);
    g.add_edge(3, 0, 10);
    g.add_edge(0, 2, 14);
    g.add_edge(1, 3, 14);

    TSP_SA_Config config;
    config.initial_temperature = 1000;
    config.max_iterations_per_temp = 50;
    config.seed = 42;

    auto result = tsp_simulated_annealing(g, config);
    // Optimal is 40 (perimeter)
    // SA might find suboptimal, but for 4 nodes it should find optimal easily.
    EXPECT_EQ(result.first, 40);
    EXPECT_EQ(result.second.size(), 4);
}

TEST(ExtremeFeatures, LouvainCommunity) {
    // Two cliques of size 4 connected by a single edge
    // Clique 1: 0,1,2,3
    // Clique 2: 4,5,6,7
    // Edge: 3-4
    Graph g(8, false);
    
    // Clique 1
    for(int i=0; i<4; ++i)
        for(int j=i+1; j<4; ++j)
            g.add_edge(i, j, 1);
            
    // Clique 2
    for(int i=4; i<8; ++i)
        for(int j=i+1; j<8; ++j)
            g.add_edge(i, j, 1);
            
    // Bridge
    g.add_edge(3, 4, 1);
    
    auto communities = louvain_communities(g);
    
    // Should have 2 communities
    // 0,1,2,3 should be same
    // 4,5,6,7 should be same
    // And comm(0) != comm(4)
    
    EXPECT_EQ(communities[0], communities[1]);
    EXPECT_EQ(communities[0], communities[2]);
    EXPECT_EQ(communities[0], communities[3]);
    
    EXPECT_EQ(communities[4], communities[5]);
    EXPECT_EQ(communities[4], communities[6]);
    EXPECT_EQ(communities[4], communities[7]);
    
    EXPECT_NE(communities[0], communities[4]);
}

TEST(ExtremeFeatures, SpectralBisection) {
    // Simple path graph: 0-1-2-3
    // Fiedler vector should be monotonic
    Graph g(4, false);
    g.add_edge(0, 1, 1);
    g.add_edge(1, 2, 1);
    g.add_edge(2, 3, 1);
    
    auto parts = spectral_bisection(g);
    
    // Should split into {0,1} and {2,3} or similar balanced cut
    EXPECT_EQ(parts.first.size(), 2);
    EXPECT_EQ(parts.second.size(), 2);
    
    // Check if partition is valid (disjoint and covers all)
    std::vector<bool> seen(4, false);
    for(int u : parts.first) seen[u] = true;
    for(int u : parts.second) seen[u] = true;
    for(int i=0; i<4; ++i) EXPECT_TRUE(seen[i]);
    
    // Check connectivity of cut? 
    // Just verify it runs and gives balanced output for this symmetric graph.
}

TEST(ExtremeFeatures, AntColonyTSP) {
    // Same square graph
    Graph g(4, false);
    g.add_edge(0, 1, 10);
    g.add_edge(1, 2, 10);
    g.add_edge(2, 3, 10);
    g.add_edge(3, 0, 10);
    g.add_edge(0, 2, 14);
    g.add_edge(1, 3, 14);

    TSP_ACO_Config config;
    config.num_ants = 10;
    config.max_iterations = 20;
    config.seed = 123;

    auto result = tsp_ant_colony(g, config);
    EXPECT_EQ(result.first, 40);
}
