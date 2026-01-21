#include <gtest/gtest.h>
#include <graphlib/bipartite.h>
#include <graphlib/max_flow.h>
#include <random>
#include <vector>

using namespace graphlib;

TEST(BipartiteExtraTest, StressMaxMatching_Vs_MaxFlow) {
    int n_left = 50;
    int n_right = 50;
    int m = 500;
    int num_trials = 10;

    std::mt19937 rng(12345);
    std::uniform_int_distribution<int> dist_u(0, n_left - 1);
    std::uniform_int_distribution<int> dist_v(0, n_right - 1);

    for (int t = 0; t < num_trials; ++t) {
        BipartiteGraph bg(n_left, n_right);
        MaxFlow mf_solver(n_left + n_right + 2);
        int source = n_left + n_right;
        int sink = source + 1;

        // Edges from source to left
        for (int i = 0; i < n_left; ++i) {
            mf_solver.add_edge(source, i, 1);
        }

        // Edges from right to sink
        for (int i = 0; i < n_right; ++i) {
            mf_solver.add_edge(n_left + i, sink, 1);
        }

        // Random edges between left and right
        for (int i = 0; i < m; ++i) {
            int u = dist_u(rng);
            int v = dist_v(rng);
            bg.add_edge(u, n_left + v);
            mf_solver.add_edge(u, n_left + v, 1);
        }

        int matching_bg = bg.maximum_matching();
        int matching_mf = mf_solver.dinic(source, sink);

        EXPECT_EQ(matching_bg, matching_mf) << "Mismatch in trial " << t;
    }
}

TEST(BipartiteExtraTest, StressHungarian_Vs_MCMF) {
    // Hungarian finds Min Cost Perfect Matching (or Max Weight Perfect Matching depending on implementation)
    // The library signature is `hungarian_min_cost(long long inf)`.
    // We assume it finds Min Cost Maximum Matching (or Perfect if possible).
    // Let's test on equal sizes where perfect matching is likely.

    int n = 20; // 20 left, 20 right
    int num_trials = 5;
    long long inf = 1e18;

    std::mt19937 rng(54321);
    std::uniform_int_distribution<int> dist_cost(1, 100);

    for (int t = 0; t < num_trials; ++t) {
        BipartiteGraph bg(n, n);
        MaxFlow mcmf_solver(2 * n + 2);
        int source = 2 * n;
        int sink = 2 * n + 1;

        // Source -> Left (cap 1, cost 0)
        for (int i = 0; i < n; ++i) {
            mcmf_solver.add_edge(source, i, 1, 0);
        }

        // Right -> Sink (cap 1, cost 0)
        for (int i = 0; i < n; ++i) {
            mcmf_solver.add_edge(n + i, sink, 1, 0);
        }

        // Complete bipartite graph with random costs
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                int cost = dist_cost(rng);
                bg.add_edge(i, n + j, cost);
                mcmf_solver.add_edge(i, n + j, 1, cost);
            }
        }

        // Hungarian Result
        long long cost_hungarian = bg.hungarian_min_cost(inf);

        // MCMF Result
        // min_cost_max_flow returns pair {flow, cost}
        std::pair<long long, long long> res = mcmf_solver.min_cost_max_flow(source, sink);
        long long cost_mcmf = res.second;

        EXPECT_EQ(cost_hungarian, cost_mcmf) << "Mismatch in trial " << t;
    }
}
