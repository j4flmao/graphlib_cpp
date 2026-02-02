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

std::pair<long long, std::vector<int>> tsp_christofides(const Graph& g) {
    int n = g.vertex_count();
    if (n == 0) return {0, {}};
    if (n == 1) return {0, {0}};

    // 1. Compute MST
    std::vector<MstEdge> edges;
    for (int u = 0; u < n; ++u) {
        Edge* e = g.get_edges(u);
        while (e) {
            if (u < e->to) {
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

    // 2. Build MST graph and identify odd degree vertices
    Graph multigraph(n, false);
    std::vector<int> degrees(n, 0);
    for (const auto& e : mst_edges) {
        multigraph.add_edge(e.u, e.v, e.weight);
        degrees[e.u]++;
        degrees[e.v]++;
    }

    std::vector<int> odds;
    for (int i = 0; i < n; ++i) {
        if (degrees[i] % 2 != 0) {
            odds.push_back(i);
        }
    }

    // 3. Minimum Weight Perfect Matching on odd vertices
    if (!odds.empty()) {
        int k = (int)odds.size();
        // Construct complete graph on odds
        // We need original distances. Since g is complete metric graph, direct edge is shortest.
        
        GeneralMatching matcher(k);
        long long max_w = 0;
        
        // Find max weight to transform to min weight matching
        for (int i = 0; i < k; ++i) {
            for (int j = i + 1; j < k; ++j) {
                int u = odds[i];
                int v = odds[j];
                // Find weight u-v in g
                long long w = std::numeric_limits<long long>::max();
                Edge* e = g.get_edges(u);
                while(e) {
                    if (e->to == v) {
                        w = std::min(w, e->weight);
                    }
                    e = e->next;
                }
                if (w != std::numeric_limits<long long>::max()) {
                    max_w = std::max(max_w, w);
                }
            }
        }
        
        long long M = max_w + 1;
        for (int i = 0; i < k; ++i) {
            for (int j = i + 1; j < k; ++j) {
                int u = odds[i];
                int v = odds[j];
                long long w = std::numeric_limits<long long>::max();
                Edge* e = g.get_edges(u);
                while(e) {
                    if (e->to == v) {
                        w = std::min(w, e->weight);
                    }
                    e = e->next;
                }
                if (w != std::numeric_limits<long long>::max()) {
                    matcher.add_edge(i, j, M - w);
                }
            }
        }
        
        matcher.maximum_weight_matching();
        std::vector<int> match = matcher.get_mate();
        
        // Add matching edges to multigraph
        for (int i = 0; i < k; ++i) {
            if (match[i] != -1 && match[i] > i) {
                int u = odds[i];
                int v = odds[match[i]];
                
                // Find weight
                long long w = std::numeric_limits<long long>::max();
                Edge* e = g.get_edges(u);
                while(e) {
                    if (e->to == v) {
                        w = std::min(w, e->weight);
                    }
                    e = e->next;
                }
                
                multigraph.add_edge(u, v, w);
            }
        }
    }
    
    // 4. Find Eulerian Circuit (Hierholzer's or DFS)
    // Multigraph is now Eulerian (all degrees even) and connected.
    // Use Hierholzer's algorithm adapted for recursion or stack
    
    // Since Graph uses linked list, we can remove edges? 
    // No, Graph doesn't support removal easily.
    // We can mark used edges.
    
    // Let's use a temporary adjacency structure that allows edge removal/marking
    struct TempEdge {
        int to;
        long long weight;
        int id; // to track reverse edge
        bool used;
        TempEdge* reverse_edge;
    };
    
    std::vector<std::vector<TempEdge*>> adj(n);
    // Use deque to prevent pointer invalidation on push_back, 
    // or simply reserve exact amount.
    // Total edges = (N-1) from MST + Matching edges.
    // Each undirected edge creates 2 TempEdges.
    // Safe upper bound: (N + N) * 2 = 4N.
    std::vector<TempEdge> all_edges_store;
    all_edges_store.reserve(n * 6); 

    
    int edge_id_counter = 0;
    for(int u=0; u<n; ++u) {
        Edge* e = multigraph.get_edges(u);
        while(e) {
            if(u < e->to) {
                // Add u->v and v->u
                all_edges_store.push_back({e->to, e->weight, edge_id_counter, false, nullptr});
                TempEdge* uv = &all_edges_store.back();
                
                all_edges_store.push_back({u, e->weight, edge_id_counter, false, nullptr});
                TempEdge* vu = &all_edges_store.back();
                
                uv->reverse_edge = vu;
                vu->reverse_edge = uv;
                
                adj[u].push_back(uv);
                adj[e->to].push_back(vu);
                
                edge_id_counter++;
            }
            e = e->next;
        }
    }
    
    std::vector<int> circuit;
    std::stack<int> curr_path;
    curr_path.push(0);
    
    while(!curr_path.empty()) {
        int u = curr_path.top();
        
        bool found = false;
        // Find unused edge
        while(!adj[u].empty()) {
            TempEdge* e = adj[u].back();
            if(e->used) {
                adj[u].pop_back();
                continue;
            }
            
            e->used = true;
            e->reverse_edge->used = true;
            curr_path.push(e->to);
            found = true;
            break;
        }
        
        if(!found) {
            circuit.push_back(u);
            curr_path.pop();
        }
    }
    
    // 5. Shortcut to form Hamiltonian Cycle
    std::vector<int> tsp_path;
    std::vector<bool> visited(n, false);
    long long total_cost = 0;
    
    // Circuit is in reverse order, but for undirected TSP it doesn't matter.
    // Also, 0 is at end and start.
    
    for (auto it = circuit.rbegin(); it != circuit.rend(); ++it) {
        int u = *it;
        if (!visited[u]) {
            visited[u] = true;
            tsp_path.push_back(u);
        }
    }
    
    // Calculate cost
    if (tsp_path.size() != n) return {-1, {}};
    
    for (size_t i = 0; i < n; ++i) {
        int u = tsp_path[i];
        int v = tsp_path[(i + 1) % n];
        
        long long w = std::numeric_limits<long long>::max();
        Edge* e = g.get_edges(u);
        while(e) {
            if (e->to == v) {
                w = std::min(w, e->weight);
            }
            e = e->next;
        }
        if (w == std::numeric_limits<long long>::max()) return {-1, {}};
        total_cost += w;
    }
    
    return {total_cost, tsp_path};
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

// -----------------------------------------------------------------------------
// Max Cut
// -----------------------------------------------------------------------------

std::pair<long long, std::vector<int>> max_cut_approx(const Graph& g) {
    int n = g.vertex_count();
    std::vector<int> partition(n, 0); // Start all in set 0
    std::vector<long long> gain(n, 0); // Gain if moved to other set
    
    // Greedy strategy: Loop through vertices, if moving improves cut, move it.
    // Repeat until stable. This is a local search 0.5-approx.
    
    bool improved = true;
    while(improved) {
        improved = false;
        
        for (int u = 0; u < n; ++u) {
            long long w_internal = 0; // edges to same set
            long long w_external = 0; // edges to other set
            
            Edge* e = g.get_edges(u);
            while(e) {
                if (partition[e->to] == partition[u]) {
                    w_internal += e->weight;
                } else {
                    w_external += e->weight;
                }
                e = e->next;
            }
            
            // If internal > external, we gain by flipping
            if (w_internal > w_external) {
                partition[u] = 1 - partition[u];
                improved = true;
            }
        }
    }
    
    // Calculate final cut
    long long total_cut = 0;
    for (int u = 0; u < n; ++u) {
        Edge* e = g.get_edges(u);
        while(e) {
            if (u < e->to) {
                if (partition[u] != partition[e->to]) {
                    total_cut += e->weight;
                }
            }
            e = e->next;
        }
    }
    
    return {total_cut, partition};
}

// -----------------------------------------------------------------------------
// Feedback Vertex Set
// -----------------------------------------------------------------------------

std::vector<int> feedback_vertex_set_approx(const Graph& g) {
    int n = g.vertex_count();
    std::vector<int> fvs;
    std::vector<bool> removed(n, false);
    
    // Greedy heuristic: Repeatedly find a cycle and remove max-degree node in it.
    
    std::vector<std::vector<int>> adj(n);
    for(int u=0; u<n; ++u) {
        Edge* e = g.get_edges(u);
        while(e) {
            adj[u].push_back(e->to);
            e = e->next;
        }
    }
    
    while(true) {
        std::vector<int> parent(n, -1);
        std::vector<int> color(n, 0); // 0: unvisited, 1: visiting, 2: visited
        std::vector<int> cycle_nodes;
        
        bool cycle_found = false;
        
        std::function<void(int, int)> dfs = [&](int u, int p) {
            if (cycle_found) return;
            color[u] = 1;
            parent[u] = p;
            
            for (int v : adj[u]) {
                if (removed[v] || v == p) continue;
                
                if (color[v] == 1) {
                    // Cycle detected
                    int curr = u;
                    while (curr != v) {
                        cycle_nodes.push_back(curr);
                        curr = parent[curr];
                    }
                    cycle_nodes.push_back(v);
                    cycle_found = true;
                    return;
                }
                
                if (color[v] == 0) {
                    dfs(v, u);
                    if (cycle_found) return;
                }
            }
            color[u] = 2;
        };
        
        for(int i=0; i<n; ++i) {
            if (!removed[i] && color[i] == 0) {
                dfs(i, -1);
                if (cycle_found) break;
            }
        }
        
        if (!cycle_found) break;
        
        int best_node = -1;
        int max_deg = -1;
        
        for (int u : cycle_nodes) {
            int d = 0;
            for(int v : adj[u]) {
                if(!removed[v]) d++;
            }
            if (d > max_deg) {
                max_deg = d;
                best_node = u;
            }
        }
        
        removed[best_node] = true;
        fvs.push_back(best_node);
    }
    
    return fvs;
}

}
