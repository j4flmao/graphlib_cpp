#ifndef GRAPHLIB_CYCLE_ALGO_H
#define GRAPHLIB_CYCLE_ALGO_H

#include "export.h"
#include "graph_core.h"
#include <vector>
#include <utility>

namespace graphlib {

/**
 * @brief Finds the Minimum Mean Cycle in a directed graph.
 * 
 * The mean weight of a cycle is the sum of edge weights divided by the number of edges.
 * Uses Karp's Algorithm O(VE).
 * 
 * @param g The directed graph.
 * @return The minimum mean cycle weight. Returns infinity if no cycle exists.
 */
GRAPHLIB_API double minimum_mean_cycle(const Graph& g);

}

#endif
