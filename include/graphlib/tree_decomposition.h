#ifndef GRAPHLIB_TREE_DECOMPOSITION_H
#define GRAPHLIB_TREE_DECOMPOSITION_H

#include "export.h"
#include "graph_core.h"
#include <vector>
#include <set>

namespace graphlib {

/**
 * @brief Represents a node in the tree decomposition.
 */
struct Bag {
    int id;
    std::vector<int> vertices; // Vertices in this bag
};

/**
 * @brief Represents a tree decomposition of a graph.
 * A tree decomposition is a tree where each node is a "Bag" of vertices from the original graph.
 * Properties:
 * 1. Every vertex of G is in at least one bag.
 * 2. For every edge (u, v) in G, there is a bag containing both u and v.
 * 3. All bags containing a vertex u form a connected subtree.
 * 
 * Width of decomposition = max(|Bag|) - 1.
 * Treewidth of G = min width over all possible decompositions.
 */
struct TreeDecomposition {
    std::vector<Bag> bags;
    std::vector<std::vector<int>> adj; // Adjacency of bags (tree structure)
    int width;
    
    // Returns index of root bag (usually 0)
    int root() const { return bags.empty() ? -1 : 0; }
};

/**
 * @brief Computes a Tree Decomposition using the Min-Degree Heuristic.
 * This is a heuristic approach and does not guarantee minimal treewidth (which is NP-hard).
 * However, it works well for many practical graphs.
 * 
 * Algorithm:
 * 1. Minimum Degree fill-in (Elimination Game).
 * 2. Construct clique tree from the chordal completion.
 * 
 * @param g The graph.
 * @return TreeDecomposition The computed decomposition.
 */
GRAPHLIB_API TreeDecomposition compute_tree_decomposition(const Graph& g);

}

#endif
