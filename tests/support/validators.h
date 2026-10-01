#ifndef GRAPHLIB_TEST_VALIDATORS_H
#define GRAPHLIB_TEST_VALIDATORS_H

#include "graphlib/graph_core.h"
#include <vector>

namespace graphlib::test {

inline bool is_valid_topological_order(const Graph& graph, const std::vector<int>& order) {
    if (static_cast<int>(order.size()) != graph.vertex_count()) return false;
    std::vector<int> position(order.size(), -1);
    for (int i = 0; i < static_cast<int>(order.size()); ++i) {
        if (order[i] < 0 || order[i] >= graph.vertex_count() || position[order[i]] != -1) return false;
        position[order[i]] = i;
    }
    for (int u = 0; u < graph.vertex_count(); ++u)
        for (Edge* edge = graph.get_edges(u); edge; edge = edge->next)
            if (position[u] >= position[edge->to]) return false;
    return true;
}

inline bool is_independent_set(const Graph& graph, const std::vector<int>& vertices) {
    std::vector<char> selected(graph.vertex_count(), 0);
    for (int v : vertices) {
        if (v < 0 || v >= graph.vertex_count() || selected[v]) return false;
        selected[v] = 1;
    }
    for (int u : vertices)
        for (Edge* edge = graph.get_edges(u); edge; edge = edge->next)
            if (selected[edge->to]) return false;
    return true;
}

} // namespace graphlib::test

#endif
