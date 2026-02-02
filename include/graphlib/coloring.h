#ifndef GRAPHLIB_COLORING_H
#define GRAPHLIB_COLORING_H

#include "export.h"
#include "graph_core.h"
#include <vector>

namespace graphlib {

/**
 * @brief Greedy Graph Coloring using Welsh-Powell or DSATUR heuristic.
 * 
 * Attempts to color the graph with minimal number of colors such that no two adjacent vertices share the same color.
 * 
 * @param g The undirected graph.
 * @return A vector 'colors' where colors[i] is the color ID (0-indexed) of vertex i.
 */
GRAPHLIB_API std::vector<int> greedy_coloring(const Graph& g);

/**
 * @brief Computes the chromatic number (minimum colors needed) exactly.
 * WARNING: NP-Hard. Uses exact algorithm (e.g. backtracking with pruning).
 * Suitable only for small graphs (N < 20-30 usually).
 */
GRAPHLIB_API int chromatic_number(const Graph& g);

}

#endif
