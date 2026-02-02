#ifndef GRAPHLIB_COMMUNITY_H
#define GRAPHLIB_COMMUNITY_H

#include "export.h"
#include "graph_core.h"
#include <vector>
#include <map>
#include <utility>

namespace graphlib {

/**
 * @brief Detects communities using the Louvain Method.
 * Optimizes Modularity in a hierarchical manner.
 * 
 * @param g The graph.
 * @return std::vector<int> A vector where result[u] is the community ID of vertex u.
 */
GRAPHLIB_API std::vector<int> louvain_communities(const Graph& g);

/**
 * @brief Computes the K-Truss Decomposition of the graph.
 * A k-truss is a subgraph where every edge participates in at least k-2 triangles.
 * 
 * @param g The graph (undirected).
 * @return std::map<std::pair<int, int>, int> Map from edge (u,v) [where u < v] to its trussness value.
 *         The trussness is the max k such that the edge belongs to a k-truss.
 */
GRAPHLIB_API std::map<std::pair<int, int>, int> k_truss_decomposition(const Graph& g);

}

#endif
