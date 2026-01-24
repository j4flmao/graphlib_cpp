#ifndef GRAPHLIB_K_SHORTEST_PATHS_H
#define GRAPHLIB_K_SHORTEST_PATHS_H

#include "export.h"
#include "graphlib.h"
#include <vector>
#include <utility>

namespace graphlib {

/**
 * @brief Yen's Algorithm for K-Shortest Loopless Paths.
 * 
 * Finds the K shortest paths from start to end in a weighted directed graph.
 * The paths are guaranteed to be loopless if the graph has no negative cycles.
 * 
 * Time Complexity: O(K * N * (M + N log N))
 * 
 * @param g The graph.
 * @param start Start vertex.
 * @param end End vertex.
 * @param k Number of paths to find.
 * @return A vector of paths, where each path is a vector of vertex indices.
 *         The paths are sorted by total weight in ascending order.
 *         Returns fewer than K paths if fewer exist.
 */
GRAPHLIB_API std::vector<std::vector<int>> yen_k_shortest_paths(const Graph& g, int start, int end, int k);

}

#endif
