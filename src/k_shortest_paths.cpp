#include "graphlib/k_shortest_paths.h"
#include <queue>
#include <algorithm>
#include <set>
#include <map>
#include <limits>

namespace graphlib {

namespace {

struct PathNode {
    int v;
    long long dist;
    
    bool operator>(const PathNode& other) const {
        return dist > other.dist;
    }
};

// Dijkstra that respects blocked edges and blocked vertices
// blocked_edges: set of (u, v) pairs that cannot be used.
// blocked_vertices: set of vertices that cannot be visited (except potentially start).
// Note: Since Graph uses Edge*, and we might have parallel edges, blocking (u, v) blocks ALL edges from u to v?
// Yen's algorithm typically blocks specific edges in the path. 
// However, in a simple graph, (u, v) is unique.
// In a multigraph, we should block specific Edge pointers.
// But we can't easily identify Edge pointers from previous paths unless we store them.
// For this implementation, we will assume simple graph behavior for blocking: blocking (u, v) blocks all edges u->v.
// This is a common simplification unless specific edge IDs are tracked.
// IF we want to support multigraphs correctly, we need the paths to store Edge* or indices.
// The return type is vector<vector<int>> (vertex indices), so we lose edge identity.
// So blocking (u, v) is the only option.
std::pair<std::vector<int>, long long> dijkstra_blocked(
    const Graph& g, 
    int start, 
    int end, 
    const std::set<std::pair<int, int>>& blocked_edges,
    const std::set<int>& blocked_vertices
) {
    int n = g.vertex_count();
    std::vector<long long> dist(n, std::numeric_limits<long long>::max());
    std::vector<int> parent(n, -1);
    std::priority_queue<PathNode, std::vector<PathNode>, std::greater<PathNode>> pq;

    if (blocked_vertices.count(start)) {
                return {{}, -1};
            }

            dist[start] = 0;
            pq.push({start, 0});

            while (!pq.empty()) {
                PathNode current = pq.top();
                pq.pop();

                int u = current.v;
                long long d = current.dist;

                if (d > dist[u]) continue;
                if (u == end) break;

                for (Edge* e = g.get_edges(u); e != nullptr; e = e->next) {
                    int v = e->to;

                    if (blocked_vertices.count(v)) continue;
                    if (blocked_edges.count({u, v})) continue;

                    if (dist[u] + e->weight < dist[v]) {
                        dist[v] = dist[u] + e->weight;
                        parent[v] = u;
                        pq.push({v, dist[v]});
                    }
                }
            }

            if (dist[end] == std::numeric_limits<long long>::max()) {
                return {{}, -1};
            }

    std::vector<int> path;
    int curr = end;
    while (curr != -1) {
        path.push_back(curr);
        curr = parent[curr];
    }
    std::reverse(path.begin(), path.end());
    return {path, dist[end]};
}

} // namespace

GRAPHLIB_API std::vector<std::vector<int>> yen_k_shortest_paths(const Graph& g, int start, int end, int k) {
    if (k <= 0) return {};
    int n = g.vertex_count();
    if (start < 0 || start >= n || end < 0 || end >= n) return {};

    std::vector<std::vector<int>> A;
    // (cost, path)
    using PathEntry = std::pair<long long, std::vector<int>>;
    // Use a set to keep candidates unique and sorted
    std::set<PathEntry> B;

    // 1. Find shortest path
    auto p0 = dijkstra_blocked(g, start, end, {}, {});
    if (p0.second == -1) {
        return {};
    }

    A.push_back(p0.first);

    // 2. Iterate
    for (int k_idx = 1; k_idx < k; ++k_idx) {
        const auto& prev_path = A.back();
        
        // The spur node ranges from the first node to the second-to-last node of the previous path
        for (size_t i = 0; i < prev_path.size() - 1; ++i) {
            int spur_node = prev_path[i];
            
            // Root path is path from start to spur_node
            std::vector<int> root_path;
            for (size_t j = 0; j <= i; ++j) {
                root_path.push_back(prev_path[j]);
            }

            // Block edges and nodes
            std::set<std::pair<int, int>> blocked_edges;
            std::set<int> blocked_vertices;

            // Block edges used by existing paths A[0]...A[k-1] that share the same root_path
            for (const auto& path : A) {
                if (path.size() > i && std::equal(root_path.begin(), root_path.end(), path.begin())) {
                    blocked_edges.insert({path[i], path[i+1]});
                }
            }

            // Block nodes in root_path (except spur_node) to ensure loopless
            for (size_t j = 0; j < i; ++j) {
                blocked_vertices.insert(root_path[j]);
            }

            // Find spur path from spur_node to end
            auto spur_res = dijkstra_blocked(g, spur_node, end, blocked_edges, blocked_vertices);
            
            if (spur_res.second != -1) {
                const auto& spur_path = spur_res.first;
                long long spur_weight = spur_res.second;

                // Total path = root_path (excluding spur_node duplicate) + spur_path
                std::vector<int> total_path = root_path;
                total_path.pop_back(); // Remove spur_node to avoid duplication
                total_path.insert(total_path.end(), spur_path.begin(), spur_path.end());

                // Calculate total weight
                // Since we don't have direct access to edge weights in the path vector easily,
                // we should re-calculate or accumulate.
                // Or better, dijkstra_blocked returned cost from spur_node to end.
                // We need cost of root_path.
                long long root_weight = 0;
                for (size_t j = 0; j < i; ++j) {
                    int u = root_path[j];
                    int v = root_path[j+1];
                    // Find min weight edge u->v (assuming simple graph or best edge used)
                    // In strict Yen's with multigraphs, we'd need to know EXACTLY which edge was used.
                    // But here we just scan for the edge weight.
                    // This is an approximation if multiple edges exist with different weights?
                    // Actually, if we always pick the shortest, it's fine.
                    long long min_w = std::numeric_limits<long long>::max();
                    for (Edge* e = g.get_edges(u); e != nullptr; e = e->next) {
                        if (e->to == v) {
                            if (e->weight < min_w) min_w = e->weight;
                        }
                    }
                    root_weight += min_w;
                }

                long long total_weight = root_weight + spur_weight;
                B.insert({total_weight, total_path});
            }
        }

        if (B.empty()) break;

        // Extract best candidate
        auto best = *B.begin();
        B.erase(B.begin());
        A.push_back(best.second);
    }

    return A;
}

}
