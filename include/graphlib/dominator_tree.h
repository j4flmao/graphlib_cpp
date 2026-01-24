#ifndef GRAPHLIB_DOMINATOR_TREE_H
#define GRAPHLIB_DOMINATOR_TREE_H

#include "export.h"
#include "graph_core.h"
#include <vector>

namespace graphlib {

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

class GRAPHLIB_API DominatorTree {
private:
    const Graph& graph_;
    int root_;
    int n_;
    std::vector<int> dfn_;
    std::vector<int> rev_dfn_;
    std::vector<int> parent_;
    std::vector<int> sdom_;
    std::vector<int> idom_;
    std::vector<int> dsu_ancestor_;
    std::vector<int> dsu_label_;
    int timer_;

    void dfs(int u);
    int find(int u, bool compress = true);
    void build();

public:
    DominatorTree(const Graph& graph, int root);

    // Returns the immediate dominator of u. Returns -1 if u is not reachable from root or u is root.
    int get_idom(int u) const;

    // Returns true if u dominates v.
    bool dominates(int u, int v) const;

    // Returns the dominator tree as an adjacency list.
    std::vector<std::vector<int>> get_tree() const;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

} // namespace graphlib

#endif // GRAPHLIB_DOMINATOR_TREE_H
