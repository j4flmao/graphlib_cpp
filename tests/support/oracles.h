#ifndef GRAPHLIB_TEST_ORACLES_H
#define GRAPHLIB_TEST_ORACLES_H

#include "graphlib/directed_mst.h"
#include "graphlib/max_flow.h"
#include <limits>
#include <stdexcept>
#include <vector>

namespace graphlib::test {

inline long long brute_force_min_cut(const MaxFlow& /*network*/, int /*source*/, int /*sink*/) {
    // MaxFlow intentionally keeps residual edges private. This overload is a placeholder
    // for serialized test networks; use brute_force_min_cut_edges below in new tests.
    throw std::logic_error("Use brute_force_min_cut_edges with an explicit edge list");
}

struct CapacityEdge { int from; int to; long long capacity; };

inline long long brute_force_min_cut_edges(int n, int source, int sink,
                                           const std::vector<CapacityEdge>& edges) {
    if (n < 0 || source < 0 || source >= n || sink < 0 || sink >= n || source == sink)
        throw std::invalid_argument("Invalid min-cut oracle input");
    if (n > 20) throw std::invalid_argument("Min-cut oracle is limited to 20 vertices");
    long long answer = std::numeric_limits<long long>::max();
    const unsigned long long limit = 1ULL << n;
    for (unsigned long long side = 0; side < limit; ++side) {
        if (!(side & (1ULL << source)) || (side & (1ULL << sink))) continue;
        long long cut = 0;
        for (const auto& edge : edges)
            if ((side & (1ULL << edge.from)) && !(side & (1ULL << edge.to))) cut += edge.capacity;
        answer = std::min(answer, cut);
    }
    return answer;
}

inline bool is_valid_arborescence(int n, int root,
                                  const std::vector<DirectedEdge>& edges,
                                  const std::vector<int>& selected_ids) {
    if (n < 0 || root < 0 || root >= n || static_cast<int>(selected_ids.size()) != n - 1) return false;
    std::vector<int> parent(n, -1);
    for (int id : selected_ids) {
        bool found = false;
        for (const auto& edge : edges) if (edge.id == id) {
            if (edge.v == root || parent[edge.v] != -1) return false;
            parent[edge.v] = edge.u; found = true; break;
        }
        if (!found) return false;
    }
    for (int v = 0; v < n; ++v) if (v != root) {
        if (parent[v] == -1) return false;
        std::vector<char> seen(n, 0);
        int u = v;
        while (u != root) {
            if (u < 0 || u >= n || seen[u]) return false;
            seen[u] = 1; u = parent[u];
        }
    }
    return true;
}

} // namespace graphlib::test

#endif
