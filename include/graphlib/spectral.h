#ifndef GRAPHLIB_SPECTRAL_H
#define GRAPHLIB_SPECTRAL_H

#include "export.h"
#include "graph_core.h"
#include <vector>
#include <utility>

namespace graphlib {

/**
 * @brief Computes the Fiedler Vector of the graph.
 * The Fiedler vector is the eigenvector corresponding to the second smallest eigenvalue 
 * of the Laplacian matrix L = D - A.
 * It is used for spectral graph partitioning.
 * 
 * @param g The graph (undirected).
 * @param iterations Number of iterations for power method.
 * @param tolerance Convergence tolerance.
 * @return std::vector<double> The Fiedler vector.
 */
GRAPHLIB_API std::vector<double> fiedler_vector(const Graph& g, int iterations = 1000, double tolerance = 1e-9);

/**
 * @brief Partitions the graph into two sets using Spectral Bisection.
 * Uses the sign of the Fiedler vector (or median cut) to split vertices.
 * 
 * @param g The graph.
 * @return Pair of vectors representing the two partitions.
 */
GRAPHLIB_API std::pair<std::vector<int>, std::vector<int>> spectral_bisection(const Graph& g);

}

#endif
