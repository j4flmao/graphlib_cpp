#include "graphlib/cycle_algo.h"
#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>

namespace graphlib {

double minimum_mean_cycle(const Graph& g) {
    int n = g.vertex_count();
    if (n == 0) return std::numeric_limits<double>::infinity();

    // d[k][v] = shortest path weight of length k ending at v
    // We treat "start" as a virtual source connected to all nodes with weight 0.
    // Effectively d[0][v] = 0 for all v.
    
    // Use vector of vectors. O(N^2) space.
    // For n=5000, this is ~200MB. Acceptable for "Extreme" library.
    // If n is very large, this algorithm is not suitable (O(VE) time anyway).
    
    std::vector<std::vector<double>> d(n + 1, std::vector<double>(n, std::numeric_limits<double>::infinity()));

    for (int i = 0; i < n; ++i) {
        d[0][i] = 0;
    }

    for (int k = 1; k <= n; ++k) {
        bool changed = false;
        for (int u = 0; u < n; ++u) {
            // If d[k-1][u] is infinity, we can't extend from it
            if (d[k-1][u] == std::numeric_limits<double>::infinity()) continue;

            Edge* e = g.get_edges(u);
            while (e) {
                int v = e->to;
                if (d[k-1][u] + e->weight < d[k][v]) {
                    d[k][v] = d[k-1][u] + e->weight;
                    changed = true;
                }
                e = e->next;
            }
        }
        // Optimization: if no path of length k exists or no improvement, 
        // implies no cycles reachable? No, standard Bellman-Ford early exit doesn't apply directly
        // because we need exact length k.
        // If not changed, it means no path of length k exists? 
        // Yes, if d[k] is all infinity.
        // But d[0] is 0. So paths exist.
        // If d[k] didn't change from infinity, then no path of length k.
        // But d[k] starts at infinity.
        if (!changed) {
            // If we can't make a path of length k, we can't make length n.
            // But this happens only if max path length < k. i.e. DAG.
            // If DAG, no cycle.
            // But we check changed flag relative to initialization? 
            // The loop updates d[k] based on d[k-1].
            // We can check if all d[k][v] are infinity.
            bool any_path = false;
            for(int i=0; i<n; ++i) if (d[k][i] < std::numeric_limits<double>::infinity()) any_path = true;
            if (!any_path) return std::numeric_limits<double>::infinity();
        }
    }

    double min_avg = std::numeric_limits<double>::infinity();
    bool cycle_found = false;

    for (int v = 0; v < n; ++v) {
        if (d[n][v] == std::numeric_limits<double>::infinity()) continue;

        double max_val = -std::numeric_limits<double>::infinity();
        
        for (int k = 0; k < n; ++k) {
            if (d[k][v] == std::numeric_limits<double>::infinity()) continue;
            
            double val = (d[n][v] - d[k][v]) / (double)(n - k);
            if (val > max_val) {
                max_val = val;
            }
        }
        
        if (max_val < min_avg) {
            min_avg = max_val;
            cycle_found = true;
        }
    }

    if (!cycle_found) return std::numeric_limits<double>::infinity();
    return min_avg;
}

}
