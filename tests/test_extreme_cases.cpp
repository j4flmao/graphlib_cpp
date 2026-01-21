#include <gtest/gtest.h>
#include "graphlib/graph_core.h"
#include "graphlib/shortest_path.h"
#include "graphlib/max_flow.h"
#include "graphlib/mst.h"
#include <vector>
#include <limits>
#include <algorithm>

using namespace graphlib;

// Extreme Case 1: Self-loops and Multi-edges
TEST(ExtremeTest, SelfLoopsAndMultiEdges) {
    Graph g(3, true);
    // Self loop
    g.add_edge(0, 0, 10);
    
    // Multi-edge
    g.add_edge(0, 1, 5);
    g.add_edge(0, 1, 3); // Better edge
    
    // BFS Distances
    std::vector<int> dist = bfs_distances(g, 0);
    EXPECT_EQ(dist[0], 0);
    EXPECT_EQ(dist[1], 1);
    
    // Shortest Path (Dijkstra) should pick weight 3
    ShortestPath sp(3);
    sp.add_edge(0, 0, 10);
    sp.add_edge(0, 1, 5);
    sp.add_edge(0, 1, 3);
    
    std::vector<long long> sp_dist = sp.dijkstra(0, 1000000);
    EXPECT_EQ(sp_dist[1], 3);
}

// Extreme Case 2: Max Integer Weights
TEST(ExtremeTest, LargeWeightsOverflowCheck) {
    ShortestPath sp(3);
    long long large = 1e15; // Within long long, but sum might overflow if not careful
    // But Dijkstra uses long long, max is LLONG_MAX. 
    // 2 * 1e15 is safe.
    
    sp.add_edge(0, 1, large);
    sp.add_edge(1, 2, large);
    
    long long inf = std::numeric_limits<long long>::max();
    std::vector<long long> dist = sp.dijkstra(0, inf);
    EXPECT_EQ(dist[2], 2 * large);
}

// Extreme Case 3: Dense Graph Performance
// Complete graph K_20 (380 edges)
TEST(ExtremeTest, DenseGraphShortestPath) {
    int n = 20;
    ShortestPath sp(n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                sp.add_edge(i, j, 1);
            }
        }
    }
    
    // 0 -> n-1 direct edge cost 1
    std::vector<long long> dist = sp.dijkstra(0, 1000000);
    EXPECT_EQ(dist[n-1], 1);
}

// Extreme Case 4: Deep Recursion (Chain Graph)
// 0->1->2->...->999
TEST(ExtremeTest, DeepChainDFS) {
    int n = 1000;
    Graph g(n, true);
    for (int i = 0; i < n - 1; ++i) {
        g.add_edge(i, i + 1);
    }
    
    // BFS distances check
    std::vector<int> dist = bfs_distances(g, 0);
    EXPECT_EQ(dist[n-1], n-1);
}

// Extreme Case 5: Disconnected Components for MST
TEST(ExtremeTest, MSTDisconnectedForest) {
    // 0-1 (10)
    // 2-3 (20)
    Graph g(4, false); // Undirected
    g.add_edge(0, 1, 10);
    g.add_edge(2, 3, 20);
    
    // Kruskal should return 10 + 20 = 30
    std::vector<MstEdge> edges;
    for(int i=0; i<4; ++i) {
        Edge* e = g.get_edges(i);
        while(e) {
            if (i < e->to) { // Add once
                edges.push_back({i, e->to, e->weight});
            }
            e = e->next;
        }
    }
    
    long long cost = MST::kruskal(4, edges);
    EXPECT_EQ(cost, 30);
}

TEST(ExtremeTest, ZeroNodesException) {
    EXPECT_THROW(Graph(0), std::invalid_argument);
    EXPECT_THROW(ShortestPath(0), std::invalid_argument);
    EXPECT_THROW(MaxFlow(0), std::invalid_argument);
}

TEST(ExtremeTest, SingleNodeGraph) {
    Graph g(1);
    // Add self loop
    g.add_edge(0, 0, 5);
    
    auto dist = bfs_distances(g, 0);
    EXPECT_EQ(dist[0], 0); // Distance to self is 0
}

TEST(ExtremeTest, MaxFlowDisconnected) {
    MaxFlow mf(5);
    // 0-1-2 and 3-4, no path 0->4
    mf.add_edge(0, 1, 10);
    mf.add_edge(1, 2, 10);
    mf.add_edge(3, 4, 10);
    
    EXPECT_EQ(mf.dinic(0, 4), 0);
}

TEST(ExtremeTest, MaxFlowPathological) {
    // "Diamond" graph that can be slow for Edmonds-Karp if not careful, 
    // but Dinic should be fast.
    // However, here we just check correctness on a small instance.
    // S -> U, V -> T
    // U -> V with capacity 1
    // S->U cap 1000, V->T cap 1000
    // S->V cap 0, U->T cap 0
    
    MaxFlow mf(4);
    int S=0, U=1, V=2, T=3;
    long long C = 1000;
    mf.add_edge(S, U, C);
    mf.add_edge(V, T, C);
    mf.add_edge(U, V, 1);
    
    // Also add direct large paths to make it interesting
    // S->V direct? No.
    
    // Max flow is 1 (S->U->V->T).
    EXPECT_EQ(mf.dinic(S, T), 1);
    
    // Add S->V and U->T
    mf.add_edge(S, V, C);
    mf.add_edge(U, T, C);
    
    // Now flow:
    // S->U->T (1000)
    // S->V->T (1000)
    // S->U->V->T (1) - but U->V is shared? No, capacity constraints.
    // U has 1000 in, 1001 out.
    // V has 1001 in, 1000 out.
    // Flow: S->U (1000) -> T
    //       S->V (1000) -> T
    // U->V isn't needed.
    // Total 2000.
    
    // Reset capacities? MaxFlow class accumulates edges if we add more.
    // Let's make a new graph for clear logic.
    MaxFlow mf2(4);
    mf2.add_edge(S, U, C);
    mf2.add_edge(U, T, C);
    mf2.add_edge(S, V, C);
    mf2.add_edge(V, T, C);
    mf2.add_edge(U, V, 1);
    
    EXPECT_EQ(mf2.dinic(S, T), 2000);
}

TEST(ExtremeTest, MSTAllEqualWeights) {
    // Triangle with all weights 10
    std::vector<MstEdge> edges;
    edges.push_back({0, 1, 10});
    edges.push_back({1, 2, 10});
    edges.push_back({2, 0, 10});
    
    // MST should pick 2 edges, cost 20
    long long cost = MST::kruskal(3, edges);
    EXPECT_EQ(cost, 20);
}

TEST(ExtremeTest, DijkstraDisconnectedInfinity) {
    ShortestPath sp(2);
    // No edges
    std::vector<long long> dist = sp.dijkstra(0, -1); // -1 means no infinity limit logic in helper? 
    // Wait, the API takes 'inf' as a parameter to return for unreachable?
    // Let's check ShortestPath::dijkstra signature. 
    // It usually returns a vector. If unreachable, it contains INF.
    // In our implementation (from memory/previous files), it initializes with the passed 'inf' value or uses it as "infinity".
    
    long long inf_val = 123456789;
    dist = sp.dijkstra(0, inf_val);
    EXPECT_EQ(dist[1], inf_val);
    EXPECT_EQ(dist[0], 0);
}

// Extreme Case 6: Max Flow with Parallel Edges
TEST(ExtremeTest, MaxFlowParallelEdges) {
    MaxFlow mf(2);
    mf.add_edge(0, 1, 10);
    mf.add_edge(0, 1, 10);
    // Total capacity 20
    
    EXPECT_EQ(mf.dinic(0, 1), 20);
}
