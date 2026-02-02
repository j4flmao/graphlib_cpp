#include "graphlib/cycle_basis.h"
#include "graphlib/shortest_path.h"
#include "graphlib/mst.h"
#include <algorithm>
#include <queue>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <limits>
#include <bitset>

namespace graphlib {

namespace {
    struct EdgeKey {
        int u, v;
        EdgeKey(int a, int b) : u(std::min(a, b)), v(std::max(a, b)) {}
        bool operator<(const EdgeKey& other) const {
            return u < other.u || (u == other.u && v < other.v);
        }
        bool operator==(const EdgeKey& other) const {
            return u == other.u && v == other.v;
        }
    };
    
    struct EdgeKeyHash {
        size_t operator()(const EdgeKey& k) const {
            return std::hash<long long>()(static_cast<long long>(k.u) * 1000000 + k.v);
        }
    };
    
    // BFS shortest path from source
    std::pair<std::vector<int>, std::vector<long long>> bfs_paths(const Graph& g, int source) {
        int n = g.vertex_count();
        std::vector<int> parent(n, -1);
        std::vector<long long> dist(n, std::numeric_limits<long long>::max());
        
        std::queue<int> q;
        q.push(source);
        dist[source] = 0;
        parent[source] = source;
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            
            for (Edge* e = g.get_edges(u); e; e = e->next) {
                if (dist[e->to] == std::numeric_limits<long long>::max()) {
                    dist[e->to] = dist[u] + e->weight;
                    parent[e->to] = u;
                    q.push(e->to);
                }
            }
        }
        
        return {parent, dist};
    }
    
    // Reconstruct path from parent array
    std::vector<int> reconstruct(int source, int target, const std::vector<int>& parent) {
        if (parent[target] == -1) return {};
        
        std::vector<int> path;
        for (int v = target; v != source; v = parent[v]) {
            path.push_back(v);
        }
        path.push_back(source);
        std::reverse(path.begin(), path.end());
        return path;
    }
    
    // Convert path to edge set
    std::set<EdgeKey> path_to_edges(const std::vector<int>& path) {
        std::set<EdgeKey> edges;
        for (size_t i = 0; i + 1 < path.size(); ++i) {
            edges.insert(EdgeKey(path[i], path[i + 1]));
        }
        return edges;
    }
    
    // XOR two edge sets
    std::set<EdgeKey> xor_edge_sets(const std::set<EdgeKey>& a, const std::set<EdgeKey>& b) {
        std::set<EdgeKey> result;
        for (const auto& e : a) {
            if (b.find(e) == b.end()) result.insert(e);
        }
        for (const auto& e : b) {
            if (a.find(e) == a.end()) result.insert(e);
        }
        return result;
    }
    
    // Check linear independence using Gaussian elimination on edge vectors
    bool is_linearly_independent(const std::vector<std::set<EdgeKey>>& basis,
                                  const std::set<EdgeKey>& cycle,
                                  const std::map<EdgeKey, int>& edge_to_idx) {
        if (basis.empty()) return !cycle.empty();
        
        int num_edges = static_cast<int>(edge_to_idx.size());
        std::vector<std::bitset<10000>> matrix(basis.size() + 1);
        
        for (size_t i = 0; i < basis.size(); ++i) {
            for (const auto& e : basis[i]) {
                auto it = edge_to_idx.find(e);
                if (it != edge_to_idx.end()) {
                    matrix[i].set(it->second);
                }
            }
        }
        
        for (const auto& e : cycle) {
            auto it = edge_to_idx.find(e);
            if (it != edge_to_idx.end()) {
                matrix[basis.size()].set(it->second);
            }
        }
        
        // Gaussian elimination
        int row = 0;
        for (int col = 0; col < num_edges && row < static_cast<int>(matrix.size()); ++col) {
            int pivot = -1;
            for (int r = row; r < static_cast<int>(matrix.size()); ++r) {
                if (matrix[r].test(col)) {
                    pivot = r;
                    break;
                }
            }
            if (pivot == -1) continue;
            
            std::swap(matrix[row], matrix[pivot]);
            for (int r = 0; r < static_cast<int>(matrix.size()); ++r) {
                if (r != row && matrix[r].test(col)) {
                    matrix[r] ^= matrix[row];
                }
            }
            ++row;
        }
        
        // The new cycle is independent if it wasn't reduced to zero
        return matrix[basis.size()].any();
    }
}

std::vector<Cycle> minimum_cycle_basis(const Graph& g) {
    int n = g.vertex_count();
    if (n == 0) return {};
    
    // Collect all edges and build edge index
    std::vector<std::tuple<int, int, long long>> all_edges;
    std::set<EdgeKey> edge_set;
    
    for (int u = 0; u < n; ++u) {
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            EdgeKey key(u, e->to);
            if (edge_set.find(key) == edge_set.end()) {
                edge_set.insert(key);
                all_edges.emplace_back(u, e->to, e->weight);
            }
        }
    }
    
    int m = static_cast<int>(all_edges.size());
    if (m == 0) return {};
    
    // Map edges to indices
    std::map<EdgeKey, int> edge_to_idx;
    int idx = 0;
    for (const auto& [u, v, w] : all_edges) {
        edge_to_idx[EdgeKey(u, v)] = idx++;
    }
    
    // Number of independent cycles = m - n + c (where c = connected components)
    // For connected graph: m - n + 1
    
    // Generate candidate cycles using Horton's method:
    // For each vertex v and each edge (x, y), consider cycle:
    // shortest_path(v, x) + edge(x, y) + shortest_path(y, v)
    
    std::vector<std::pair<long long, std::set<EdgeKey>>> candidates;
    
    for (int v = 0; v < n; ++v) {
        auto [parent, dist] = bfs_paths(g, v);
        
        for (const auto& [x, y, w] : all_edges) {
            if (dist[x] == std::numeric_limits<long long>::max() ||
                dist[y] == std::numeric_limits<long long>::max()) continue;
            
            // Cycle: path(v->x) + edge(x,y) + path(y->v)
            auto path_vx = reconstruct(v, x, parent);
            auto path_yv = reconstruct(v, y, parent);
            std::reverse(path_yv.begin(), path_yv.end());
            
            // Combine paths
            std::set<EdgeKey> cycle_edges = path_to_edges(path_vx);
            for (size_t i = 0; i + 1 < path_yv.size(); ++i) {
                cycle_edges.insert(EdgeKey(path_yv[i], path_yv[i + 1]));
            }
            cycle_edges.insert(EdgeKey(x, y));
            
            // Remove edges that appear twice (XOR)
            std::set<EdgeKey> final_cycle;
            std::map<EdgeKey, int> count;
            for (const auto& e : path_to_edges(path_vx)) count[e]++;
            for (size_t i = 0; i + 1 < path_yv.size(); ++i) {
                count[EdgeKey(path_yv[i], path_yv[i + 1])]++;
            }
            count[EdgeKey(x, y)]++;
            
            for (const auto& [e, c] : count) {
                if (c % 2 == 1) final_cycle.insert(e);
            }
            
            if (!final_cycle.empty()) {
                long long weight = dist[x] + dist[y] + w;
                candidates.emplace_back(weight, final_cycle);
            }
        }
    }
    
    // Sort candidates by weight
    std::sort(candidates.begin(), candidates.end());
    
    // Greedily select linearly independent cycles
    std::vector<std::set<EdgeKey>> basis_edges;
    std::vector<Cycle> result;
    
    for (const auto& [weight, cycle_edges] : candidates) {
        if (is_linearly_independent(basis_edges, cycle_edges, edge_to_idx)) {
            basis_edges.push_back(cycle_edges);
            
            Cycle c;
            c.weight = weight;
            for (const auto& e : cycle_edges) {
                c.edges.emplace_back(e.u, e.v);
            }
            result.push_back(c);
            
            // Check if we have enough cycles
            // For connected graph: need m - n + 1 cycles
            if (static_cast<int>(result.size()) >= m - n + 1) break;
        }
    }
    
    return result;
}

std::vector<Cycle> fundamental_cycles(const Graph& g) {
    int n = g.vertex_count();
    if (n == 0) return {};
    
    // Find spanning tree using BFS
    std::vector<bool> visited(n, false);
    std::vector<int> parent(n, -1);
    std::set<EdgeKey> tree_edges;
    
    std::queue<int> q;
    q.push(0);
    visited[0] = true;
    
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            if (!visited[e->to]) {
                visited[e->to] = true;
                parent[e->to] = u;
                tree_edges.insert(EdgeKey(u, e->to));
                q.push(e->to);
            }
        }
    }
    
    // For each non-tree edge, find the fundamental cycle
    std::vector<Cycle> result;
    std::set<EdgeKey> processed;
    
    for (int u = 0; u < n; ++u) {
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            EdgeKey key(u, e->to);
            if (processed.count(key)) continue;
            processed.insert(key);
            
            if (tree_edges.find(key) != tree_edges.end()) continue;
            
            // Non-tree edge: find cycle
            Cycle c;
            c.weight = e->weight;
            c.edges.emplace_back(u, e->to);
            
            // Find path from u to e->to in tree
            std::vector<int> path_u, path_v;
            for (int v = u; v != -1; v = parent[v]) path_u.push_back(v);
            for (int v = e->to; v != -1; v = parent[v]) path_v.push_back(v);
            
            // Find LCA
            std::set<int> ancestors_u(path_u.begin(), path_u.end());
            int lca = -1;
            for (int v : path_v) {
                if (ancestors_u.count(v)) {
                    lca = v;
                    break;
                }
            }
            
            // Add edges from u to LCA
            for (int v = u; v != lca; v = parent[v]) {
                c.edges.emplace_back(v, parent[v]);
                // Find weight
                for (Edge* edge = g.get_edges(v); edge; edge = edge->next) {
                    if (edge->to == parent[v]) {
                        c.weight += edge->weight;
                        break;
                    }
                }
            }
            
            // Add edges from e->to to LCA
            for (int v = e->to; v != lca; v = parent[v]) {
                c.edges.emplace_back(v, parent[v]);
                for (Edge* edge = g.get_edges(v); edge; edge = edge->next) {
                    if (edge->to == parent[v]) {
                        c.weight += edge->weight;
                        break;
                    }
                }
            }
            
            result.push_back(c);
        }
    }
    
    return result;
}

long long count_cycles(const Graph& g, int max_length) {
    int n = g.vertex_count();
    if (n == 0 || max_length < 3) return 0;
    
    // For undirected graphs, use DFS-based cycle detection
    long long count = 0;
    
    // Build adjacency matrix for fast lookup
    std::vector<std::vector<bool>> adj(n, std::vector<bool>(n, false));
    for (int u = 0; u < n; ++u) {
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            adj[u][e->to] = true;
        }
    }
    
    // Enumerate cycles using DFS
    std::function<void(int, int, int, std::vector<bool>&)> dfs;
    dfs = [&](int start, int curr, int len, std::vector<bool>& visited) {
        if (len > max_length) return;
        
        for (int next = start; next < n; ++next) {
            if (!adj[curr][next]) continue;
            
            if (next == start && len >= 3) {
                count++;
            } else if (!visited[next] && next > start) {
                visited[next] = true;
                dfs(start, next, len + 1, visited);
                visited[next] = false;
            }
        }
    };
    
    std::vector<bool> visited(n, false);
    for (int i = 0; i < n; ++i) {
        visited[i] = true;
        dfs(i, i, 1, visited);
        visited[i] = false;
    }
    
    // Each cycle is counted once for each starting vertex and direction
    // For undirected: divide by 2
    if (!g.is_directed()) {
        count /= 2;
    }
    
    return count;
}

std::vector<std::vector<int>> find_all_cycles(const Graph& g, int max_cycles) {
    int n = g.vertex_count();
    if (n == 0) return {};
    
    std::vector<std::vector<int>> result;
    
    // Johnson's algorithm for finding all elementary cycles
    std::vector<std::vector<int>> adj(n);
    for (int u = 0; u < n; ++u) {
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            adj[u].push_back(e->to);
        }
    }
    
    std::vector<bool> blocked(n, false);
    std::vector<std::set<int>> B(n);
    std::vector<int> stack;
    
    std::function<bool(int, int)> circuit;
    circuit = [&](int v, int s) -> bool {
        if (max_cycles > 0 && static_cast<int>(result.size()) >= max_cycles) return false;
        
        bool found = false;
        stack.push_back(v);
        blocked[v] = true;
        
        for (int w : adj[v]) {
            if (w == s) {
                result.push_back(stack);
                found = true;
            } else if (!blocked[w]) {
                if (circuit(w, s)) found = true;
            }
        }
        
        if (found) {
            std::function<void(int)> unblock;
            unblock = [&](int u) {
                blocked[u] = false;
                for (int w : B[u]) {
                    if (blocked[w]) unblock(w);
                }
                B[u].clear();
            };
            unblock(v);
        } else {
            for (int w : adj[v]) {
                B[w].insert(v);
            }
        }
        
        stack.pop_back();
        return found;
    };
    
    for (int s = 0; s < n; ++s) {
        if (max_cycles > 0 && static_cast<int>(result.size()) >= max_cycles) break;
        
        // Reset
        for (int i = 0; i < n; ++i) {
            blocked[i] = false;
            B[i].clear();
        }
        
        circuit(s, s);
        
        // Remove s from graph for next iteration
        for (int u = 0; u < n; ++u) {
            adj[u].erase(std::remove(adj[u].begin(), adj[u].end(), s), adj[u].end());
        }
        adj[s].clear();
    }
    
    return result;
}

int girth(const Graph& g) {
    int n = g.vertex_count();
    if (n == 0) return -1;
    
    int min_girth = std::numeric_limits<int>::max();
    
    // BFS from each vertex to find shortest cycle
    for (int start = 0; start < n; ++start) {
        std::vector<int> dist(n, -1);
        std::vector<int> parent(n, -1);
        std::queue<int> q;
        
        q.push(start);
        dist[start] = 0;
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            
            for (Edge* e = g.get_edges(u); e; e = e->next) {
                int v = e->to;
                if (dist[v] == -1) {
                    dist[v] = dist[u] + 1;
                    parent[v] = u;
                    q.push(v);
                } else if (parent[u] != v && parent[v] != u) {
                    // Found a cycle
                    int cycle_len = dist[u] + dist[v] + 1;
                    min_girth = std::min(min_girth, cycle_len);
                }
            }
        }
    }
    
    return min_girth == std::numeric_limits<int>::max() ? -1 : min_girth;
}

std::vector<int> longest_cycle(const Graph& g) {
    int n = g.vertex_count();
    if (n == 0) return {};
    
    if (n <= 20) {
        // Exact algorithm using bitmask DP (Hamiltonian path variant)
        const long long INF = 1e18;
        std::vector<std::vector<long long>> dp(1 << n, std::vector<long long>(n, INF));
        std::vector<std::vector<int>> parent(1 << n, std::vector<int>(n, -1));
        
        // Build adjacency
        std::vector<std::vector<bool>> adj(n, std::vector<bool>(n, false));
        for (int u = 0; u < n; ++u) {
            for (Edge* e = g.get_edges(u); e; e = e->next) {
                adj[u][e->to] = true;
            }
        }
        
        // Initialize
        for (int i = 0; i < n; ++i) {
            dp[1 << i][i] = 0;
        }
        
        // DP
        for (int mask = 1; mask < (1 << n); ++mask) {
            for (int u = 0; u < n; ++u) {
                if (!(mask & (1 << u))) continue;
                if (dp[mask][u] == INF) continue;
                
                for (int v = 0; v < n; ++v) {
                    if (mask & (1 << v)) continue;
                    if (!adj[u][v]) continue;
                    
                    int new_mask = mask | (1 << v);
                    if (dp[new_mask][v] > dp[mask][u] + 1) {
                        dp[new_mask][v] = dp[mask][u] + 1;
                        parent[new_mask][v] = u;
                    }
                }
            }
        }
        
        // Find longest cycle
        int best_len = 0;
        int best_mask = 0;
        int best_end = -1;
        int best_start = -1;
        
        for (int mask = 1; mask < (1 << n); ++mask) {
            int len = __builtin_popcount(mask);
            if (len < 3) continue;
            
            for (int end = 0; end < n; ++end) {
                if (!(mask & (1 << end))) continue;
                if (dp[mask][end] == INF) continue;
                
                // Find start of path
                int start = -1;
                for (int s = 0; s < n; ++s) {
                    if ((mask & (1 << s)) && dp[1 << s][s] == 0) {
                        // Check if this was the start
                        int cur_mask = mask;
                        int cur = end;
                        while (parent[cur_mask][cur] != -1) {
                            int prev = parent[cur_mask][cur];
                            cur_mask ^= (1 << cur);
                            cur = prev;
                        }
                        if (cur == s && adj[end][s]) {
                            start = s;
                            break;
                        }
                    }
                }
                
                if (start != -1 && len > best_len) {
                    best_len = len;
                    best_mask = mask;
                    best_end = end;
                    best_start = start;
                }
            }
        }
        
        if (best_len == 0) return {};
        
        // Reconstruct path
        std::vector<int> cycle;
        int cur_mask = best_mask;
        int cur = best_end;
        
        while (parent[cur_mask][cur] != -1) {
            cycle.push_back(cur);
            int prev = parent[cur_mask][cur];
            cur_mask ^= (1 << cur);
            cur = prev;
        }
        cycle.push_back(cur);
        
        std::reverse(cycle.begin(), cycle.end());
        return cycle;
    }
    
    // For larger graphs, use heuristic
    std::vector<int> best_cycle;
    
    // Try DFS from each vertex
    for (int start = 0; start < n; ++start) {
        std::vector<bool> visited(n, false);
        std::vector<int> path;
        
        std::function<void(int)> dfs = [&](int u) {
            path.push_back(u);
            visited[u] = true;
            
            for (Edge* e = g.get_edges(u); e; e = e->next) {
                if (e->to == start && path.size() >= 3) {
                    if (path.size() > best_cycle.size()) {
                        best_cycle = path;
                    }
                } else if (!visited[e->to]) {
                    dfs(e->to);
                }
            }
            
            path.pop_back();
            visited[u] = false;
        };
        
        dfs(start);
    }
    
    return best_cycle;
}

} // namespace graphlib
