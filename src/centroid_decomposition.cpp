#include "graphlib/centroid_decomposition.h"
#include <algorithm>

namespace graphlib {

CentroidDecomposition::CentroidDecomposition(const Graph& graph)
    : graph_(graph), n_(graph.vertex_count()), root_(-1) {
    parent_.assign(n_, -1);
    level_.assign(n_, 0);
    subtree_size_.resize(n_);
    is_removed_.assign(n_, false);

    if (n_ > 0) {
        decompose(0, -1, 0);
        // The first centroid found in decompose(0, ...) acts as the root of the centroid tree.
        // However, decompose calls find_centroid which returns the actual centroid.
        // My decompose function sets parent, but doesn't explicitly return the global root.
        // The global root is the one with parent -1.
        for(int i=0; i<n_; ++i) {
            if(parent_[i] == -1) {
                root_ = i;
                break;
            }
        }
    }
}

void CentroidDecomposition::calc_subtree_size(int u, int p) {
    subtree_size_[u] = 1;
    for (Edge* e = graph_.get_edges(u); e != nullptr; e = e->next) {
        int v = e->to;
        if (v != p && !is_removed_[v]) {
            calc_subtree_size(v, u);
            subtree_size_[u] += subtree_size_[v];
        }
    }
}

int CentroidDecomposition::find_centroid(int u, int p, int n) {
    for (Edge* e = graph_.get_edges(u); e != nullptr; e = e->next) {
        int v = e->to;
        if (v != p && !is_removed_[v] && subtree_size_[v] > n / 2) {
            return find_centroid(v, u, n);
        }
    }
    return u;
}

void CentroidDecomposition::decompose(int u, int p, int lvl) {
    calc_subtree_size(u, -1);
    int centroid = find_centroid(u, -1, subtree_size_[u]);

    is_removed_[centroid] = true;
    parent_[centroid] = p;
    level_[centroid] = lvl;

    for (Edge* e = graph_.get_edges(centroid); e != nullptr; e = e->next) {
        int v = e->to;
        if (!is_removed_[v]) {
            decompose(v, centroid, lvl + 1);
        }
    }
}

int CentroidDecomposition::get_root() const {
    return root_;
}

int CentroidDecomposition::get_parent(int u) const {
    if (u < 0 || u >= n_) return -1;
    return parent_[u];
}

int CentroidDecomposition::get_level(int u) const {
    if (u < 0 || u >= n_) return 0;
    return level_[u];
}

std::vector<std::vector<int>> CentroidDecomposition::get_tree() const {
    std::vector<std::vector<int>> tree(n_);
    for (int i = 0; i < n_; ++i) {
        int p = parent_[i];
        if (p != -1) {
            tree[p].push_back(i);
        }
    }
    return tree;
}

} // namespace graphlib
