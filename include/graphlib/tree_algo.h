#ifndef GRAPHLIB_TREE_ALGO_H
#define GRAPHLIB_TREE_ALGO_H

#include "export.h"
#include "graph_core.h"
#include <vector>
#include <string>
#include <utility>

namespace graphlib {

// Returns the center(s) of a tree.
// A tree has either 1 center or 2 adjacent centers.
GRAPHLIB_API std::vector<int> tree_center(const Graph& tree);

// Returns the canonical string/hash representation of a rooted tree.
// Two rooted trees are isomorphic if and only if their canonical forms are equal.
// Uses AHU algorithm idea (sorting children tuples).
GRAPHLIB_API std::string tree_canonical_form(const Graph& tree, int root);

// Checks if two rooted trees are isomorphic.
GRAPHLIB_API bool are_trees_isomorphic(const Graph& t1, int root1, const Graph& t2, int root2);

// Checks if two unrooted trees are isomorphic.
// Compares canonical forms rooted at their centers.
GRAPHLIB_API bool are_unrooted_trees_isomorphic(const Graph& t1, const Graph& t2);

// Lowest Common Ancestor using Binary Lifting
class GRAPHLIB_API LCA {
public:
    LCA(const Graph& tree, int root = 0);
    int query(int u, int v) const;
    int dist(int u, int v) const;
    int kth_ancestor(int u, int k) const;

private:
    int n_;
    int log_n_;
#pragma warning(push)
#pragma warning(disable: 4251)
    std::vector<std::vector<int>> up_;
    std::vector<int> depth_;
#pragma warning(pop)
    
    void dfs(const Graph& tree, int u, int p, int d);
};

// Heavy-Light Decomposition
class GRAPHLIB_API HLD {
public:
    HLD(const Graph& tree, int root = 0);
    
    int lca(int u, int v) const;
    int dist(int u, int v) const;
    
    // Returns list of intervals [l, r] in the linearized array (DFS order)
    // corresponding to the path from u to v.
    std::vector<std::pair<int, int>> get_path_intervals(int u, int v) const;
    
    // Returns interval [l, r] for subtree of u
    std::pair<int, int> get_subtree_interval(int u) const;
    
    int get_pos(int u) const { return pos_[u]; }
    int get_head(int u) const { return head_[u]; }
    int get_depth(int u) const { return depth_[u]; }
    int get_parent(int u) const { return parent_[u]; }

private:
    int n_;
#pragma warning(push)
#pragma warning(disable: 4251)
    std::vector<int> parent_;
    std::vector<int> depth_;
    std::vector<int> heavy_;
    std::vector<int> head_;
    std::vector<int> pos_;
    std::vector<int> sz_;
#pragma warning(pop)
    int cur_pos_;
    
    void dfs_sz(const Graph& tree, int u, int p);
    void dfs_hld(const Graph& tree, int u, int h);
};

}

#endif
