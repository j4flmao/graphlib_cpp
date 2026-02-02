#ifndef GRAPHLIB_TREE_CENTROID_H
#define GRAPHLIB_TREE_CENTROID_H

#include "export.h"
#include "graph_core.h"
#include <vector>

namespace graphlib {

/**
 * @brief Centroid Decomposition of a Tree.
 * 
 * Decomposes a tree into a centroid tree, where the height is guaranteed to be O(log N).
 * Useful for Divide and Conquer on trees.
 */
class GRAPHLIB_API CentroidDecomposition {
public:
    /**
     * @brief Constructs the centroid decomposition.
     * @param tree The input tree (must be connected and acyclic).
     */
    explicit CentroidDecomposition(const Graph& tree);

    /**
     * @brief Returns the parent of vertex 'u' in the Centroid Tree.
     * Returns -1 if 'u' is the root of the Centroid Tree.
     */
    int get_centroid_parent(int u) const;

    /**
     * @brief Returns the root of the Centroid Tree.
     */
    int get_centroid_root() const;

private:
    int n_;
    std::vector<int> centroid_parent_; // Parent in centroid tree
    std::vector<bool> blocked_;        // Blocked nodes during decomposition
    std::vector<int> subtree_size_;    // Temp size for DFS
    int root_;

    void decompose(int u, int p, const Graph& g);
    void calc_subtree_sizes(int u, int p, const Graph& g);
    int find_centroid(int u, int p, int size, const Graph& g);
};

}

#endif
