#ifndef GRAPHLIB_CONTEST_ALGORITHMS_H
#define GRAPHLIB_CONTEST_ALGORITHMS_H
#include "graph_core.h"
#include "export.h"
#include <vector>
namespace graphlib {
GRAPHLIB_API std::vector<long long> widest_paths(const Graph& g, int source);
GRAPHLIB_API std::vector<long long> count_shortest_paths(const Graph& g, int source, long long mod = 1000000007LL);
GRAPHLIB_API std::vector<long long> shortest_paths_at_most_k_edges(const Graph& g, int source, int max_edges);
GRAPHLIB_API std::vector<int> lexicographically_smallest_topological_order(const Graph& g, bool& has_cycle);
GRAPHLIB_API long long count_topological_orders(const Graph& g);
}
#endif
