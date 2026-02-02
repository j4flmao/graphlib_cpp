#ifndef GRAPHLIB_NP_HARD_H
#define GRAPHLIB_NP_HARD_H

#include "export.h"
#include "graph_core.h"
#include <vector>

namespace graphlib {

/**
 * @brief Solves the Traveling Salesperson Problem (TSP) using Bitmask DP.
 * 
 * Time Complexity: O(n^2 * 2^n)
 * Space Complexity: O(n * 2^n)
 * 
 * @param g The graph (must be small, n <= 20).
 * @return The minimum cost to visit all nodes and return to start, or -1 if n > 20 or disconnected.
 */
GRAPHLIB_API long long tsp_bitmask(const Graph& g);

/**
 * @brief Finds the Maximum Clique size using Bron-Kerbosch algorithm.
 * 
 * @param g The graph (undirected).
 * @return The size of the maximum clique.
 */
GRAPHLIB_API int max_clique_bron_kerbosch(const Graph& g);

/**
 * @brief Finds the Maximum Weight Clique.
 * 
 * @param g The graph (undirected).
 * @param weights Weights of the vertices.
 * @return The weight of the maximum weight clique.
 */
GRAPHLIB_API long long max_weight_clique(const Graph& g, const std::vector<long long>& weights);


/**
 * @brief Colors the graph using greedy strategy with Welsh-Powell heuristic.
 * 
 * @param g The graph.
 * @param colors Output vector to store color of each vertex (0-indexed).
 * @return The number of colors used.
 */
GRAPHLIB_API int greedy_coloring(const Graph& g, std::vector<int>& colors);

/**
 * @brief Colors the graph using DSatur (Degree of Saturation) algorithm.
 * Usually produces better coloring than greedy Welsh-Powell.
 * 
 * @param g The graph.
 * @param colors Output vector to store color of each vertex (0-indexed).
 * @return The number of colors used.
 */
GRAPHLIB_API int dsatur_coloring(const Graph& g, std::vector<int>& colors);

/**
 * @brief Finds the Maximum Independent Set.
 * Uses Max Clique on the complement graph.
 * 
 * @param g The graph.
 * @return The size of the maximum independent set.
 */
GRAPHLIB_API int max_independent_set(const Graph& g);

/**
 * @brief Solves the Chinese Postman Problem (Route Inspection Problem).
 * Finds the minimum length closed walk that traverses every edge at least once.
 * 
 * Requires the graph to be connected.
 * 
 * @param g The graph (undirected, weighted).
 * @return The minimum total weight, or -1 if graph is not connected or has odd number of odd-degree vertices (should not happen in undirected).
 */
GRAPHLIB_API long long chinese_postman(const Graph& g);

/**
 * @brief Approximates TSP for Metric Graphs using MST + DFS.
 * Guarantees a solution within 2 * OPT.
 * 
 * @param g The graph (must be complete and satisfy triangle inequality).
 * @return The cost of the approximate tour and the tour path.
 */
GRAPHLIB_API std::pair<long long, std::vector<int>> tsp_metric_approx(const Graph& g);

/**
 * @brief Approximates TSP for Metric Graphs using Christofides Algorithm.
 * Guarantees a solution within 1.5 * OPT.
 * 
 * Steps:
 * 1. Find MST.
 * 2. Find vertices with odd degree in MST.
 * 3. Find Minimum Weight Perfect Matching on these vertices.
 * 4. Combine MST and Matching to form Eulerian Multigraph.
 * 5. Find Eulerian Circuit.
 * 6. Shortcut to form Hamiltonian Cycle.
 * 
 * @param g The graph (must be complete and satisfy triangle inequality).
 * @return The cost of the approximate tour and the tour path.
 */
GRAPHLIB_API std::pair<long long, std::vector<int>> tsp_christofides(const Graph& g);

/**
 * @brief Approximates Minimum Vertex Cover using Maximal Matching.
 * Guarantees a solution within 2 * OPT.
 * 
 * @param g The graph.
 * @return A vector of vertices in the vertex cover.
 */
GRAPHLIB_API std::vector<int> vertex_cover_approx(const Graph& g);

/**
 * @brief Approximates the Maximum Cut of the graph.
 * Uses a randomized approach (expected 0.5 approximation) or greedy.
 * This implementation uses a deterministic greedy strategy (0.5 approximation).
 * 
 * @param g The graph.
 * @return Pair of {total_cut_weight, partition}. 
 *         partition[u] is 0 or 1, denoting the set vertex u belongs to.
 */
GRAPHLIB_API std::pair<long long, std::vector<int>> max_cut_approx(const Graph& g);

/**
 * @brief Approximates the Minimum Feedback Vertex Set for undirected graphs.
 * A feedback vertex set is a set of vertices whose removal makes the graph acyclic.
 * 
 * Algorithm: Repeatedly find a cycle and remove a vertex from it.
 * This is not a constant factor approximation in general, but efficient heuristic.
 * For factor-2, specialized algorithms exist (Bafna et al.). we use a greedy cycle-breaking heuristic.
 * 
 * @param g The graph (undirected).
 * @return std::vector<int> List of vertices in the feedback set.
 */
GRAPHLIB_API std::vector<int> feedback_vertex_set_approx(const Graph& g);

/**
 * @brief Checks if the graph has a Hamiltonian Path.
 * A Hamiltonian Path visits every vertex exactly once.
 * 
 * Uses bitmask DP for small graphs (n <= 20), backtracking with pruning for larger graphs.
 * 
 * @param g The graph.
 * @return true if a Hamiltonian Path exists, false otherwise.
 */
GRAPHLIB_API bool has_hamiltonian_path(const Graph& g);

/**
 * @brief Finds a Hamiltonian Path in the graph if one exists.
 * A Hamiltonian Path visits every vertex exactly once.
 * 
 * Uses bitmask DP for small graphs (n <= 20), backtracking with pruning for larger graphs.
 * 
 * @param g The graph.
 * @return A vector of vertices representing the path, or empty vector if no path exists.
 */
GRAPHLIB_API std::vector<int> find_hamiltonian_path(const Graph& g);


}

#endif
