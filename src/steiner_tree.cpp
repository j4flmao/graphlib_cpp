#include "graphlib/steiner_tree.h"
#include "graphlib/shortest_path.h"
#include <algorithm>
#include <limits>
#include <queue>

namespace graphlib {

long long steiner_tree(const Graph& g, const std::vector<int>& terminals) {
    if (terminals.empty()) return 0;
    if (terminals.size() == 1) return 0;

    int n = g.vertex_count();
    int k = static_cast<int>(terminals.size());
    
    // DP state: dp[mask][v] = min cost to connect subset 'mask' of terminals, rooted at 'v'
    // where v is part of the tree.
    // Initialize with infinity
    const long long INF = std::numeric_limits<long long>::max() / 2;
    std::vector<std::vector<long long>> dp(1 << k, std::vector<long long>(n, INF));

    // Base cases: terminals themselves
    for (int i = 0; i < k; ++i) {
        dp[1 << i][terminals[i]] = 0;
    }

    for (int mask = 1; mask < (1 << k); ++mask) {
        // 1. Combine submasks
        for (int v = 0; v < n; ++v) {
            for (int submask = (mask - 1) & mask; submask > 0; submask = (submask - 1) & mask) {
                if (dp[submask][v] != INF && dp[mask ^ submask][v] != INF) {
                    dp[mask][v] = std::min(dp[mask][v], dp[submask][v] + dp[mask ^ submask][v]);
                }
            }
        }

        // 2. Extend edges (SPFA / Dijkstra)
        // Using SPFA (queue optimized Bellman-Ford) since it's often faster for this DP on sparse graphs
        // or just Dijkstra. Dijkstra is safer for positive weights.
        // Assuming non-negative weights for Dijkstra.
        
        using State = std::pair<long long, int>;
        std::priority_queue<State, std::vector<State>, std::greater<State>> pq;

        for (int v = 0; v < n; ++v) {
            if (dp[mask][v] != INF) {
                pq.push({dp[mask][v], v});
            }
        }

        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();

            if (d > dp[mask][u]) continue;

            Edge* e = g.get_edges(u);
            while (e) {
                int v = e->to;
                long long w = e->weight;
                if (dp[mask][u] + w < dp[mask][v]) {
                    dp[mask][v] = dp[mask][u] + w;
                    pq.push({dp[mask][v], v});
                }
                e = e->next;
            }
        }
    }

    long long ans = INF;
    // The answer is min(dp[(1<<k)-1][v]) for any v
    for (int v = 0; v < n; ++v) {
        ans = std::min(ans, dp[(1 << k) - 1][v]);
    }

    return (ans == INF) ? -1 : ans;
}

}
