#include "graphlib/motif.h"
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <numeric>
#include <queue>
#include <functional>

namespace graphlib {

namespace {
    struct PairHash {
        size_t operator()(const std::pair<int, int>& p) const {
            return std::hash<long long>()(static_cast<long long>(p.first) * 1000000 + p.second);
        }
    };
}

TriangleResult count_triangles(const Graph& g) {
    int n = g.vertex_count();
    TriangleResult result;
    result.total_triangles = 0;
    result.per_vertex.resize(n, 0);
    
    if (n == 0) return result;
    
    // Compute degrees
    std::vector<int> degree(n, 0);
    for (int u = 0; u < n; ++u) {
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            degree[u]++;
        }
    }
    
    // Build adjacency set for fast lookup
    std::vector<std::unordered_set<int>> adj(n);
    for (int u = 0; u < n; ++u) {
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            adj[u].insert(e->to);
        }
    }
    
    // Create degree ordering
    std::vector<int> order(n);
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(), [&](int a, int b) {
        return degree[a] < degree[b] || (degree[a] == degree[b] && a < b);
    });
    
    std::vector<int> rank(n);
    for (int i = 0; i < n; ++i) {
        rank[order[i]] = i;
    }
    
    // Count triangles using node-iterator algorithm
    for (int u = 0; u < n; ++u) {
        std::vector<int> higher_neighbors;
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            if (rank[e->to] > rank[u]) {
                higher_neighbors.push_back(e->to);
            }
        }
        
        for (size_t i = 0; i < higher_neighbors.size(); ++i) {
            int v = higher_neighbors[i];
            for (size_t j = i + 1; j < higher_neighbors.size(); ++j) {
                int w = higher_neighbors[j];
                if (adj[v].count(w)) {
                    result.total_triangles++;
                    result.per_vertex[u]++;
                    result.per_vertex[v]++;
                    result.per_vertex[w]++;
                }
            }
        }
    }
    
    return result;
}

std::vector<std::tuple<int, int, int>> list_triangles(const Graph& g, int max_triangles) {
    int n = g.vertex_count();
    std::vector<std::tuple<int, int, int>> triangles;
    
    if (n == 0) return triangles;
    
    // Build adjacency set
    std::vector<std::unordered_set<int>> adj(n);
    for (int u = 0; u < n; ++u) {
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            adj[u].insert(e->to);
        }
    }
    
    for (int u = 0; u < n; ++u) {
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            int v = e->to;
            if (u >= v) continue;
            
            for (Edge* f = g.get_edges(v); f; f = f->next) {
                int w = f->to;
                if (v >= w) continue;
                
                if (adj[u].count(w)) {
                    triangles.emplace_back(u, v, w);
                    if (max_triangles > 0 && static_cast<int>(triangles.size()) >= max_triangles) {
                        return triangles;
                    }
                }
            }
        }
    }
    
    return triangles;
}

long long count_4cliques(const Graph& g) {
    int n = g.vertex_count();
    if (n < 4) return 0;
    
    long long count = 0;
    
    // Build adjacency set and matrix
    std::vector<std::unordered_set<int>> adj(n);
    for (int u = 0; u < n; ++u) {
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            adj[u].insert(e->to);
        }
    }
    
    // For each edge (u, v), count common neighbors with higher indices
    for (int u = 0; u < n; ++u) {
        std::vector<int> neighbors;
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            if (e->to > u) {
                neighbors.push_back(e->to);
            }
        }
        
        for (size_t i = 0; i < neighbors.size(); ++i) {
            int v = neighbors[i];
            for (size_t j = i + 1; j < neighbors.size(); ++j) {
                int w = neighbors[j];
                if (!adj[v].count(w)) continue;
                
                // Triangle (u, v, w) found, look for 4th vertex
                for (size_t k = j + 1; k < neighbors.size(); ++k) {
                    int x = neighbors[k];
                    if (adj[v].count(x) && adj[w].count(x)) {
                        count++;
                    }
                }
            }
        }
    }
    
    return count;
}

long long count_k_cliques(const Graph& g, int k) {
    if (k <= 0) return 0;
    if (k == 1) return g.vertex_count();
    
    int n = g.vertex_count();
    if (n < k) return 0;
    
    if (k == 2) {
        long long edges = 0;
        for (int u = 0; u < n; ++u) {
            for (Edge* e = g.get_edges(u); e; e = e->next) {
                if (u < e->to) edges++;
            }
        }
        return edges;
    }
    
    if (k == 3) {
        return count_triangles(g).total_triangles;
    }
    
    if (k == 4) {
        return count_4cliques(g);
    }
    
    // General case: enumerate using backtracking
    std::vector<std::unordered_set<int>> adj(n);
    for (int u = 0; u < n; ++u) {
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            adj[u].insert(e->to);
        }
    }
    
    long long count = 0;
    std::vector<int> clique;
    clique.reserve(k);
    
    std::function<void(int, std::vector<int>&)> enumerate;
    enumerate = [&](int start, std::vector<int>& candidates) {
        if (static_cast<int>(clique.size()) == k) {
            count++;
            return;
        }
        
        for (size_t i = 0; i < candidates.size(); ++i) {
            int v = candidates[i];
            clique.push_back(v);
            
            // Build new candidate list
            std::vector<int> new_candidates;
            for (size_t j = i + 1; j < candidates.size(); ++j) {
                int u = candidates[j];
                bool connected = true;
                for (int c : clique) {
                    if (!adj[c].count(u)) {
                        connected = false;
                        break;
                    }
                }
                if (connected) {
                    new_candidates.push_back(u);
                }
            }
            
            enumerate(v + 1, new_candidates);
            clique.pop_back();
        }
    };
    
    std::vector<int> all_vertices(n);
    std::iota(all_vertices.begin(), all_vertices.end(), 0);
    enumerate(0, all_vertices);
    
    return count;
}

std::map<MotifType, long long> count_3node_motifs(const Graph& g) {
    std::map<MotifType, long long> counts;
    for (int i = static_cast<int>(MotifType::M1); i <= static_cast<int>(MotifType::M13); ++i) {
        counts[static_cast<MotifType>(i)] = 0;
    }
    
    int n = g.vertex_count();
    if (n < 3 || !g.is_directed()) return counts;
    
    // Build adjacency matrix
    std::vector<std::vector<int>> adj_matrix(n, std::vector<int>(n, 0));
    // 0 = no edge, 1 = forward, 2 = backward, 3 = bidirectional
    
    for (int u = 0; u < n; ++u) {
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            adj_matrix[u][e->to] |= 1;
            adj_matrix[e->to][u] |= 2;
        }
    }
    
    // Enumerate all triples
    for (int a = 0; a < n; ++a) {
        for (int b = a + 1; b < n; ++b) {
            for (int c = b + 1; c < n; ++c) {
                // Classify the motif based on edges
                int ab = adj_matrix[a][b];
                int ac = adj_matrix[a][c];
                int bc = adj_matrix[b][c];
                
                // Convert to bidirectional representation
                bool ab_fwd = (ab & 1) != 0;
                bool ab_bwd = (ab & 2) != 0;
                bool ac_fwd = (ac & 1) != 0;
                bool ac_bwd = (ac & 2) != 0;
                bool bc_fwd = (bc & 1) != 0;
                bool bc_bwd = (bc & 2) != 0;
                
                int edge_count = 0;
                if (ab_fwd || ab_bwd) edge_count++;
                if (ac_fwd || ac_bwd) edge_count++;
                if (bc_fwd || bc_bwd) edge_count++;
                
                // Skip if not connected enough
                if (edge_count < 2) continue;
                
                // This is a simplified classification - full motif census is complex
                // Counting based on bidirectionality
                int bidirectional_count = 0;
                if (ab_fwd && ab_bwd) bidirectional_count++;
                if (ac_fwd && ac_bwd) bidirectional_count++;
                if (bc_fwd && bc_bwd) bidirectional_count++;
                
                if (edge_count == 3 && bidirectional_count == 3) {
                    counts[MotifType::M13]++;
                } else if (edge_count == 3 && bidirectional_count == 2) {
                    counts[MotifType::M11]++;
                } else if (edge_count == 3 && bidirectional_count == 1) {
                    counts[MotifType::M8]++;
                } else if (edge_count == 3 && bidirectional_count == 0) {
                    // Check for cycle
                    bool is_cycle = (ab_fwd && bc_fwd && ac_bwd) || 
                                   (ab_bwd && bc_bwd && ac_fwd);
                    if (is_cycle) {
                        counts[MotifType::M6]++;
                    } else {
                        counts[MotifType::M4]++;
                    }
                } else if (edge_count == 2) {
                    if (bidirectional_count == 2) {
                        counts[MotifType::M9]++;
                    } else if (bidirectional_count == 1) {
                        counts[MotifType::M5]++;
                    } else {
                        // All unidirectional
                        counts[MotifType::M1]++;
                    }
                }
            }
        }
    }
    
    return counts;
}

std::vector<long long> count_graphlets(const Graph& g, int k) {
    if (k < 2 || k > 5) return {};
    
    std::vector<long long> counts;
    
    if (k == 2) {
        // Just edges
        long long edges = 0;
        int n = g.vertex_count();
        for (int u = 0; u < n; ++u) {
            for (Edge* e = g.get_edges(u); e; e = e->next) {
                if (u < e->to) edges++;
            }
        }
        counts.push_back(edges);
        return counts;
    }
    
    if (k == 3) {
        // G0: 3 isolated vertices (not typically counted)
        // G1: edge + isolated vertex (P2)
        // G2: path of 3 vertices (P3)
        // G3: triangle (K3)
        auto tri_result = count_triangles(g);
        counts.push_back(tri_result.total_triangles);  // triangles
        
        // Count paths of length 2
        int n = g.vertex_count();
        long long paths = 0;
        for (int v = 0; v < n; ++v) {
            int deg = 0;
            for (Edge* e = g.get_edges(v); e; e = e->next) deg++;
            paths += static_cast<long long>(deg) * (deg - 1) / 2;
        }
        paths -= 3 * tri_result.total_triangles;  // Subtract triangles
        counts.insert(counts.begin(), paths);
        
        return counts;
    }
    
    // For k=4 and k=5, implement simplified counting
    counts.push_back(count_k_cliques(g, k));
    
    return counts;
}

std::vector<std::vector<long long>> graphlet_degree_distribution(const Graph& g, int max_size) {
    int n = g.vertex_count();
    std::vector<std::vector<long long>> gdd(n);
    
    // Simplified: just return triangle participation for now
    auto tri_result = count_triangles(g);
    for (int i = 0; i < n; ++i) {
        gdd[i].push_back(tri_result.per_vertex[i]);
    }
    
    return gdd;
}

std::vector<std::vector<int>> find_motif_occurrences(const Graph& g, const Graph& pattern, int max_matches) {
    std::vector<std::vector<int>> matches;
    
    int n = g.vertex_count();
    int k = pattern.vertex_count();
    
    if (k > n || k == 0) return matches;
    
    // Build pattern adjacency
    std::vector<std::unordered_set<int>> pattern_adj(k);
    for (int u = 0; u < k; ++u) {
        for (Edge* e = pattern.get_edges(u); e; e = e->next) {
            pattern_adj[u].insert(e->to);
        }
    }
    
    // Build graph adjacency
    std::vector<std::unordered_set<int>> graph_adj(n);
    for (int u = 0; u < n; ++u) {
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            graph_adj[u].insert(e->to);
        }
    }
    
    // Backtracking search
    std::vector<int> mapping(k, -1);
    std::vector<bool> used(n, false);
    
    std::function<void(int)> search;
    search = [&](int pattern_vertex) {
        if (max_matches > 0 && static_cast<int>(matches.size()) >= max_matches) return;
        
        if (pattern_vertex == k) {
            matches.push_back(mapping);
            return;
        }
        
        for (int v = 0; v < n; ++v) {
            if (used[v]) continue;
            
            // Check if v is compatible
            bool compatible = true;
            for (int i = 0; i < pattern_vertex && compatible; ++i) {
                int u = mapping[i];
                bool pattern_edge = pattern_adj[i].count(pattern_vertex) > 0;
                bool graph_edge = graph_adj[u].count(v) > 0;
                
                if (pattern.is_directed()) {
                    bool pattern_edge_rev = pattern_adj[pattern_vertex].count(i) > 0;
                    bool graph_edge_rev = graph_adj[v].count(u) > 0;
                    if (pattern_edge != graph_edge || pattern_edge_rev != graph_edge_rev) {
                        compatible = false;
                    }
                } else {
                    if (pattern_edge != graph_edge) {
                        compatible = false;
                    }
                }
            }
            
            if (compatible) {
                mapping[pattern_vertex] = v;
                used[v] = true;
                search(pattern_vertex + 1);
                used[v] = false;
            }
        }
    };
    
    search(0);
    
    return matches;
}

double transitivity(const Graph& g) {
    int n = g.vertex_count();
    if (n < 3) return 0.0;
    
    auto tri_result = count_triangles(g);
    
    // Count connected triples (paths of length 2)
    long long triples = 0;
    for (int v = 0; v < n; ++v) {
        int deg = 0;
        for (Edge* e = g.get_edges(v); e; e = e->next) deg++;
        triples += static_cast<long long>(deg) * (deg - 1) / 2;
    }
    
    if (triples == 0) return 0.0;
    
    return 3.0 * tri_result.total_triangles / triples;
}

long long count_paths_of_length(const Graph& g, int u, int v, int k) {
    if (k < 0) return 0;
    if (k == 0) return (u == v) ? 1 : 0;
    
    int n = g.vertex_count();
    
    // Use matrix exponentiation concept with DP
    std::vector<std::vector<long long>> dp(k + 1, std::vector<long long>(n, 0));
    dp[0][u] = 1;
    
    for (int step = 1; step <= k; ++step) {
        for (int x = 0; x < n; ++x) {
            if (dp[step - 1][x] == 0) continue;
            for (Edge* e = g.get_edges(x); e; e = e->next) {
                dp[step][e->to] += dp[step - 1][x];
            }
        }
    }
    
    return dp[k][v];
}

std::vector<long long> count_simple_paths(const Graph& g, int max_length) {
    int n = g.vertex_count();
    std::vector<long long> counts(max_length + 1, 0);
    
    if (n == 0) return counts;
    
    // For each starting vertex, do DFS
    for (int start = 0; start < n; ++start) {
        std::vector<bool> visited(n, false);
        
        std::function<void(int, int)> dfs;
        dfs = [&](int u, int len) {
            counts[len]++;
            if (len >= max_length) return;
            
            for (Edge* e = g.get_edges(u); e; e = e->next) {
                if (!visited[e->to]) {
                    visited[e->to] = true;
                    dfs(e->to, len + 1);
                    visited[e->to] = false;
                }
            }
        };
        
        visited[start] = true;
        dfs(start, 0);
    }
    
    // Paths of length 0 are just vertices
    counts[0] = n;
    
    // For undirected graphs, divide by 2 (each path counted twice)
    if (!g.is_directed()) {
        for (int i = 1; i <= max_length; ++i) {
            counts[i] /= 2;
        }
    }
    
    return counts;
}

} // namespace graphlib
