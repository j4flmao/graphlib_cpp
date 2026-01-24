#include "graphlib/chordal.h"
#include <vector>
#include <set>
#include <algorithm>
#include <map>

namespace graphlib {

// Maximum Cardinality Search (MCS)
// Returns vertices in the order they were visited (reverse PEO).
static std::vector<int> mcs(const Graph& g) {
    int n = g.vertex_count();
    std::vector<int> weight(n, 0);
    std::vector<bool> visited(n, false);
    std::vector<int> order;
    order.reserve(n);

    // Buckets for O(N+E) implementation
    // max weight is n-1
    std::vector<std::vector<int>> buckets(n);
    for (int i = 0; i < n; ++i) {
        buckets[0].push_back(i);
    }

    int max_weight = 0;

    for (int i = 0; i < n; ++i) {
        // Find a vertex with max weight
        int u = -1;
        while (max_weight >= 0) {
            while (!buckets[max_weight].empty()) {
                int candidate = buckets[max_weight].back();
                buckets[max_weight].pop_back();
                if (!visited[candidate]) {
                    u = candidate;
                    goto found;
                }
            }
            max_weight--;
        }
    found:
        if (u == -1) break; // Should not happen if graph is consistent

        visited[u] = true;
        order.push_back(u);

        Edge* e = g.get_edges(u);
        while (e) {
            int v = e->to;
            if (!visited[v]) {
                weight[v]++;
                if (weight[v] > max_weight) {
                    max_weight = weight[v];
                }
                buckets[weight[v]].push_back(v);
            }
            e = e->next;
        }
    }

    return order;
}

GRAPHLIB_API bool is_chordal(const Graph& g, std::vector<int>* peo_out) {
    int n = g.vertex_count();
    if (n == 0) {
        if (peo_out) peo_out->clear();
        return true;
    }

    // 1. Run MCS
    std::vector<int> order = mcs(g);
    
    // MCS produces reverse PEO.
    // PEO: v1, v2, ..., vn such that neighbors of vi in {vi+1...vn} form a clique.
    // order is vn, vn-1, ..., v1.
    std::reverse(order.begin(), order.end());

    // 2. Map vertex to position in PEO
    std::vector<int> pos(n);
    for (int i = 0; i < n; ++i) {
        pos[order[i]] = i;
    }

    // 3. Check PEO property
    // For each u, let N'(u) be neighbors v with pos[v] > pos[u].
    // Let w be the node in N'(u) with smallest pos[w].
    // We need to check if (N'(u) \ {w}) is a subset of N'(w) \cup {w} (actually neighbors of w).
    // Specifically, every x in N'(u)\{w} must be connected to w.
    
    for (int i = 0; i < n; ++i) {
        int u = order[i];
        
        // Find neighbors with higher position
        int parent = -1; // The neighbor with smallest index > i
        std::vector<int> high_neighbors;
        
        Edge* e = g.get_edges(u);
        while (e) {
            int v = e->to;
            if (pos[v] > i) {
                if (parent == -1 || pos[v] < pos[parent]) {
                    parent = v;
                }
                high_neighbors.push_back(v);
            }
            e = e->next;
        }

        if (parent != -1) {
            // Check if all other high_neighbors are connected to parent
            for (int neighbor : high_neighbors) {
                if (neighbor == parent) continue;
                // Check edge (neighbor, parent)
                bool connected = false;
                Edge* e2 = g.get_edges(neighbor);
                while (e2) {
                    if (e2->to == parent) {
                        connected = true;
                        break;
                    }
                    e2 = e2->next;
                }
                if (!connected) {
                    return false;
                }
            }
        }
    }

    if (peo_out) {
        *peo_out = std::move(order);
    }
    return true;
}

    GRAPHLIB_API std::vector<int> maximum_clique_chordal(const Graph& g) {
        std::vector<int> peo;
        if (!is_chordal(g, &peo)) {
            return {};
        }
    
        int n = g.vertex_count();
        std::vector<int> pos(n);
        for (int i = 0; i < n; ++i) pos[peo[i]] = i;
    
        int max_size = 0;
        std::vector<int> best_clique;
    
        // Iterate through PEO. Candidate clique is {u} U N'(u).
        for (int i = 0; i < n; ++i) {
            int u = peo[i];
            std::vector<int> clique;
            clique.push_back(u);
    
            Edge* e = g.get_edges(u);
            while (e) {
                int v = e->to;
                if (pos[v] > i) {
                    clique.push_back(v);
                }
                e = e->next;
            }
            
            if (static_cast<int>(clique.size()) > max_size) {
                max_size = static_cast<int>(clique.size());
                best_clique = std::move(clique);
            }
        }
    
        return best_clique;
    }

GRAPHLIB_API int chromatic_number_chordal(const Graph& g, std::vector<int>* coloring) {
        std::vector<int> peo;
        if (!is_chordal(g, &peo)) {
            return -1;
        }

        int n = g.vertex_count();
        // Greedy coloring on PEO (reverse PEO order works best? No, PEO order v1..vn is fine)
        // Actually, for optimal coloring, we should color in reverse PEO order (vn...v1).
        // Let's verify.
        // Perfect elimination ordering v1, ..., vn.
        // Coloring greedily in reverse order (vn, ..., v1) is optimal.
        // Because when we color vi, its already-colored neighbors are in {vi+1, ..., vn}, which form a clique.
        // So they all have distinct colors. We just pick the smallest available.
        // The number of colors used will be exactly size of max clique (omega) because graph is perfect.
        
        std::vector<int> colors(n, -1);
        int max_color = 0;
    
        for (int i = n - 1; i >= 0; --i) {
            int u = peo[i];
            std::set<int> neighbor_colors;
            Edge* e = g.get_edges(u);
            while (e) {
                int v = e->to;
                if (colors[v] != -1) {
                    neighbor_colors.insert(colors[v]);
                }
                e = e->next;
            }
    
            int c = 0;
            while (neighbor_colors.count(c)) {
                c++;
            }
            colors[u] = c;
            if (c + 1 > max_color) max_color = c + 1;
        }
    
        if (coloring) {
            *coloring = std::move(colors);
        }
        return max_color;
    }

GRAPHLIB_API std::vector<int> maximum_independent_set_chordal(const Graph& g) {
    std::vector<int> peo;
    if (!is_chordal(g, &peo)) {
        return {};
    }

    int n = g.vertex_count();
    std::vector<int> independent_set;
    std::vector<bool> blocked(n, false);

    // Iterate PEO v1...vn.
    // If v is not blocked, add to IS and block all neighbors.
    // Since v's earlier neighbors (in PEO) are not relevant (we process 1..n),
    // we only care about neighbors.
    // Wait, is this correct?
    // Gavril's algorithm:
    // Process PEO v1...vn. If v_i not adjacent to any vertex already in I, add to I.
    // Since we process in order, we check neighbors.
    // Actually, simpler:
    // Iterate v1 to vn. If v1 is available, take it. Then mark neighbors as unavailable.
    // This works because v1 is simplicial in the subgraph induced by {v1...vn}.
    
    for (int i = 0; i < n; ++i) {
        int u = peo[i];
        if (!blocked[u]) {
            independent_set.push_back(u);
            blocked[u] = true; // Block self
            Edge* e = g.get_edges(u);
            while (e) {
                blocked[e->to] = true;
                e = e->next;
            }
        }
    }

    return independent_set;
}

}
