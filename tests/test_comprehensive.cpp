#include <gtest/gtest.h>
#include <graphlib/graphlib.h>
#include <vector>
#include <algorithm>
#include <random>
#include <set>
#include <limits>

using namespace graphlib;

// ============================================================================
// 1. CORNER CASES: Empty Graphs, Single Node, Disconnected
// ============================================================================

TEST(CornerCases, EmptyGraph) {
    // If n=0 is not allowed by constructor, we expect it to throw.
    // "Number of vertices must be positive" is the message.
    EXPECT_THROW({
        Graph g(0, false);
    }, std::exception); // Catch generic exception or string check
}

TEST(CornerCases, SingleNodeGraph) {
    Graph g(1, false);
    EXPECT_EQ(g.vertex_count(), 1);
    EXPECT_TRUE(is_connected(g));
    EXPECT_TRUE(is_tree(g));
    EXPECT_TRUE(has_eulerian_trail_undirected(g)); // 0 edges, trail length 0
    EXPECT_TRUE(has_eulerian_cycle_undirected(g));
}

TEST(CornerCases, ShortestPathEmptyOrSingle) {
    ShortestPath sp(1);
    std::vector<long long> dist = sp.dijkstra(0, 1e18);
    EXPECT_EQ(dist[0], 0);
    
    // Self loop
    sp.add_edge(0, 0, 10);
    dist = sp.dijkstra(0, 1e18);
    EXPECT_EQ(dist[0], 0); // Distance to self is 0, ignoring positive cycle
}

TEST(CornerCases, MaxFlowZeroCapacity) {
    MaxFlow mf(2);
    mf.add_edge(0, 1, 0);
    EXPECT_EQ(mf.dinic(0, 1), 0);
}

TEST(CornerCases, MaxFlowSourceEqualsSink) {
    MaxFlow mf(2);
    // Source == Sink should throw
    EXPECT_THROW(mf.dinic(0, 0), std::invalid_argument);
    EXPECT_THROW(mf.edmonds_karp(0, 0), std::invalid_argument);
}

TEST(CornerCases, MSTDisconnected) {
    // 0-1, 2-3
    int n = 4;
    std::vector<MstEdge> edges = {
        {0, 1, 10},
        {2, 3, 20}
    };
    // Kruskal should return sum of edges in forest = 30
    long long w = MST::kruskal(n, edges);
    EXPECT_EQ(w, 30);
}

TEST(CornerCases, SCCSingleNode) {
    SCC g(1);
    std::vector<int> comp(1);
    int count = g.tarjan(comp);
    EXPECT_EQ(count, 1);
    EXPECT_EQ(comp[0], 0);
}

TEST(CornerCases, BipartiteNoEdges) {
    BipartiteGraph bg(3, 3);
    EXPECT_TRUE(bg.is_bipartite());
    EXPECT_EQ(bg.maximum_matching(), 0);
}

// ============================================================================
// 2. STRESS & RANDOMIZED INVARIANTS
// ============================================================================

// Helper to generate random graph
Graph make_random_graph_internal(int n, int m, bool directed, int seed) {
    return make_random_graph(n, m, directed, 1, 100, seed);
}

TEST(StressTest, MaxFlowMinCutTheorem) {
    // For any graph, Max Flow == Min Cut value.
    // We can use Global Min Cut for undirected graphs? 
    // No, s-t Max Flow == s-t Min Cut.
    // Global Min Cut is min over all s,t.
    // Let's verify: Max Flow(s,t) >= Global Min Cut (undirected)
    // Actually, Global Min Cut = min_{s,t} MaxFlow(s,t).
    
    int n = 20;
    int m = 50;
    // Undirected graph for global min cut
    Graph g = make_random_graph(n, m, false, 1, 10, 12345);
    long long global_cut = global_min_cut_undirected(g);
    
    // Compute max flow for all pairs? Too slow.
    // Compute for a few pairs.
    MaxFlow mf(n);
    // Copy edges
    for (int u = 0; u < n; ++u) {
        Edge* e = g.get_edges(u);
        while (e) {
            if (u < e->to) {
                mf.add_undirected_edge(u, e->to, e->weight);
            }
            e = e->next;
        }
    }
    
    long long min_max_flow = std::numeric_limits<long long>::max();
    
    // Check random pairs
    for (int i = 0; i < 10; ++i) {
        int s = i;
        int t = (i + 1) % n;
        MaxFlow mf_copy(n); // Rebuild to avoid residual issues or reset?
        // MaxFlow class doesn't have clear_flow, so rebuild.
        MaxFlow temp_mf(n);
         for (int u = 0; u < n; ++u) {
            Edge* e = g.get_edges(u);
            while (e) {
                if (u < e->to) {
                    temp_mf.add_undirected_edge(u, e->to, e->weight);
                }
                e = e->next;
            }
        }
        long long flow = temp_mf.dinic(s, t);
        min_max_flow = std::min(min_max_flow, flow);
    }
    
    EXPECT_LE(global_cut, min_max_flow);
}

TEST(StressTest, ShortestPathInvariants) {
    // Triangle inequality: d(u,v) <= d(u,w) + w(w,v)
    int n = 30;
    ShortestPath sp(n);
    std::mt19937 rng(12345);
    std::uniform_int_distribution<int> dist_node(0, n - 1);
    std::uniform_int_distribution<int> dist_weight(1, 100);
    
    // Add random edges
    for (int i = 0; i < 100; ++i) {
        int u = dist_node(rng);
        int v = dist_node(rng);
        if (u != v)
            sp.add_edge(u, v, dist_weight(rng));
    }
    
    std::vector<long long> d = sp.dijkstra(0, 1e18);
    
    // Check inequality for all edges
    // We need access to edges. ShortestPath doesn't expose them easily except via private.
    // But we know we added them.
    // Let's just check triangle inequality for all triplets (u, v, w) is hard without adj matrix.
    // Instead, verify d[v] <= d[u] + w(u,v)
    // We can't iterate edges easily from ShortestPath object externally without Graph access.
    // We'll trust the internal tests, but let's compare Dijkstra vs Bellman-Ford.
    
    bool has_neg;
    std::vector<long long> d_bf = sp.bellman_ford(0, 1e18, has_neg);
    
    EXPECT_FALSE(has_neg);
    for (int i = 0; i < n; ++i) {
        if (d[i] != 1e18) {
            EXPECT_EQ(d[i], d_bf[i]);
        } else {
            EXPECT_EQ(d_bf[i], 1e18);
        }
    }
}

TEST(StressTest, BipartiteMatchingSize) {
    // Random bipartite graph
    int n_left = 20;
    int n_right = 20;
    BipartiteGraph bg(n_left, n_right);
    std::mt19937 rng(54321);
    
    for (int i = 0; i < n_left; ++i) {
        for (int j = 0; j < n_right; ++j) {
            if (rng() % 3 == 0) { // 33% density
                bg.add_edge(i, n_left + j);
            }
        }
    }
    
    int matching = bg.maximum_matching();
    EXPECT_LE(matching, n_left);
    EXPECT_LE(matching, n_right);
    
    // Greedy matching size <= Max matching
    // We don't have greedy implemented exposed, but we know it's >= 0.
    EXPECT_GE(matching, 0);
}

TEST(CornerCases, TreeLCAIdentity) {
    TreeLCA t(1);
    t.build(0);
    EXPECT_EQ(t.lca(0, 0), 0);
    EXPECT_EQ(t.distance(0, 0), 0);
    EXPECT_TRUE(t.is_ancestor(0, 0));
}

TEST(CornerCases, TreeDeepChain) {
    // 0-1-2-...-999
    // Recursion limit check
    int n = 2000; 
    TreeLCA t(n);
    for(int i=0; i<n-1; ++i) t.add_edge(i, i+1);
    
    // If implementation uses deep recursion, this might overflow stack on small stacks.
    // But usually 2000 is fine. 
    // Let's try 10000 if we want to be mean, but default stack is 1MB -> ~10-20k frames.
    t.build(0);
    EXPECT_EQ(t.distance(0, n-1), n-1);
    EXPECT_EQ(t.lca(0, n-1), 0);
}

TEST(CornerCases, ConnectivityCompleteGraph) {
    int n = 10;
    Connectivity g(n);
    for(int i=0; i<n; ++i)
        for(int j=i+1; j<n; ++j)
            g.add_edge(i, j);
            
    std::vector<std::pair<int, int>> bridges;
    g.bridges(bridges);
    EXPECT_TRUE(bridges.empty());
    
    std::vector<int> aps;
    g.articulation_points(aps);
    EXPECT_TRUE(aps.empty()); // Removing any node keeps it connected (k-connected)
}

TEST(CornerCases, ConnectivityLineGraph) {
    int n = 10;
    Connectivity g(n);
    for(int i=0; i<n-1; ++i) g.add_edge(i, i+1);
    
    std::vector<std::pair<int, int>> bridges;
    g.bridges(bridges);
    EXPECT_EQ(bridges.size(), n-1); // All edges are bridges
    
    std::vector<int> aps;
    g.articulation_points(aps);
    // 1..n-2 are APs. 0 and n-1 are not.
    // Sort aps to check
    std::sort(aps.begin(), aps.end());
    EXPECT_EQ(aps.size(), n-2);
    if (n > 2) {
        EXPECT_EQ(aps.front(), 1);
        EXPECT_EQ(aps.back(), n-2);
    }
}
