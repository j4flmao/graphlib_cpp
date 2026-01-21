#include <gtest/gtest.h>
#include "graphlib/graph_core.h"
#include "graphlib/shortest_path.h"
#include "graphlib/mst.h"
#include <vector>
#include <random>

using namespace graphlib;

TEST(ScaleTest, LargeBFSChain) {
    // 100,000 nodes line graph: 0-1-2-...-99999
    // Tests recursion depth if BFS is recursive (it shouldn't be)
    // Tests basic O(N) performance
    int n = 100000;
    Graph g(n, false);
    for(int i=0; i<n-1; ++i) {
        g.add_edge(i, i+1);
    }
    
    auto dist = bfs_distances(g, 0);
    EXPECT_EQ(dist[n-1], n-1);
}

TEST(ScaleTest, LargeDijkstraStar) {
    // Star graph with 100,000 leaves
    // 0 connected to 1..99999
    int n = 100000;
    ShortestPath sp(n);
    for(int i=1; i<n; ++i) {
        sp.add_edge(0, i, 1);
    }
    
    long long inf = 1e18;
    auto dist = sp.dijkstra(0, inf);
    
    EXPECT_EQ(dist[1], 1);
    EXPECT_EQ(dist[n-1], 1);
}

TEST(ScaleTest, LargeKruskal) {
    // Sparse graph: N=10000, M=20000
    // O(M log M)
    int n = 10000;
    std::vector<MstEdge> edges;
    for(int i=0; i<n-1; ++i) {
        edges.push_back({i, i+1, 1}); // Line
    }
    // Add some random edges
    for(int i=0; i<n; ++i) {
        edges.push_back({i, (i+2)%n, 2});
    }
    
    long long weight = MST::kruskal(n, edges);
    // MST should pick all weight 1 edges (n-1 of them)
    // Weight should be n-1
    EXPECT_EQ(weight, n-1);
}

TEST(ScaleTest, LargeConnectedComponents) {
    // 1000 components of size 10
    int num_comps = 1000;
    int comp_size = 10;
    int n = num_comps * comp_size;
    Graph g(n, false);
    
    for(int c=0; c<num_comps; ++c) {
        int base = c * comp_size;
        for(int i=0; i<comp_size-1; ++i) {
            g.add_edge(base+i, base+i+1);
        }
    }
    
    // Using BFS to count components
    int components_found = 0;
    std::vector<bool> visited(n, false);
    for(int i=0; i<n; ++i) {
        if(!visited[i]) {
            components_found++;
            auto dist = bfs_distances(g, i);
            for(int j=0; j<n; ++j) {
                if(dist[j] != -1) visited[j] = true;
            }
        }
    }
    EXPECT_EQ(components_found, num_comps);
}
