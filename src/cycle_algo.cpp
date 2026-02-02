#include "graphlib/cycle_algo.h"
#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>

namespace graphlib {

double minimum_mean_cycle(const Graph& g, std::vector<int>* cycle) {
    int n = g.vertex_count();
    if (n == 0) {
        if (cycle) cycle->clear();
        return std::numeric_limits<double>::infinity();
    }

    std::vector<std::vector<double>> d(n + 1, std::vector<double>(n, std::numeric_limits<double>::infinity()));
    std::vector<std::vector<int>> parent(n + 1, std::vector<int>(n, -1));

    for (int i = 0; i < n; ++i) {
        d[0][i] = 0;
    }

    for (int k = 1; k <= n; ++k) {
        bool changed = false;
        for (int u = 0; u < n; ++u) {
            if (d[k-1][u] == std::numeric_limits<double>::infinity()) continue;

            Edge* e = g.get_edges(u);
            while (e) {
                int v = e->to;
                if (d[k-1][u] + e->weight < d[k][v]) {
                    d[k][v] = d[k-1][u] + e->weight;
                    parent[k][v] = u;
                    changed = true;
                }
                e = e->next;
            }
        }
        if (!changed) {
            bool any_path = false;
            for (int i = 0; i < n; ++i) {
                if (d[k][i] < std::numeric_limits<double>::infinity()) {
                    any_path = true;
                    break;
                }
            }
            if (!any_path) {
                if (cycle) cycle->clear();
                return std::numeric_limits<double>::infinity();
            }
        }
    }

    double min_avg = std::numeric_limits<double>::infinity();
    int best_v = -1;
    int best_k = -1;

    for (int v = 0; v < n; ++v) {
        if (d[n][v] == std::numeric_limits<double>::infinity()) continue;

        double max_val = -std::numeric_limits<double>::infinity();
        int max_k = -1;
        
        for (int k = 0; k < n; ++k) {
            if (d[k][v] == std::numeric_limits<double>::infinity()) continue;
            
            double val = (d[n][v] - d[k][v]) / (double)(n - k);
            if (val > max_val) {
                max_val = val;
                max_k = k;
            }
        }
        
        if (max_val < min_avg) {
            min_avg = max_val;
            best_v = v;
            best_k = max_k;
        }
    }

    if (best_v == -1) {
        if (cycle) cycle->clear();
        return std::numeric_limits<double>::infinity();
    }

    if (cycle) {
        cycle->clear();
        std::vector<int> path;
        int cur = best_v;
        for (int k = n; k > best_k; --k) {
            path.push_back(cur);
            cur = parent[k][cur];
        }
        
        std::vector<bool> visited(n, false);
        std::vector<int> pos(n, -1);
        for (int i = 0; i < (int)path.size(); ++i) {
            int v = path[i];
            if (visited[v]) {
                for (int j = pos[v]; j >= i; --j) {
                    cycle->push_back(path[j]);
                }
                break;
            }
            visited[v] = true;
            pos[v] = i;
        }
    }

    return min_avg;
}

}
