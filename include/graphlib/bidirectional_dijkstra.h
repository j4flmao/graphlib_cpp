#ifndef GRAPHLIB_BIDIRECTIONAL_DIJKSTRA_H
#define GRAPHLIB_BIDIRECTIONAL_DIJKSTRA_H

#include "export.h"
#include "graph_core.h"
#include <vector>
#include <utility>

namespace graphlib {

/**
 * @brief Computes the shortest path using Bidirectional Dijkstra.
 * Runs two simultaneous Dijkstra searches: one forward from source, one backward from target.
 * Can be significantly faster than standard Dijkstra for point-to-point queries.
 * 
 * @param g The graph.
 * @param source Source vertex.
 * @param target Target vertex.
 * @return Pair of {distance, path}. Path is empty if unreachable.
 */
GRAPHLIB_API std::pair<long long, std::vector<int>> bidirectional_dijkstra(const Graph& g, int source, int target);

}

#endif
