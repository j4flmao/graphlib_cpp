#ifndef GRAPHLIB_CHORDAL_H
#define GRAPHLIB_CHORDAL_H

#include "export.h"
#include "graph_core.h"
#include <vector>

namespace graphlib {

/**
 * @brief Checks if a graph is chordal and optionally returns a Perfect Elimination Ordering (PEO).
 * A graph is chordal if every cycle of length > 3 has a chord.
 * 
 * @param g The graph to check.
 * @param peo Output parameter. If not null, stores the PEO (vertex mapping: order -> vertex index).
 *            The PEO is a permutation of 0..n-1 such that for any v = peo[i], 
 *            the neighbors of v that appear later in the ordering (peo[j] with j > i) form a clique.
 * @return true if the graph is chordal, false otherwise.
 */
GRAPHLIB_API bool is_chordal(const Graph& g, std::vector<int>* peo = nullptr);

/**
 * @brief Finds the Maximum Clique in a chordal graph using PEO.
 * Time Complexity: O(N + E)
 * 
 * @param g The chordal graph.
 * @return A vector containing the vertices of the maximum clique.
 *         Returns empty if the graph is not chordal (checked internally).
 */
GRAPHLIB_API std::vector<int> maximum_clique_chordal(const Graph& g);

/**
 * @brief Computes the Chromatic Number of a chordal graph.
 * Also effectively finds the Maximum Independent Set size (on chordal graphs, Max Clique Size = Chromatic Number? No, 
 * perfect graphs have omega(G) = chi(G). Chordal graphs are perfect. So yes).
 * But we might want the actual coloring.
 * 
 * @param g The chordal graph.
 * @param coloring Output parameter. If not null, stores the color of each vertex (0-based).
 * @return The chromatic number (minimum colors needed).
 */
GRAPHLIB_API int chromatic_number_chordal(const Graph& g, std::vector<int>* coloring = nullptr);

/**
 * @brief Finds the Maximum Independent Set in a chordal graph.
 * Time Complexity: O(N + E)
 * 
 * @param g The chordal graph.
 * @return A vector containing the vertices of the maximum independent set.
 */
GRAPHLIB_API std::vector<int> maximum_independent_set_chordal(const Graph& g);

}

#endif
