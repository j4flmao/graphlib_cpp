#include "graphlib/bipartite.h"
#include "graphlib/max_flow.h"
#include <queue>
#include <algorithm>
#include <limits>
#include <functional>

namespace graphlib {

BipartiteGraph::BipartiteGraph(int n_left, int n_right) 
    : Graph(n_left + n_right, false), n_left_(n_left), n_right_(n_right) {}

bool BipartiteGraph::is_bipartite() const {
    return !graphlib::is_bipartite(*this).empty();
}

long long BipartiteGraph::hungarian_min_cost(long long max_cost_limit) {
    (void)max_cost_limit;
    int source = vertex_count();
    int sink = source + 1;
    MaxFlow mf(sink + 1);
    
    // Edges from source to left partition
    for (int i = 0; i < n_left_; ++i) {
        mf.add_edge(source, i, 1, 0);
    }
    
    // Edges from right partition to sink
    for (int i = n_left_; i < n_left_ + n_right_; ++i) {
        mf.add_edge(i, sink, 1, 0);
    }
    
    // Edges between partitions (from left to right)
    for (int u = 0; u < n_left_; ++u) {
        Edge* e = get_edges(u);
        while (e) {
            int v = e->to;
            // Only consider edges to the right partition
            if (v >= n_left_ && v < n_left_ + n_right_) {
                mf.add_edge(u, v, 1, e->weight);
            }
            e = e->next;
        }
    }
    
    std::pair<long long, long long> result = mf.min_cost_max_flow(source, sink);
    return result.second;
}

int BipartiteGraph::maximum_matching() {
    std::vector<int> match = hopcroft_karp_matching(*this);
    int count = 0;
    for (int x : match) {
        if (x != -1) count++;
    }
    return count / 2;
}

GRAPHLIB_API std::vector<int> is_bipartite(const Graph& g) {
    int n = g.vertex_count();
    std::vector<int> color(n, -1);

    for (int i = 0; i < n; ++i) {
        if (color[i] != -1) continue;

        std::queue<int> q;
        q.push(i);
        color[i] = 0;

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            Edge* e = g.get_edges(u);
            while (e) {
                int v = e->to;
                if (color[v] == -1) {
                    color[v] = 1 - color[u];
                    q.push(v);
                } else if (color[v] == color[u]) {
                    return {}; // Not bipartite
                }
                e = e->next;
            }
        }
    }
    return color;
}

GRAPHLIB_API std::vector<int> hopcroft_karp_matching(const Graph& g) {
    int n = g.vertex_count();
    std::vector<int> partition = is_bipartite(g);
    if (partition.empty()) {
        return {}; // Not bipartite
    }

    // pair_u[u] stores the vertex in V matched with u in U
    // pair_v[v] stores the vertex in U matched with v in V
    std::vector<int> pair_u(n, -1); 
    std::vector<int> pair_v(n, -1); 
    std::vector<int> dist(n);

    std::vector<int> U;
    for(int i=0; i<n; ++i) {
        if(partition[i] == 0) U.push_back(i);
    }

    // Ptr array for DFS optimization (Dinic-style)
    std::vector<Edge*> ptr(n);

    while (true) {
        // BFS to find shortest augmenting paths
        std::queue<int> q;
        for (int u : U) {
            if (pair_u[u] == -1) {
                dist[u] = 0;
                q.push(u);
            } else {
                dist[u] = std::numeric_limits<int>::max();
            }
        }

        int max_dist = std::numeric_limits<int>::max();

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            if (dist[u] < max_dist) {
                for (Edge* e = g.get_edges(u); e != nullptr; e = e->next) {
                    int v = e->to;
                    if (partition[v] == 0) continue; // Skip edges within same partition (shouldn't happen in bipartite)

                    if (pair_v[v] == -1) {
                        if (max_dist == std::numeric_limits<int>::max()) {
                            max_dist = dist[u] + 1;
                        }
                    } else {
                        int next_u = pair_v[v];
                        if (dist[next_u] == std::numeric_limits<int>::max()) {
                            dist[next_u] = dist[u] + 1;
                            q.push(next_u);
                        }
                    }
                }
            }
        }

        if (max_dist == std::numeric_limits<int>::max()) break;

        // DFS with ptr optimization
        for (int i = 0; i < n; ++i) ptr[i] = g.get_edges(i);

        std::function<bool(int)> dfs;
        dfs = [&](int u) -> bool {
            for (Edge*& e = ptr[u]; e != nullptr; e = e->next) {
                int v = e->to;
                if (partition[v] == 0) continue;

                if (pair_v[v] == -1) {
                    // Found an unmatched vertex in V
                    // It must be at the correct distance
                    if (dist[u] + 1 == max_dist) {
                        pair_v[v] = u;
                        pair_u[u] = v;
                        return true;
                    }
                } else {
                    int next_u = pair_v[v];
                    if (dist[next_u] == dist[u] + 1) {
                        if (dfs(next_u)) {
                            pair_v[v] = u;
                            pair_u[u] = v;
                            return true;
                        }
                    }
                }
            }
            return false;
        };

        for (int u : U) {
            if (pair_u[u] == -1) {
                dfs(u);
            }
        }
    }

    std::vector<int> result(n, -1);
    for (int i = 0; i < n; ++i) {
        if (partition[i] == 0 && pair_u[i] != -1) {
            result[i] = pair_u[i];
            result[pair_u[i]] = i;
        } else if (partition[i] == 1 && pair_v[i] != -1) {
            result[i] = pair_v[i];
            result[pair_v[i]] = i;
        }
    }
    return result;
}

GRAPHLIB_API std::vector<int> max_bipartite_matching_dfs(const Graph& g) {
    int n = g.vertex_count();
    std::vector<int> partition = is_bipartite(g);
    if (partition.empty()) return {};

    std::vector<int> match(n, -1);
    std::vector<bool> vis;
    
    std::function<bool(int)> dfs;
    dfs = [&](int u) {
        Edge* e = g.get_edges(u);
        while (e) {
            int v = e->to;
            if (!vis[v]) {
                vis[v] = true;
                if (match[v] < 0 || dfs(match[v])) {
                    match[v] = u;
                    return true;
                }
            }
            e = e->next;
        }
        return false;
    };

    int result = 0;
    // Iterate only over one partition (e.g. 0)
    for (int i = 0; i < n; ++i) {
        if (partition[i] == 0) {
            vis.assign(n, false);
            if (dfs(i)) {
                result++;
            }
        }
    }
    
    // Construct symmetric result
    std::vector<int> final_match(n, -1);
    for (int i = 0; i < n; ++i) {
        if (partition[i] == 1 && match[i] != -1) {
            final_match[i] = match[i];
            final_match[match[i]] = i;
        }
    }
    return final_match;
}

}
