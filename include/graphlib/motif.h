#ifndef GRAPHLIB_MOTIF_H
#define GRAPHLIB_MOTIF_H

#include "export.h"
#include "graph_core.h"
#include <vector>
#include <map>

namespace graphlib {

/**
 * @brief Triangle counting result.
 */
struct TriangleResult {
    long long total_triangles;          // Total number of triangles
    std::vector<long long> per_vertex;  // Triangles incident to each vertex
};

/**
 * @brief Counts all triangles in an undirected graph.
 * 
 * Uses the node-iterator algorithm with degree ordering.
 * Time Complexity: O(m * sqrt(m)) where m = edges
 * 
 * @param g The undirected graph.
 * @return TriangleResult Triangle counts.
 */
GRAPHLIB_API TriangleResult count_triangles(const Graph& g);

/**
 * @brief Lists all triangles (for small graphs).
 * 
 * @param g The graph.
 * @param max_triangles Maximum number to return (0 = unlimited).
 * @return std::vector<std::tuple<int,int,int>> List of triangles as (u,v,w) tuples.
 */
GRAPHLIB_API std::vector<std::tuple<int, int, int>> list_triangles(const Graph& g, int max_triangles = 0);

/**
 * @brief Counts 4-cliques (K4) in the graph.
 * 
 * @param g The graph.
 * @return long long Number of 4-cliques.
 */
GRAPHLIB_API long long count_4cliques(const Graph& g);

/**
 * @brief Counts k-cliques in the graph.
 * 
 * Uses enumeration for small k, approximation for larger k.
 * 
 * @param g The graph.
 * @param k Clique size.
 * @return long long Number of k-cliques.
 */
GRAPHLIB_API long long count_k_cliques(const Graph& g, int k);

/**
 * @brief Motif types for directed graphs (3-node motifs).
 */
enum class MotifType {
    M1,   // A -> B -> C
    M2,   // A -> B, A -> C
    M3,   // A -> B, C -> B
    M4,   // A -> B -> C, A -> C
    M5,   // A <-> B -> C
    M6,   // A -> B, B -> C, C -> A (cycle)
    M7,   // A <-> B <-> C
    M8,   // A <-> B -> C, A -> C
    M9,   // A <-> B, B <-> C
    M10,  // A <-> B, B <-> C, A -> C
    M11,  // A <-> B, B <-> C, A <-> C
    M12,  // A <-> B <-> C <-> A (triangle)
    M13   // A <-> B <-> C <-> A (complete)
};

/**
 * @brief Counts 3-node network motifs in a directed graph.
 * 
 * Network motifs are recurring significant patterns in networks.
 * 
 * @param g The directed graph.
 * @return std::map<MotifType, long long> Counts for each motif type.
 */
GRAPHLIB_API std::map<MotifType, long long> count_3node_motifs(const Graph& g);

/**
 * @brief Counts graphlets (induced subgraphs) of size k.
 * 
 * @param g The graph.
 * @param k Graphlet size (2, 3, 4, or 5).
 * @return std::vector<long long> Counts for each graphlet type.
 */
GRAPHLIB_API std::vector<long long> count_graphlets(const Graph& g, int k);

/**
 * @brief Computes Graphlet Degree Distribution (GDD).
 * 
 * For each vertex, counts how many times it participates in each graphlet orbit.
 * Useful for network comparison.
 * 
 * @param g The graph.
 * @param max_size Maximum graphlet size (3, 4, or 5).
 * @return std::vector<std::vector<long long>> GDD[v][orbit] = count for vertex v in orbit.
 */
GRAPHLIB_API std::vector<std::vector<long long>> graphlet_degree_distribution(const Graph& g, int max_size = 4);

/**
 * @brief Finds all occurrences of a pattern in the graph.
 * 
 * @param g The graph to search in.
 * @param pattern The pattern to find.
 * @param max_matches Maximum matches to return.
 * @return std::vector<std::vector<int>> Each match as a list of vertices.
 */
GRAPHLIB_API std::vector<std::vector<int>> find_motif_occurrences(const Graph& g, const Graph& pattern, int max_matches = 1000);

/**
 * @brief Computes transitivity (global clustering coefficient).
 * 
 * Transitivity = 3 * triangles / connected_triples
 * 
 * @param g The graph.
 * @return double Global clustering coefficient.
 */
GRAPHLIB_API double transitivity(const Graph& g);

/**
 * @brief Counts paths of length k between two vertices.
 * 
 * @param g The graph.
 * @param u Source vertex.
 * @param v Target vertex.
 * @param k Path length.
 * @return long long Number of paths of length k from u to v.
 */
GRAPHLIB_API long long count_paths_of_length(const Graph& g, int u, int v, int k);

/**
 * @brief Counts all simple paths up to length max_length.
 * 
 * @param g The graph.
 * @param max_length Maximum path length.
 * @return std::vector<long long> counts[k] = number of paths of length k.
 */
GRAPHLIB_API std::vector<long long> count_simple_paths(const Graph& g, int max_length);

}

#endif
