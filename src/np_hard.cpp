#include "graphlib/np_hard.h"
#include "graphlib/shortest_path.h"
#include "graphlib/general_matching.h"
#include "graphlib/mst.h"
#include <vector>
#include <algorithm>
#include <numeric>
#include <set>
#include <limits>
#include <cmath>
#include <stack>
#include <queue>
#include <map>

namespace graphlib {

// -----------------------------------------------------------------------------
// Traveling Salesperson Problem (TSP) using Bitmask DP
// -----------------------------------------------------------------------------

long long tsp_bitmask(const Graph& g) {
    int n = g.vertex_count();
    if (n == 0) return 0;
    if (n == 1) return 0;
    if (n > 20) return -1; // Too large for bitmask DP
    
    // Build adjacency matrix
    long long INF = 1000000000000000LL; // Sufficiently large
    std::vector<std::vector<long long>> dist(n, std::vector<long long>(n, INF));
    
    for(int u=0; u<n; ++u) {
        Edge* e = g.get_edges(u);
        while(e) {
            // Keep minimum weight if multi-edges
            if (e->weight < dist[u][e->to]) {
                dist[u][e->to] = e->weight;
            }
            e = e->next;
        }
    }
    
    // dp[mask][last_visited]
    std::vector<std::vector<long long>> dp(1LL << n, std::vector<long long>(n, INF));
    
    // Start at vertex 0
    dp[1][0] = 0;
    
    for(long long mask=1; mask < (1LL<<n); ++mask) {
        for(int u=0; u<n; ++u) {
            if((mask >> u) & 1) {
                if(dp[mask][u] == INF) continue;
                
                for(int v=0; v<n; ++v) {
                    if(!((mask >> v) & 1)) {
                        if(dist[u][v] != INF) {
                            long long next_mask = mask | (1LL << v);
                            dp[next_mask][v] = std::min(dp[next_mask][v], dp[mask][u] + dist[u][v]);
                        }
                    }
                }
            }
        }
    }
    
    long long ans = INF;
    long long full_mask = (1LL << n) - 1;
    // Try to return to 0 from any node
    for(int u=1; u<n; ++u) {
        if(dp[full_mask][u] != INF && dist[u][0] != INF) {
            ans = std::min(ans, dp[full_mask][u] + dist[u][0]);
        }
    }
    
    return ans == INF ? -1 : ans;
}

// -----------------------------------------------------------------------------
// Maximum Clique (Bron-Kerbosch Algorithm)
// -----------------------------------------------------------------------------

namespace {
    void bron_kerbosch_recursive(const std::vector<std::vector<bool>>& adj,
                       std::vector<int>& R, std::vector<int>& P, std::vector<int>& X,
                       int& max_size) {
        if (P.empty() && X.empty()) {
            max_size = std::max(max_size, (int)R.size());
            return;
        }

        // Pivot selection to minimize branching
        int pivot = -1;
        if (!P.empty()) pivot = P[0];
        else if (!X.empty()) pivot = X[0];
        
        // Use vector copy for iteration to allow modification of P/X
        std::vector<int> P_copy = P;
        
        for (int v : P_copy) {
            if (pivot != -1 && adj[pivot][v]) continue; // Skip neighbors of pivot
            
            std::vector<int> newR = R;
            newR.push_back(v);
            
            std::vector<int> newP;
            std::vector<int> newX;
            
            for (int p : P) {
                if (adj[v][p]) newP.push_back(p);
            }
            for (int x : X) {
                if (adj[v][x]) newX.push_back(x);
            }
            
            bron_kerbosch_recursive(adj, newR, newP, newX, max_size);
            
            // P = P \ {v}, X = X U {v}
            auto it = std::find(P.begin(), P.end(), v);
            if (it != P.end()) P.erase(it);
            X.push_back(v);
        }
    }
}

int max_clique_bron_kerbosch(const Graph& g) {
    int n = g.vertex_count();
    if (n == 0) return 0;

    std::vector<std::vector<bool>> adj(n, std::vector<bool>(n, false));
    for (int u = 0; u < n; ++u) {
        Edge* e = g.get_edges(u);
        while (e) {
            if (e->to != u) adj[u][e->to] = true;
            e = e->next;
        }
    }
    
    std::vector<int> R, P(n), X;
    std::iota(P.begin(), P.end(), 0);
    
    int max_size = 0;
    bron_kerbosch_recursive(adj, R, P, X, max_size);
    return max_size;
}

// -----------------------------------------------------------------------------
// Maximum Weight Clique
// -----------------------------------------------------------------------------

namespace {
    void max_weight_clique_recursive(const std::vector<std::vector<bool>>& adj,
                                     const std::vector<long long>& weights,
                                     std::vector<int>& R, std::vector<int>& P, std::vector<int>& X,
                                     long long current_weight,
                                     long long& max_weight) {
        if (P.empty() && X.empty()) {
            if (current_weight > max_weight) {
                max_weight = current_weight;
            }
            return;
        }

        long long potential = current_weight;
        for (int v : P) potential += weights[v];
        if (potential <= max_weight) return;

        int pivot = -1;
        if (!P.empty()) pivot = P[0];
        else if (!X.empty()) pivot = X[0];

        std::vector<int> P_copy = P;

        for (int v : P_copy) {
            if (pivot != -1 && adj[pivot][v]) continue;

            std::vector<int> newR = R;
            newR.push_back(v);

            std::vector<int> newP;
            std::vector<int> newX;

            for (int p : P) if (adj[v][p]) newP.push_back(p);
            for (int x : X) if (adj[v][x]) newX.push_back(x);

            max_weight_clique_recursive(adj, weights, newR, newP, newX, current_weight + weights[v], max_weight);

            auto it_p = std::find(P.begin(), P.end(), v);
            if (it_p != P.end()) P.erase(it_p);
            X.push_back(v);
        }
    }
}

long long max_weight_clique(const Graph& g, const std::vector<long long>& weights) {
    int n = g.vertex_count();
    if (n == 0) return 0;
    
    std::vector<std::vector<bool>> adj(n, std::vector<bool>(n, false));
    for (int u = 0; u < n; ++u) {
        Edge* e = g.get_edges(u);
        while (e) {
            if (e->to != u) adj[u][e->to] = true;
            e = e->next;
        }
    }

    std::vector<int> R, P(n), X;
    std::iota(P.begin(), P.end(), 0);

    long long max_weight = 0;
    max_weight_clique_recursive(adj, weights, R, P, X, 0, max_weight);
    return max_weight;
}

// -----------------------------------------------------------------------------
// Graph Coloring
// -----------------------------------------------------------------------------

int greedy_coloring(const Graph& g, std::vector<int>& colors) {
    int n = g.vertex_count();
    colors.assign(n, -1);
    
    // Sort vertices by degree descending (Welsh-Powell)
    std::vector<int> nodes(n);
    std::iota(nodes.begin(), nodes.end(), 0);
    std::vector<int> degrees(n, 0);
    
    for(int u=0; u<n; ++u) {
        Edge* e = g.get_edges(u);
        while(e) {
            degrees[u]++;
            e = e->next;
        }
    }
    
    std::sort(nodes.begin(), nodes.end(), [&](int a, int b) {
        return degrees[a] > degrees[b];
    });
    
    int max_color = 0;
    for(int u : nodes) {
        std::set<int> neighbor_colors;
        Edge* e = g.get_edges(u);
        while(e) {
            if(colors[e->to] != -1) {
                neighbor_colors.insert(colors[e->to]);
            }
            e = e->next;
        }
        
        int color = 0;
        while(neighbor_colors.count(color)) {
            color++;
        }
        colors[u] = color;
        max_color = std::max(max_color, color + 1);
    }
    
    return max_color;
}

int dsatur_coloring(const Graph& g, std::vector<int>& colors) {
    int n = g.vertex_count();
    colors.assign(n, -1);
    std::vector<int> degrees(n, 0);
    std::vector<std::set<int>> sat(n); // Saturation degrees (set of neighbor colors)
    
    for(int u=0; u<n; ++u) {
        Edge* e = g.get_edges(u);
        while(e) {
            degrees[u]++;
            e = e->next;
        }
    }
    
    std::set<std::pair<int, int>> pq; // (-saturation, -degree, u) but use custom logic
    // Just simple O(N^2) loop is fine for dense graphs, or use heap with lazy updates.
    // For simplicity O(N^2) selection
    
    std::vector<bool> colored(n, false);
    int colored_cnt = 0;
    
    // Initial: node with max degree
    int start_node = 0;
    for(int i=1; i<n; ++i) if (degrees[i] > degrees[start_node]) start_node = i;
    
    // Use priority queue?
    // Let's just pick max saturation node each time.
    
    int max_color = 0;
    
    for (int i = 0; i < n; ++i) {
        int u = -1;
        int max_sat = -1;
        int max_deg = -1;
        
        for (int v = 0; v < n; ++v) {
            if (!colored[v]) {
                int s = (int)sat[v].size();
                if (s > max_sat) {
                    max_sat = s;
                    max_deg = degrees[v];
                    u = v;
                } else if (s == max_sat) {
                    if (degrees[v] > max_deg) {
                        max_deg = degrees[v];
                        u = v;
                    }
                }
            }
        }
        
        if (u == -1) break; // Should not happen
        
        colored[u] = true;
        
        // Find first available color
        int color = 0;
        while (sat[u].count(color)) {
            color++;
        }
        colors[u] = color;
        max_color = std::max(max_color, color + 1);
        
        // Update neighbors
        Edge* e = g.get_edges(u);
        while(e) {
            int v = e->to;
            if (!colored[v]) {
                sat[v].insert(color);
            }
            e = e->next;
        }
    }
    
    return max_color;
}

// -----------------------------------------------------------------------------
// Maximum Independent Set
// -----------------------------------------------------------------------------

int max_independent_set(const Graph& g) {
    // Max Independent Set on G is Max Clique on Complement(G)
    int n = g.vertex_count();
    Graph complement(n, false);
    
    std::vector<std::vector<bool>> adj(n, std::vector<bool>(n, false));
    for (int u = 0; u < n; ++u) {
        Edge* e = g.get_edges(u);
        while(e) {
            adj[u][e->to] = true;
            e = e->next;
        }
    }
    
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (!adj[i][j]) {
                complement.add_edge(i, j);
            }
        }
    }
    
    return max_clique_bron_kerbosch(complement);
}

// -----------------------------------------------------------------------------
// Chinese Postman Problem
// -----------------------------------------------------------------------------

long long chinese_postman(const Graph& g) {
    int n = g.vertex_count();
    
    // 1. Check connectivity and calculate total weight
    long long total_weight = 0;
    std::vector<int> degrees(n, 0);
    
    // We need to traverse all edges.
    // Sum weights (divide by 2 for undirected)
    for (int u = 0; u < n; ++u) {
        Edge* e = g.get_edges(u);
        while(e) {
            total_weight += e->weight;
            degrees[u]++;
            e = e->next;
        }
    }
    total_weight /= 2;
    
    // Check connectivity (ignoring isolated vertices? No, all edges must be traversed)
    // BFS from first vertex with degree > 0
    int start_node = -1;
    for(int i=0; i<n; ++i) if(degrees[i] > 0) { start_node = i; break; }
    
    if (start_node == -1) return 0; // No edges
    
    std::vector<bool> visited(n, false);
    std::queue<int> q;
    q.push(start_node);
    visited[start_node] = true;
    int visited_count = 0;
    
    while(!q.empty()) {
        int u = q.front(); q.pop();
        visited_count++;
        Edge* e = g.get_edges(u);
        while(e) {
            if(!visited[e->to]) {
                visited[e->to] = true;
                q.push(e->to);
            }
            e = e->next;
        }
    }
    
    for(int i=0; i<n; ++i) {
        if(degrees[i] > 0 && !visited[i]) return -1; // Disconnected component with edges
    }
    
    // 2. Identify odd degree vertices
    std::vector<int> odds;
    for(int i=0; i<n; ++i) {
        if(degrees[i] % 2 != 0) odds.push_back(i);
    }
    
    if (odds.empty()) return total_weight; // Eulerian
    
    // 3. Compute pairwise distances between odd vertices
    // Use Floyd-Warshall for simplicity if N is small, or repeated Dijkstra.
    // Assuming N is reasonable (< 1000?). Repeated Dijkstra is better O(K * E log V).
    
    int k = (int)odds.size();
    std::vector<std::vector<long long>> odd_dist(k, std::vector<long long>(k));
    
    ShortestPath sp(n);
    for(int u=0; u<n; ++u) {
        Edge* e = g.get_edges(u);
        while(e) {
            sp.add_edge(u, e->to, e->weight);
            e = e->next;
        }
    }
    long long INF = std::numeric_limits<long long>::max();

    for (int i = 0; i < k; ++i) {
        std::vector<long long> dist = sp.dijkstra(odds[i], INF);
        
        for (int j = 0; j < k; ++j) {
            odd_dist[i][j] = dist[odds[j]];
        }
    }
    
    // 4. Minimum Weight Perfect Matching on Complete Graph of odds
    GeneralMatching matcher(k);
    // Use negative weights for Maximum Weight Matching to simulate Min Weight
    // MWM maximizes sum(w). We want min sum(dist).
    // Let M = max_dist + 1. w' = M - dist.
    // min sum(dist) = max sum(M - dist) = max (k/2 * M - sum(dist))
    
    long long max_d = 0;
    for(int i=0; i<k; ++i) for(int j=0; j<k; ++j) if(odd_dist[i][j] != std::numeric_limits<long long>::max()) max_d = std::max(max_d, odd_dist[i][j]);
    
    long long M = max_d + 1;
    
    for (int i = 0; i < k; ++i) {
        for (int j = i + 1; j < k; ++j) {
            matcher.add_edge(i, j, M - odd_dist[i][j]);
        }
    }
    
    long long matching_weight = matcher.maximum_weight_matching();
    // matching_weight = (k/2)*M - min_cost
    long long min_cost = (long long)(k/2) * M - matching_weight;
    
    return total_weight + min_cost;
}

// -----------------------------------------------------------------------------
// Approximations
// -----------------------------------------------------------------------------

std::pair<long long, std::vector<int>> tsp_metric_approx(const Graph& g) {
    // 1. Compute MST
    int n = g.vertex_count();
    std::vector<MstEdge> edges;
    for (int u = 0; u < n; ++u) {
        Edge* e = g.get_edges(u);
        while (e) {
            if (u < e->to) { // Undirected, take each edge once
                edges.push_back({u, e->to, e->weight});
            }
            e = e->next;
        }
    }
    std::sort(edges.begin(), edges.end());
    
    UnionFind uf(n);
    std::vector<MstEdge> mst_edges;
    for (const auto& e : edges) {
        if (uf.unite(e.u, e.v)) {
            mst_edges.push_back(e);
        }
    }
    
    // 2. Build MST graph
    Graph mst_graph(n, false);
    for (const auto& e : mst_edges) {
        mst_graph.add_edge(e.u, e.v, e.weight);
    }
    
    // 3. Preorder traversal (DFS)
    std::vector<int> path;
    std::vector<bool> visited(n, false);
    std::vector<int> stack;
    stack.push_back(0);
    
    while(!stack.empty()) {
        int u = stack.back();
        stack.pop_back();
        
        if (!visited[u]) {
            visited[u] = true;
            path.push_back(u);
            
            // Push neighbors in reverse order to visit them in correct order?
            // Order doesn't strictly matter for approx ratio, but let's just push.
            Edge* e = mst_graph.get_edges(u);
            while(e) {
                if(!visited[e->to]) {
                    stack.push_back(e->to);
                }
                e = e->next;
            }
        }
    }
    
    if (path.size() != n) {
        return {-1, {}};
    }
    
    // 4. Calculate cost
    long long cost = 0;
    for (size_t i = 0; i < path.size(); ++i) {
        int u = path[i];
        int v = path[(i + 1) % path.size()];
        
        // Find edge weight u-v in original graph
        long long w = std::numeric_limits<long long>::max();
        Edge* e = g.get_edges(u);
        while(e) {
            if(e->to == v) {
                w = std::min(w, e->weight);
            }
            e = e->next;
        }
        
        if (w == std::numeric_limits<long long>::max()) {
            return {-1, {}};
        }
        cost += w;
    }
    
    return {cost, path};
}

std::vector<int> vertex_cover_approx(const Graph& g) {
    // Maximal Matching approximation
    int n = g.vertex_count();
    std::vector<bool> matched(n, false);
    std::vector<int> cover;
    
    for (int u = 0; u < n; ++u) {
        if (!matched[u]) {
            Edge* e = g.get_edges(u);
            while(e) {
                int v = e->to;
                if (!matched[v]) {
                    matched[u] = true;
                    matched[v] = true;
                    cover.push_back(u);
                    cover.push_back(v);
                    break;
                }
                e = e->next;
            }
        }
    }
    
    return cover;
}

}
