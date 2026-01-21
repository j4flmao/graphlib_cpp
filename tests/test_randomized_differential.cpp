#include <gtest/gtest.h>
#include "graphlib/graph_core.h"
#include "graphlib/shortest_path.h"
#include "graphlib/max_flow.h"
#include "graphlib/mst.h"
#include "graphlib/splay_tree.h"
#include "graphlib/tree.h"
#include "graphlib/general_matching.h"
#include <random>
#include <set>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace graphlib;

class RandomizedTest : public ::testing::Test {
protected:
    std::mt19937 rng;
    
    void SetUp() override {
        rng.seed(42); // Fixed seed for reproducibility
    }
    
    int rand_int(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(rng);
    }
    
    double rand_double() {
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        return dist(rng);
    }
    
    Graph make_random_graph_internal(int n, double density, bool directed, int min_w, int max_w) {
        Graph g(n, directed);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (i == j) continue;
                if (!directed && i > j) continue;
                
                if (rand_double() < density) {
                    g.add_edge(i, j, rand_int(min_w, max_w));
                }
            }
        }
        return g;
    }
};

// --- Splay Tree Fuzzing ---

TEST_F(RandomizedTest, SplayTreeVsStdSet) {
    SplayTree st;
    std::set<int> ref;
    
    int num_ops = 5000;
    int range = 1000;
    
    for (int i = 0; i < num_ops; ++i) {
        int op = rand_int(0, 2);
        int val = rand_int(0, range);
        
        if (op == 0) { // Insert
            if (ref.find(val) == ref.end()) {
                st.insert(val);
                ref.insert(val);
            }
        } else if (op == 1) { // Remove
            if (ref.find(val) != ref.end()) {
                st.remove(val);
                ref.erase(val);
            }
        } else { // Search
            bool found_st = st.search(val);
            bool found_ref = (ref.find(val) != ref.end());
            ASSERT_EQ(found_st, found_ref) << "Search mismatch for value " << val << " at iter " << i;
        }
    }
    
    // Final verify
    for (int i = 0; i <= range; ++i) {
        ASSERT_EQ(st.search(i), ref.count(i) > 0);
    }
}

// --- Shortest Path Differential Testing ---

TEST_F(RandomizedTest, DijkstraVsFloydWarshall) {
    int num_tests = 20;
    int n = 20;
    
    for (int t = 0; t < num_tests; ++t) {
        Graph g = make_random_graph_internal(n, 0.3, true, 1, 100);
        ShortestPath sp(n);
        
        // Copy edges to SP solver and manual adj matrix for FW
        std::vector<std::vector<long long>> dist_fw(n, std::vector<long long>(n, 1e18));
        for (int i = 0; i < n; ++i) dist_fw[i][i] = 0;
        
        for (int i = 0; i < n; ++i) {
            Edge* e = g.get_edges(i);
            while (e) {
                sp.add_edge(i, e->to, e->weight);
                dist_fw[i][e->to] = std::min(dist_fw[i][e->to], e->weight);
                e = e->next;
            }
        }
        
        // Floyd-Warshall Ground Truth
        for (int k = 0; k < n; ++k) {
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    if (dist_fw[i][k] != 1e18 && dist_fw[k][j] != 1e18) {
                        dist_fw[i][j] = std::min(dist_fw[i][j], dist_fw[i][k] + dist_fw[k][j]);
                    }
                }
            }
        }
        
        // Verify Dijkstra for each source
        for (int i = 0; i < n; ++i) {
            std::vector<long long> d_dijk = sp.dijkstra(i, 1e18);
            for (int j = 0; j < n; ++j) {
                ASSERT_EQ(d_dijk[j], dist_fw[i][j]) << "Mismatch at test " << t << " source " << i << " dest " << j;
            }
        }
        
        // Verify Bellman-Ford
        for (int i = 0; i < n; ++i) {
            bool has_cycle = false;
            std::vector<long long> d_bf = sp.bellman_ford(i, 1e18, has_cycle);
            ASSERT_FALSE(has_cycle);
            for (int j = 0; j < n; ++j) {
                ASSERT_EQ(d_bf[j], dist_fw[i][j]) << "BF Mismatch at test " << t << " source " << i << " dest " << j;
            }
        }
    }
}

// --- MST Differential Testing ---

TEST_F(RandomizedTest, KruskalVsPrim) {
    int num_tests = 50;
    int n = 30;
    
    for (int t = 0; t < num_tests; ++t) {
        // Generate connected graph
        Graph g = make_random_graph_internal(n, 0.4, false, 1, 100);
        // Ensure connectivity by adding a path 0-1-2...n-1
        for (int i = 0; i < n - 1; ++i) {
            g.add_edge(i, i + 1, rand_int(1, 100));
            g.add_edge(i + 1, i, rand_int(1, 100)); // Undirected, though core usually handles it.
            // Note: graph_core's add_edge is directed. For MST we need undirected semantics usually.
            // But MST algorithms in this lib take `Graph` or edge list.
            // MST::kruskal takes vector<MstEdge>.
            // MST::prim takes adj list.
        }
        
        std::vector<MstEdge> edges;
        std::vector<std::vector<std::pair<int, long long>>> adj(n);
        
        for (int i = 0; i < n; ++i) {
            Edge* e = g.get_edges(i);
            while (e) {
                if (i < e->to) { // Add each undirected edge once for Kruskal
                    edges.push_back({i, e->to, e->weight});
                }
                // For Prim, we need full adj
                adj[i].push_back({e->to, e->weight});
                // Check if reverse exists in graph structure? 
                // The random generator above adds edges i->j. 
                // If we want undirected, we should assume the input graph has both directions or the alg handles it.
                // MST::prim usually expects undirected graph represented as adj list.
                // Let's ensure symmetry in adj for Prim.
                // The generator `make_random_graph_internal(..., false, ...)` generates i->j only if i < j?
                // Wait, my helper `make_random_graph_internal` with `directed=false` only adds i->j where i!=j.
                // Actually `!directed && i > j` continue means it only adds i->j where i < j.
                // So g has only one direction.
                // We need to feed Prim bidirectional edges.
                adj[e->to].push_back({i, e->weight});
                
                e = e->next;
            }
            // Add the "ensure connectivity" edges which might be duplicated if we are not careful
            // The loop above adds i->i+1.
        }
        
        long long w_kruskal = MST::kruskal(n, edges);
        std::vector<Edge> prim_result; // Prim signature in header is static long long prim(int n, int start, const vector<vector...>>& adj, long long inf)
        long long w_prim = MST::prim(n, 0, adj, 1e18);
        
        ASSERT_EQ(w_kruskal, w_prim) << "MST weight mismatch at test " << t;
    }
}

// --- Max Flow Differential Testing ---

TEST_F(RandomizedTest, EdmondsKarpVsDinic) {
    int num_tests = 20;
    int n = 30;
    
    for (int t = 0; t < num_tests; ++t) {
        MaxFlow mf1(n);
        MaxFlow mf2(n);
        
        // Random edges
        int m = rand_int(n, n*n/3);
        for (int i = 0; i < m; ++i) {
            int u = rand_int(0, n-1);
            int v = rand_int(0, n-1);
            if (u == v) continue;
            int cap = rand_int(1, 100);
            
            mf1.add_edge(u, v, cap);
            mf2.add_edge(u, v, cap);
        }
        
        int s = 0, sink = n - 1;
        long long flow1 = mf1.edmonds_karp(s, sink);
        long long flow2 = mf2.dinic(s, sink);
        
        ASSERT_EQ(flow1, flow2) << "Max Flow mismatch at test " << t;
    }
}

// --- LCA Fuzzing ---

TEST_F(RandomizedTest, LCAVsNaive) {
    int num_tests = 20;
    int n = 50;
    
    for (int t = 0; t < num_tests; ++t) {
        TreeLCA tree(n);
        std::vector<int> parent(n, -1);
        std::vector<int> depth(n, 0);
        
        // Build random tree: i connects to random node in 0..i-1
        for (int i = 1; i < n; ++i) {
            int p = rand_int(0, i - 1);
            tree.add_edge(p, i);
            parent[i] = p;
            depth[i] = depth[p] + 1;
        }
        
        tree.build(0);
        
        // Naive LCA
        auto naive_lca = [&](int u, int v) {
            while (depth[u] > depth[v]) u = parent[u];
            while (depth[v] > depth[u]) v = parent[v];
            while (u != v) {
                u = parent[u];
                v = parent[v];
            }
            return u;
        };
        
        for (int k = 0; k < 100; ++k) {
            int u = rand_int(0, n - 1);
            int v = rand_int(0, n - 1);
            ASSERT_EQ(tree.lca(u, v), naive_lca(u, v));
            ASSERT_EQ(tree.distance(u, v), depth[u] + depth[v] - 2 * depth[naive_lca(u, v)]);
        }
    }
}

// --- General Matching Correctness ---
// Verify matching on random graphs by checking:
// 1. Matched vertices are disjoint.
// 2. Matched edges exist in graph.
// 3. Size is plausible (can't easily check maximality for large N without slow alg, but we can trust the algo if invariants hold and it passes standard tests).
// We will test on small N with brute force check if possible, or just invariants.
// Let's implement a small brute force for N=8.

TEST_F(RandomizedTest, GeneralMatchingSmallBruteForce) {
    int num_tests = 50;
    int n = 8; // Small enough for bitmask DP or brute force
    
    for (int t = 0; t < num_tests; ++t) {
        GeneralMatching gm(n);
        std::vector<std::pair<int, int>> edges;
        
        int m = rand_int(0, n * (n - 1) / 2);
        for (int i = 0; i < m; ++i) {
            int u = rand_int(0, n - 1);
            int v = rand_int(0, n - 1);
            if (u != v) {
                gm.add_edge(u, v);
                edges.push_back({std::min(u, v), std::max(u, v)});
            }
        }
        
        int algo_result = gm.maximum_matching();
        
        // Brute force max matching
        int max_match = 0;
        int num_edges = edges.size();
        for (int i = 0; i < (1 << num_edges); ++i) {
            int current_match = 0;
            int mask = 0;
            bool valid = true;
            for (int j = 0; j < num_edges; ++j) {
                if ((i >> j) & 1) {
                    int u = edges[j].first;
                    int v = edges[j].second;
                    if ((mask & (1 << u)) || (mask & (1 << v))) {
                        valid = false;
                        break;
                    }
                    mask |= (1 << u);
                    mask |= (1 << v);
                    current_match++;
                }
            }
            if (valid) {
                max_match = std::max(max_match, current_match);
            }
        }
        
        ASSERT_EQ(algo_result, max_match) << "Matching size mismatch at test " << t;
    }
}
