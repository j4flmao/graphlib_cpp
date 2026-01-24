#ifndef GRAPHLIB_GRAPH_MEASURES_H
#define GRAPHLIB_GRAPH_MEASURES_H

#include "export.h"
#include "graphlib.h"
#include <vector>
#include <string>

namespace graphlib {

// Returns the eccentricity of each vertex in an unweighted sense (BFS distance).
// Eccentricity(u) = max distance from u to any other vertex.
// For disconnected graphs, returns -1 for vertices that cannot reach all others (or conceptually infinity).
// In this implementation, if a node cannot reach some other node, its eccentricity is -1.
GRAPHLIB_API std::vector<int> eccentricity(const Graph& g);

// Returns the diameter of the graph (max eccentricity).
// Returns -1 if graph is disconnected.
GRAPHLIB_API int diameter(const Graph& g);

// Returns the radius of the graph (min eccentricity among non-infinite ones).
// Returns -1 if graph is disconnected (or should it be min of valid ones? Standard definition requires connected).
// We return -1 if graph is disconnected.
GRAPHLIB_API int radius(const Graph& g);

// Returns the center of the graph (vertices with eccentricity == radius).
GRAPHLIB_API std::vector<int> center(const Graph& g);

// Returns the periphery of the graph (vertices with eccentricity == diameter).
GRAPHLIB_API std::vector<int> periphery(const Graph& g);

// Returns the closeness centrality of each vertex.
// Closeness(u) = (n-1) / sum(dist(u, v) for all v).
// For disconnected graphs, typically normalized or sum of reciprocal distances is used.
// Here we use the standard formula for connected component: (n-1) / sum(dist).
// If disconnected, we can use 0 or local component size.
// We'll return 0 for isolated nodes.
GRAPHLIB_API std::vector<double> closeness_centrality(const Graph& g);

// Returns the betweenness centrality of each vertex.
// Brandes' Algorithm (O(VE) for unweighted).
GRAPHLIB_API std::vector<double> betweenness_centrality(const Graph& g);

// Returns the PageRank of each vertex.
// d: damping factor (usually 0.85).
// iterations: number of iterations (usually 100).
GRAPHLIB_API std::vector<double> pagerank(const Graph& g, double d = 0.85, int iterations = 100);

// Returns the Local Clustering Coefficient for each vertex.
// C_i = (number of edges between neighbors of i) / (k_i * (k_i - 1))  [directed: k_i * (k_i - 1), undirected: k_i * (k_i - 1) / 2]
// We assume the graph is treated as undirected for this measure usually, but definition varies.
// This implementation treats the graph as UNDIRECTED.
GRAPHLIB_API std::vector<double> clustering_coefficient(const Graph& g);

// Returns the Average Clustering Coefficient of the graph.
GRAPHLIB_API double average_clustering_coefficient(const Graph& g);

// Returns the core number for each vertex (K-Core Decomposition).
// The core number of a vertex is the largest integer k such that the vertex exists in a k-core.
// A k-core is a maximal subgraph that contains nodes of degree k or more.
GRAPHLIB_API std::vector<int> core_number(const Graph& g);

// Returns the Eigenvector Centrality of each vertex.
// Uses power iteration method.
// iterations: number of iterations (usually 100).
GRAPHLIB_API std::vector<double> eigenvector_centrality(const Graph& g, int iterations = 100);

// Returns the Hubs and Authorities scores for each vertex (HITS algorithm).
// Returns a pair of vectors: {hubs, authorities}.
// iterations: number of iterations (usually 100).
GRAPHLIB_API std::pair<std::vector<double>, std::vector<double>> hits(const Graph& g, int iterations = 100);

// Returns Katz Centrality for each vertex.
// x_v = alpha * sum_{u->v} x_u + beta
// alpha: attenuation factor (must be < 1/lambda_max).
// beta: constant weight (usually 1.0).
// iterations: number of iterations.
GRAPHLIB_API std::vector<double> katz_centrality(const Graph& g, double alpha = 0.1, double beta = 1.0, int iterations = 100);

// Returns Jaccard Coefficient for a pair of vertices (u, v).
// J(u, v) = |N(u) intersection N(v)| / |N(u) union N(v)|
// Returns 0.0 if both have no neighbors.
GRAPHLIB_API double jaccard_index(const Graph& g, int u, int v);

// Returns Adamic-Adar Index for a pair of vertices (u, v).
// AA(u, v) = sum_{w in CommonNeighbors(u,v)} 1 / log(degree(w))
GRAPHLIB_API double adamic_adar_index(const Graph& g, int u, int v);

// Detects communities using Label Propagation Algorithm (LPA).
// Returns a vector where result[u] is the community ID of vertex u.
GRAPHLIB_API std::vector<int> label_propagation_communities(const Graph& g, int max_iterations = 100, int seed = -1);

// Calculates Modularity Q of a graph given a community structure.
// communities: vector where communities[u] is the community ID of u.
// Q = (1/2m) * sum_{ij} [ A_ij - (k_i * k_j) / (2m) ] * delta(c_i, c_j)
GRAPHLIB_API double modularity(const Graph& g, const std::vector<int>& communities);

// Computes the Weisfeiler-Lehman (WL) Graph Hash.
// Useful for checking graph isomorphism (same hash => likely isomorphic).
// iterations: number of WL refinement steps (usually 2-5 is enough).
GRAPHLIB_API std::string weisfeiler_lehman_hash(const Graph& g, int iterations = 3);

}

#endif
