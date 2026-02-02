#ifndef GRAPHLIB_CLIQUE_H
#define GRAPHLIB_CLIQUE_H

#include "export.h"
#include "graph_core.h"
#include <vector>

namespace graphlib {

/**
 * @brief Finds the Maximum Clique in the graph using the Bron-Kerbosch algorithm with pivoting.
 * 
 * A clique is a subset of vertices where every two distinct vertices are adjacent.
 * The Maximum Clique problem is NP-hard. This algorithm is efficient for dense graphs 
 * but still exponential in the worst case.
 * 
 * @param g The input undirected graph.
 * @return A vector of vertices forming the maximum clique.
 */
GRAPHLIB_API std::vector<int> maximum_clique(const Graph& g);

/**
 * @brief Finds all maximal cliques in the graph.
 * A clique is maximal if it cannot be extended by including one more adjacent vertex.
 * 
 * @param g The input undirected graph.
 * @return A vector of cliques (each clique is a vector of definition).
 */
GRAPHLIB_API std::vector<std::vector<int>> find_all_maximal_cliques(const Graph& g);

}

#endif
