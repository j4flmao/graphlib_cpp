#ifndef GRAPHLIB_CENTROID_DECOMPOSITION_H
#define GRAPHLIB_CENTROID_DECOMPOSITION_H

#include "export.h"
#include "graph_core.h"
#include <vector>

namespace graphlib {

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

class GRAPHLIB_API CentroidDecomposition {
private:
    const Graph& graph_;
    int n_;
    std::vector<int> parent_; // Parent in centroid tree
    std::vector<int> level_;  // Depth in centroid tree
    std::vector<int> subtree_size_;
    std::vector<bool> is_removed_;
    int root_;

    void calc_subtree_size(int u, int p);
    int find_centroid(int u, int p, int n);
    void decompose(int u, int p, int lvl);

public:
    explicit CentroidDecomposition(const Graph& graph);

    // Returns the root of the centroid tree
    int get_root() const;

    // Returns the parent of u in the centroid tree (-1 if u is root)
    int get_parent(int u) const;

    // Returns the level (depth) of u in the centroid tree (root is 0)
    int get_level(int u) const;

    // Helper to get the full centroid tree structure
    std::vector<std::vector<int>> get_tree() const;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

} // namespace graphlib

#endif // GRAPHLIB_CENTROID_DECOMPOSITION_H
