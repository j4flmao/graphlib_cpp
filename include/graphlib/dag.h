#ifndef GRAPHLIB_DAG_H
#define GRAPHLIB_DAG_H

#include "export.h"
#include "graph_core.h"
#include <vector>

namespace graphlib {

class GRAPHLIB_API DAG : public Graph {
public:
    explicit DAG(int n);

    std::vector<int> topological_sort_kahn(bool& has_cycle);
    std::vector<int> topological_sort_dfs(bool& has_cycle);
    std::vector<long long> longest_path(int source, long long minus_inf);
    std::vector<long long> shortest_path(int source, long long inf);
    long long count_paths(int source, int target);
    bool would_create_cycle(int from, int to) const;
    bool add_edge_checked(int from, int to);
};

GRAPHLIB_API DAG build_scc_condensation_dag(const Graph& g, const std::vector<int>& component, int component_count);

}

#endif

