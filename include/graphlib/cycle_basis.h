#ifndef GRAPHLIB_CYCLE_BASIS_H
#define GRAPHLIB_CYCLE_BASIS_H

#include "export.h"
#include "graph_core.h"
#include <vector>

namespace graphlib {

/**
 * @brief Represents a cycle as a list of edges (pairs of vertices).
 */
struct Cycle {
    std::vector<std::pair<int, int>> edges;
    long long weight;  // Sum of edge weights in the cycle
};

/**
 * @brief Computes the Minimum Cycle Basis of an undirected graph.
 * 
 * A cycle basis is a set of cycles that can generate all cycles through XOR operations.
 * The minimum cycle basis minimizes the total weight of all cycles.
 * 
 * Uses Horton's Algorithm:
 * 1. Generate candidate cycles from shortest paths
 * 2. Sort by weight
 * 3. Greedily select linearly independent cycles
 * 
 * Time Complexity: O(m^3 * n) where m = edges, n = vertices
 * 
 * @param g The undirected graph.
 * @return std::vector<Cycle> The minimum cycle basis.
 */
GRAPHLIB_API std::vector<Cycle> minimum_cycle_basis(const Graph& g);

/**
 * @brief Computes the fundamental cycles from a spanning tree.
 * 
 * For each non-tree edge, the fundamental cycle is the unique cycle
 * formed by adding that edge to the tree.
 * 
 * @param g The undirected graph.
 * @return std::vector<Cycle> The fundamental cycles.
 */
GRAPHLIB_API std::vector<Cycle> fundamental_cycles(const Graph& g);

/**
 * @brief Counts the number of simple cycles up to a given length.
 * 
 * @param g The graph.
 * @param max_length Maximum cycle length to count.
 * @return long long Number of simple cycles.
 */
GRAPHLIB_API long long count_cycles(const Graph& g, int max_length);

/**
 * @brief Finds all simple cycles in the graph (for small graphs only).
 * 
 * Uses Johnson's algorithm for finding all elementary cycles.
 * Warning: Can be exponential in output size.
 * 
 * @param g The graph (directed or undirected).
 * @param max_cycles Maximum number of cycles to return (0 = unlimited).
 * @return std::vector<std::vector<int>> Each cycle as a list of vertices.
 */
GRAPHLIB_API std::vector<std::vector<int>> find_all_cycles(const Graph& g, int max_cycles = 1000);

/**
 * @brief Finds the shortest cycle in the graph (girth).
 * 
 * @param g The graph.
 * @return int The length of the shortest cycle, or -1 if acyclic.
 */
GRAPHLIB_API int girth(const Graph& g);

/**
 * @brief Finds the longest simple cycle in the graph.
 * 
 * NP-hard problem, uses heuristics for larger graphs.
 * For small graphs (n <= 20), uses exact algorithm.
 * 
 * @param g The graph.
 * @return std::vector<int> The vertices of the longest cycle, or empty if acyclic.
 */
GRAPHLIB_API std::vector<int> longest_cycle(const Graph& g);

}

#endif
