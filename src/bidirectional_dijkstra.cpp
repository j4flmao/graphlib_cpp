#include "graphlib/bidirectional_dijkstra.h"
#include <queue>
#include <limits>
#include <algorithm>
#include <map>

namespace graphlib {

namespace {
    const long long INF = std::numeric_limits<long long>::max();

    struct State {
        long long dist;
        int u;
        bool operator>(const State& other) const {
            return dist > other.dist;
        }
    };
}

GRAPHLIB_API std::pair<long long, std::vector<int>> bidirectional_dijkstra(const Graph& g, int source, int target) {
    if (source == target) return {0, {source}};

    int n = g.vertex_count();
    
    // Forward search
    std::vector<long long> dist_f(n, INF);
    std::vector<int> parent_f(n, -1);
    std::vector<bool> visited_f(n, false);
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq_f;

    // Backward search
    // We need reverse graph access.
    // If graph is undirected, it's easy.
    // If directed, we need incoming edges. The Graph class is adjacency list of OUTGOING edges.
    // We must build reverse graph or iterate all edges (slow).
    // Let's assume we build reverse graph on the fly or pre-build it.
    // For "Extreme" performance, we should build it once.
    
    std::vector<std::vector<std::pair<int, long long>>> rev_adj(n);
    bool directed = g.is_directed();
    
    if (directed) {
        for (int u = 0; u < n; ++u) {
            Edge* e = g.get_edges(u);
            while(e) {
                rev_adj[e->to].push_back({u, e->weight});
                e = e->next;
            }
        }
    }
    // If undirected, we can use g directly for both directions.

    std::vector<long long> dist_b(n, INF);
    std::vector<int> parent_b(n, -1);
    std::vector<bool> visited_b(n, false);
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq_b;

    // Init
    dist_f[source] = 0;
    pq_f.push({0, source});

    dist_b[target] = 0;
    pq_b.push({0, target});

    long long best_path_len = INF;
    int meeting_node = -1;

    while (!pq_f.empty() && !pq_b.empty()) {
        // We can alternate or pick smaller queue.
        // Standard is alternate.
        
        // Forward Step
        if (!pq_f.empty()) {
            State top = pq_f.top(); pq_f.pop();
            int u = top.u;
            long long d = top.dist;

            if (d <= dist_f[u]) {
                visited_f[u] = true;
                
                // Check connection
                if (visited_b[u]) {
                    if (dist_f[u] + dist_b[u] < best_path_len) {
                        best_path_len = dist_f[u] + dist_b[u];
                        meeting_node = u;
                    }
                }

                // Expand
                Edge* e = g.get_edges(u);
                while(e) {
                    int v = e->to;
                    long long w = e->weight;
                    if (dist_f[u] + w < dist_f[v]) {
                        dist_f[v] = dist_f[u] + w;
                        parent_f[v] = u;
                        pq_f.push({dist_f[v], v});
                    }
                    e = e->next;
                }
            }
        }

        // Check termination condition for bidirectional
        // If top_f + top_b >= best_path, we can stop?
        // Correct condition: if min(pq_f) + min(pq_b) >= best_path_len, stop.
        
        long long min_f = pq_f.empty() ? INF : pq_f.top().dist;
        long long min_b = pq_b.empty() ? INF : pq_b.top().dist;
        if (min_f != INF && min_b != INF && min_f + min_b >= best_path_len) break;

        // Backward Step
        if (!pq_b.empty()) {
            State top = pq_b.top(); pq_b.pop();
            int u = top.u; // This is 'v' in forward sense, traversing backwards
            long long d = top.dist;

            if (d <= dist_b[u]) {
                visited_b[u] = true;

                // Check connection
                if (visited_f[u]) {
                    if (dist_f[u] + dist_b[u] < best_path_len) {
                        best_path_len = dist_f[u] + dist_b[u];
                        meeting_node = u;
                    }
                }

                // Expand
                if (directed) {
                    for (auto& edge : rev_adj[u]) {
                        int v = edge.first; // parent in forward graph
                        long long w = edge.second;
                        if (dist_b[u] + w < dist_b[v]) {
                            dist_b[v] = dist_b[u] + w;
                            parent_b[v] = u; // parent_b points FROM target TO source
                            pq_b.push({dist_b[v], v});
                        }
                    }
                } else {
                    Edge* e = g.get_edges(u);
                    while(e) {
                        int v = e->to;
                        long long w = e->weight;
                        if (dist_b[u] + w < dist_b[v]) {
                            dist_b[v] = dist_b[u] + w;
                            parent_b[v] = u;
                            pq_b.push({dist_b[v], v});
                        }
                        e = e->next;
                    }
                }
            }
        }
        
        // Check termination again
        min_f = pq_f.empty() ? INF : pq_f.top().dist;
        min_b = pq_b.empty() ? INF : pq_b.top().dist;
        if (min_f != INF && min_b != INF && min_f + min_b >= best_path_len) break;
    }

    if (best_path_len == INF) return {-1, {}};

    // Reconstruct path
    // Path: source -> ... -> meeting_node -> ... -> target
    std::vector<int> path;
    
    // Forward part: source to meeting_node
    int curr = meeting_node;
    while (curr != -1) {
        path.push_back(curr);
        curr = parent_f[curr];
    }
    std::reverse(path.begin(), path.end());

    // Backward part: meeting_node to target
    // parent_b stores path from target to source (reverse)
    // parent_b[v] = u means u -> v in backward search, which is v -> u in graph?
    // Wait, in backward search: expand u, find neighbor v (incoming to u).
    // dist_b[v] = dist_b[u] + w. parent_b[v] = u.
    // So parent_b[v] is the node closer to target.
    // So path is v -> parent_b[v] -> parent_b[parent_b[v]] ... -> target.
    
    curr = parent_b[meeting_node];
    while (curr != -1) {
        path.push_back(curr);
        curr = parent_b[curr];
    }

    return {best_path_len, path};
}

}
