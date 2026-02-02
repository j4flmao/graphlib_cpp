#include <gtest/gtest.h>
#include "graphlib/parallel.h"
#include "graphlib/graph_generator.h"
#include <vector>
#include <numeric>
#include <algorithm>

using namespace graphlib;

TEST(ParallelTest, BFS) {
    // 100 nodes, connected
    Graph g = generator::random_graph(100, 0.1);
    
    // Sequential
    auto dist_seq = parallel::parallel_bfs(g, 0, parallel::ExecutionPolicy::Sequential);
    
    // Parallel
    auto dist_par = parallel::parallel_bfs(g, 0, parallel::ExecutionPolicy::Parallel);
    
    EXPECT_EQ(dist_seq, dist_par);
}

TEST(ParallelTest, ConnectedComponents) {
    Graph g(10);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    // Component 1: {0, 1, 2}
    
    g.add_edge(3, 4);
    // Component 2: {3, 4}
    
    // Component 3..7: isolated {5}, {6}, {7}, {8}, {9}
    
    std::vector<int> comp;
    int num = parallel::parallel_connected_components(g, comp, parallel::ExecutionPolicy::Parallel);
    
    EXPECT_EQ(num, 7);
    EXPECT_EQ(comp[0], comp[1]);
    EXPECT_EQ(comp[1], comp[2]);
    EXPECT_EQ(comp[3], comp[4]);
    EXPECT_NE(comp[0], comp[3]);
    EXPECT_NE(comp[0], comp[5]);
}

TEST(ParallelTest, TriangleCount) {
    // Complete graph K5 -> 10 triangles?
    // 5 choose 3 = 10.
    Graph g(5);
    for(int i=0; i<5; ++i)
        for(int j=i+1; j<5; ++j)
            g.add_edge(i, j);
            
    long long tc = parallel::parallel_triangle_count(g, parallel::ExecutionPolicy::Parallel);
    EXPECT_EQ(tc, 10);
}

TEST(ParallelTest, MIS) {
    // Star graph: Center connected to all leaves.
    // MIS should be either {Center} or {All Leaves}.
    // Size is either 1 or N-1.
    // Wait, MIS is MAXIMAL (cannot add more), not MAXIMUM (largest size).
    // But usually we prefer larger.
    // Luby finds A maximal independent set.
    
    int n = 100;
    Graph g(n);
    for(int i=1; i<n; ++i) {
        g.add_edge(0, i);
    }
    
    auto mis = parallel::parallel_maximal_independent_set(g, parallel::ExecutionPolicy::Parallel);
    
    // Check independence
    std::vector<int> in_mis(n, 0);
    for(int u : mis) in_mis[u] = 1;
    
    for(int u=0; u<n; ++u) {
        for(Edge* e = g.get_edges(u); e; e = e->next) {
            if (in_mis[u] && in_mis[e->to]) {
                FAIL() << "Two connected vertices in MIS!";
            }
        }
    }
    
    // Check maximality (cannot add any other vertex)
    for(int i=0; i<n; ++i) {
        if (!in_mis[i]) {
            // Must have a neighbor in MIS
            bool covered = false;
            for(Edge* e = g.get_edges(i); e; e = e->next) {
                if (in_mis[e->to]) {
                    covered = true;
                    break;
                }
            }
            EXPECT_TRUE(covered) << "Vertex " << i << " is not in MIS and has no neighbor in MIS!";
        }
    }
}

TEST(ParallelTest, Coloring) {
    // 3-Colorable graph (cycle C3 is 3-colorable)
    // Bipartite graph is 2-colorable.
    
    Graph g = generator::path_graph(20); // Bipartite
    
    auto colors = parallel::parallel_coloring(g, parallel::ExecutionPolicy::Parallel);
    
    for(int u=0; u<20; ++u) {
        EXPECT_GE(colors[u], 0);
        for(Edge* e = g.get_edges(u); e; e = e->next) {
            EXPECT_NE(colors[u], colors[e->to]) << "Adjacent vertices have same color!";
        }
    }
}

TEST(ParallelTest, Reduction) {
    Graph g(100);
    // Map: return vertex ID
    // Reduce: sum
    // Result: sum 0..99 = 4950
    
    long long sum = parallel::parallel_reduce_vertices<long long>(
        g,
        [](int i) { return (long long)i; },
        [](long long a, long long b) { return a + b; },
        0
    );
    
    EXPECT_EQ(sum, 4950);
}
