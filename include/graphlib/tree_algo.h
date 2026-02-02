#ifndef GRAPHLIB_TREE_ALGO_H
#define GRAPHLIB_TREE_ALGO_H

#include "export.h"
#include "graph_core.h"
#include <vector>

namespace graphlib {

// Returns the diameter of the tree and the path corresponding to the diameter.
// Output format: {u, v, length, path}
struct TreeDiameterResult {
    int u;
    int v;
    int length; // Number of edges
    std::vector<int> path;
};
GRAPHLIB_API TreeDiameterResult get_tree_diameter(const Graph& tree);

// Solves the Maximum Weight Independent Set problem on a tree.
// Returns the total weight.
// The set of selected vertices is stored in 'selected_nodes'.
GRAPHLIB_API long long max_weight_independent_set_tree(const Graph& tree, const std::vector<long long>& weights, std::vector<int>& selected_nodes);

// Solves the Minimum Dominating Set problem on a tree.
// Returns the size of the minimum dominating set.
// The set of selected vertices is stored in 'selected_nodes'.
GRAPHLIB_API int min_dominating_set_tree(const Graph& tree, std::vector<int>& selected_nodes);

// Returns the center(s) of the tree.
// A tree has 1 or 2 centers.
GRAPHLIB_API std::vector<int> get_tree_centers(const Graph& tree);

}

#endif
