#ifndef GRAPHLIB_TREE_DECOMPOSITION_H
#define GRAPHLIB_TREE_DECOMPOSITION_H

#include "export.h"
#include "graph_core.h"
#include <vector>

namespace graphlib {

/**
 * @brief Represents a Tree Decomposition of a graph.
 * 
 * A tree decomposition of a graph G = (V, E) is a pair (T, X), where T is a tree 
 * and X = {X_i | i in V(T)} is a collection of subsets of V (bags), satisfying:
 * 1. The union of all sets X_i equals V.
 * 2. For every edge (u, v) in E, there is a bag X_i containing both u and v.
 * 3. For every vertex v in V, the set of nodes {i | v in X_i} forms a connected subtree of T.
 * 
 * The width of the decomposition is max(|X_i|) - 1.
 * The treewidth of G is the minimum width over all possible tree decompositions.
 */
struct TreeDecompositionResult {
    Graph tree;                     // The decomposition tree structure
    std::vector<std::vector<int>> bags; // bags[i] is the set of vertices in node i of the tree
    int width;                      // The width of this decomposition
};

/**
 * @brief Computes a Tree Decomposition using the Min-Degree heuristic.
 * 
 * This heuristic tends to produce decompositions with reasonably small width,
 * though finding the optimal treewidth is NP-hard.
 * 
 * @param g The input graph (undirected).
 * @return TreeDecompositionResult The computed tree decomposition.
 */
GRAPHLIB_API TreeDecompositionResult tree_decomposition_min_degree(const Graph& g);

}

#endif
