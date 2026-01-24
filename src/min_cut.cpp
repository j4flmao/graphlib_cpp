#include "graphlib/min_cut.h"
#include "graphlib/max_flow.h"
#include <vector>
#include <algorithm>
#include <numeric>
#include <limits>
#include <stdexcept>

namespace graphlib {

// Implementation of Stoer-Wagner Algorithm
long long global_min_cut_undirected(const Graph& g) {
    if (g.is_directed()) {
        throw std::invalid_argument("global_min_cut_undirected requires an undirected graph");
    }

    int n = g.vertex_count();
    if (n <= 1) {
        return 0;
    }

    std::vector<std::vector<long long>> w(n, std::vector<long long>(n, 0));
    for (int u = 0; u < n; u++) {
        Edge* e = g.get_edges(u);
        while (e) {
            int v = e->to;
            if (u <= v && u != v) {
                long long weight = e->weight;
                w[u][v] += weight;
                w[v][u] += weight;
            }
            e = e->next;
        }
    }

    std::vector<int> vertices(n);
    for (int i = 0; i < n; i++) {
        vertices[i] = i;
    }

    long long best = std::numeric_limits<long long>::max();
    std::vector<long long> agg(n);
    std::vector<char> added(n);

    int size = n;
    while (size > 1) {
        for (int i = 0; i < n; i++) {
            agg[i] = 0;
            added[i] = 0;
        }

        int prev = -1;
        // int last = -1; // Unused variable

        for (int i = 0; i < size; i++) {
            int select_index = -1;
            for (int j = 0; j < size; j++) {
                int v = vertices[j];
                if (!added[v] && (select_index == -1 || agg[v] > agg[vertices[select_index]])) {
                    select_index = j;
                }
            }

            int v = vertices[select_index];
            added[v] = 1;

            if (i == size - 1) {
                // last = v;
                if (agg[v] < best) {
                    best = agg[v];
                }
                if (prev != -1) {
                    for (int j = 0; j < n; j++) {
                        w[prev][j] += w[v][j];
                        w[j][prev] = w[prev][j];
                    }
                    vertices.erase(vertices.begin() + select_index);
                    size--;
                }
            } else {
                prev = v;
                for (int j = 0; j < n; j++) {
                    if (!added[j]) {
                        agg[j] += w[v][j];
                    }
                }
            }
        }
    }

    if (best == std::numeric_limits<long long>::max()) {
        return 0;
    }
    return best;
}

// Implementation of Gomory-Hu Tree
void gomory_hu_tree(const Graph& g, std::vector<int>& parent, std::vector<long long>& min_cut) {
    if (g.is_directed()) {
        throw std::invalid_argument("gomory_hu_tree requires an undirected graph");
    }

    int n = g.vertex_count();
    parent.assign(n, 0);
    min_cut.assign(n, 0);

    if (n <= 1) {
        if (n == 1) {
            parent[0] = 0;
            min_cut[0] = 0;
        }
        return;
    }

    // Precompute adjacency matrix for faster capacity lookups or just use original graph
    // Since we need to build MaxFlow repeatedly, we can optimize.
    // However, for clarity and reusing MaxFlow class, we'll build MaxFlow from scratch or reset it.
    // The MaxFlow class in this library doesn't have a 'clear' or 'reset' method exposed easily that keeps memory,
    // so we will instantiate it inside the loop.

    // To avoid O(E) per iteration when adding edges, let's prepare the edges once.
    struct SimpleEdge {
        int u, v;
        long long w;
    };
    std::vector<SimpleEdge> edges;
    for (int u = 0; u < n; u++) {
        Edge* e = g.get_edges(u);
        while (e) {
            if (u < e->to) { // Add each undirected edge once
                edges.push_back({u, e->to, e->weight});
            }
            e = e->next;
        }
    }

    parent[0] = 0;
    for (int i = 1; i < n; i++) {
        parent[i] = 0;
    }

    for (int s = 1; s < n; s++) {
        int t = parent[s];
        if (t == s) {
            // Should not happen if logic is correct
            continue;
        }

        MaxFlow mf(n);
        for (const auto& e : edges) {
             mf.add_undirected_edge(e.u, e.v, e.w);
        }

        long long flow = mf.dinic(s, t);
        min_cut[s] = flow;

        std::vector<char> reachable;
        mf.min_cut_reachable_from_source(s, reachable);

        for (int i = s + 1; i < n; ++i) {
            if (parent[i] == t && reachable[i]) {
                parent[i] = s;
            }
        }
    }
}

} // namespace graphlib
