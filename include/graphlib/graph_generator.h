#ifndef GRAPHLIB_GRAPH_GENERATOR_H
#define GRAPHLIB_GRAPH_GENERATOR_H

#include "export.h"
#include "graph_core.h"

namespace graphlib {
namespace generator {

// Generates a random graph G(n, p) (Erdos-Renyi model).
// n: number of vertices.
// p: probability of edge creation.
// directed: if true, edges are directed.
// seed: random seed (-1 for random device).
GRAPHLIB_API Graph random_graph(int n, double p, bool directed = false, int seed = -1);

// Generates a random tree with n vertices.
// Uses Prufer sequence or random attachment.
GRAPHLIB_API Graph random_tree(int n, int seed = -1);

// Generates a complete graph K_n.
GRAPHLIB_API Graph complete_graph(int n, bool directed = false);

// Generates a cycle graph C_n.
GRAPHLIB_API Graph cycle_graph(int n, bool directed = false);

// Generates a path graph P_n.
GRAPHLIB_API Graph path_graph(int n, bool directed = false);

// Generates a 2D grid graph.
// rows, cols: dimensions.
GRAPHLIB_API Graph grid_2d_graph(int rows, int cols, bool directed = false);

// Generates a Watts-Strogatz small-world graph.
// n: number of nodes.
// k: each node is connected to k nearest neighbors in ring topology.
// p: probability of rewiring each edge.
GRAPHLIB_API Graph watts_strogatz(int n, int k, double p, int seed = -1);

// Generates a Barabasi-Albert preferential attachment graph.
// n: number of nodes.
// m: number of edges to attach from a new node to existing nodes.
GRAPHLIB_API Graph barabasi_albert(int n, int m, int seed = -1);

// Generates a random tree using Prufer sequence (Uniformly Random Tree).
// n: number of vertices.
GRAPHLIB_API Graph random_tree_prufer(int n, int seed = -1);

// Generates an R-MAT (Recursive Matrix) graph.
// Useful for simulating social networks and web graphs.
// n: number of vertices (will be rounded up to next power of 2).
// m: number of edges.
// a, b, c, d: probabilities for the 4 quadrants (must sum to 1).
GRAPHLIB_API Graph rmat_graph(int n, int m, double a, double b, double c, double d, int seed = -1);

// Generates a Random Geometric Graph.
// Places n nodes uniformly in a unit square (or hypercube).
// Connects nodes if euclidean distance <= radius.
// dim: dimension (currently only 2D supported).
GRAPHLIB_API Graph random_geometric_graph(int n, double radius, int dim = 2, int seed = -1);

} // namespace generator
} // namespace graphlib

#endif // GRAPHLIB_GRAPH_GENERATOR_H
