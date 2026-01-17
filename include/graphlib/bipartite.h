#ifndef GRAPHLIB_BIPARTITE_H
#define GRAPHLIB_BIPARTITE_H

#include "export.h"
#include "graph_core.h"

namespace graphlib {

class GRAPHLIB_API BipartiteGraph : public Graph {
private:
    int n_left_;
    int n_right_;

    bool bfs_hopcroft_karp(int* dist, int* pair_u, int* pair_v);
    bool dfs_hopcroft_karp(int u, int* dist, int* pair_u, int* pair_v);

public:
    BipartiteGraph(int n_left, int n_right);

    bool is_bipartite();
    int maximum_matching();
    long long hungarian_min_cost(long long inf);
};

}

#endif
