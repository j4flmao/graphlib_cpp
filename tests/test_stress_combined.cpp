#include <gtest/gtest.h>
#include <graphlib/graphlib.h>
#include <random>
#include <vector>
#include <algorithm>
#include <numeric>
#include <map>

#ifdef _MSC_VER
#include <intrin.h>
int count_trailing_zeros(unsigned int mask) {
    unsigned long index;
    if (_BitScanForward(&index, mask)) {
        return index;
    }
    return 32; // Should not happen if mask > 0
}
#else
int count_trailing_zeros(unsigned int mask) {
    return __builtin_ctz(mask);
}
#endif

using namespace graphlib;

// --- Helper for Brute Force Maximum Weight Matching (Bitmask DP) ---
// Returns the maximum weight of a matching in the graph.
long long brute_force_max_weight_matching(int n, const std::vector<std::tuple<int, int, int>>& edges) {
    // dp[mask] = max weight matching using vertices in mask
    // We want dp[(1<<n) - 1] (or max over all masks if partial matching allowed)
    // Actually, since we want max weight matching (not necessarily perfect),
    // we can iterate all masks.
    
    // However, it's easier to compute recursively with memoization.
    // solve(mask): max weight matching using subset of vertices in mask.
    
    std::vector<long long> memo(1 << n, -1);
    std::vector<std::vector<int>> adj_mat(n, std::vector<int>(n, -1e9)); // -1e9 represents no edge
    
    for(auto& e : edges) {
        int u = std::get<0>(e);
        int v = std::get<1>(e);
        int w = std::get<2>(e);
        adj_mat[u][v] = std::max(adj_mat[u][v], w);
        adj_mat[v][u] = std::max(adj_mat[v][u], w);
    }

    auto solve = [&](auto&& self, int mask) -> long long {
        if (mask == 0) return 0;
        if (memo[mask] != -1) return memo[mask];

        // Option 1: Don't match the first available vertex in mask
        int first = count_trailing_zeros(mask); // Get index of first set bit
        // Try not matching 'first' (if general matching allows leaving nodes unmatched)
        long long res = self(self, mask ^ (1 << first));

        // Option 2: Match 'first' with some 'next' in mask
        int submask = mask ^ (1 << first);
        while (submask > 0) {
            int next = count_trailing_zeros(submask);
            if (adj_mat[first][next] > -1e8) { // If edge exists
                res = std::max(res, (long long)adj_mat[first][next] + self(self, mask ^ (1 << first) ^ (1 << next)));
            }
            submask ^= (1 << next);
        }

        return memo[mask] = res;
    };

    return solve(solve, (1 << n) - 1);
}

// --- Stress Test: Max Weight Matching (Blossom vs DP) ---
TEST(StressCombined, MaxWeightMatching_BlossomVsDP) {
    int n = 12; // Small enough for DP (2^12 * 12^2 is tiny)
    int num_trials = 20;
    
    std::mt19937 rng(101);
    std::uniform_int_distribution<int> dist_w(-10, 50); // Allow some negative weights to test robustness
    std::uniform_real_distribution<double> dist_prob(0.0, 1.0);

    for (int t = 0; t < num_trials; ++t) {
        GeneralMatching g(n);
        std::vector<std::tuple<int, int, int>> edges;
        
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (dist_prob(rng) < 0.4) {
                    int w = dist_w(rng);
                    g.add_edge(i, j, w);
                    edges.emplace_back(i, j, w);
                }
            }
        }

        long long blossom_res = g.maximum_weight_matching();
        long long dp_res = brute_force_max_weight_matching(n, edges);

        ASSERT_EQ(blossom_res, dp_res) << "Mismatch in trial " << t;
    }
}

// --- Stress Test: MST (Prim vs Kruskal) ---
TEST(StressCombined, MST_PrimVsKruskal) {
    int n = 50;
    int num_trials = 20;
    long long inf = 1e18;
    
    std::mt19937 rng(202);
    std::uniform_int_distribution<int> dist_w(1, 100);
    std::uniform_real_distribution<double> dist_prob(0.0, 1.0);

    for (int t = 0; t < num_trials; ++t) {
        std::vector<MstEdge> edges;
        std::vector<std::vector<std::pair<int, long long>>> adj(n);
        
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (dist_prob(rng) < 0.3) {
                    int w = dist_w(rng);
                    edges.push_back({i, j, (long long)w});
                    adj[i].push_back({j, w});
                    adj[j].push_back({i, w});
                }
            }
        }
        
        // Ensure connectivity
        for (int i = 0; i < n - 1; ++i) {
            edges.push_back({i, i + 1, 1000});
            adj[i].push_back({i + 1, 1000});
            adj[i + 1].push_back({i, 1000});
        }

        long long prim_cost = MST::prim(n, 0, adj, inf);
        long long kruskal_cost = MST::kruskal(n, edges);

        ASSERT_EQ(prim_cost, kruskal_cost) << "MST mismatch in trial " << t;
    }
}

// --- Stress Test: Max Flow (Edmonds-Karp vs Dinic) ---
TEST(StressCombined, MaxFlow_EKVsDinic) {
    int n = 30;
    int num_trials = 20;
    
    std::mt19937 rng(303);
    std::uniform_int_distribution<int> dist_cap(1, 50);
    std::uniform_real_distribution<double> dist_prob(0.0, 1.0);

    for (int t = 0; t < num_trials; ++t) {
        MaxFlow mf1(n);
        MaxFlow mf2(n);
        
        int source = 0;
        int sink = n - 1;

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (i == j) continue;
                if (dist_prob(rng) < 0.2) {
                    int cap = dist_cap(rng);
                    mf1.add_edge(i, j, cap);
                    mf2.add_edge(i, j, cap);
                }
            }
        }

        long long ek_flow = mf1.edmonds_karp(source, sink);
        long long dinic_flow = mf2.dinic(source, sink);

        ASSERT_EQ(ek_flow, dinic_flow) << "Max Flow mismatch in trial " << t;
    }
}

// --- Stress Test: Connectivity (BFS vs Union-Find/Kosaraju) ---
TEST(StressCombined, Connectivity_BFSVsKosaraju) {
    int n = 50;
    int num_trials = 20;
    
    std::mt19937 rng(404);
    std::uniform_real_distribution<double> dist_prob(0.0, 1.0);

    for (int t = 0; t < num_trials; ++t) {
        Connectivity g(n); // Use Connectivity class
        SCC scc_solver(n); 
        
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (dist_prob(rng) < 0.1) {
                    g.add_edge(i, j);
                    
                    scc_solver.add_edge(i, j);
                    scc_solver.add_edge(j, i);
                }
            }
        }

        std::vector<int> comp_bfs;
        int bfs_components = g.connected_components(comp_bfs);
        
        std::vector<int> comp_scc;
        int scc_components = scc_solver.kosaraju(comp_scc);

        ASSERT_EQ(bfs_components, scc_components) << "Connectivity mismatch in trial " << t;
    }
}
