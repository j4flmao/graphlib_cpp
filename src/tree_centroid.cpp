#include "graphlib/tree_centroid.h"
#include <algorithm>
#include <stdexcept>

namespace graphlib {

CentroidDecomposition::CentroidDecomposition(const Graph& tree)
    : n_(tree.vertex_count()),
      centroid_parent_(n_, -1),
      blocked_(n_, false),
      subtree_size_(n_, 0),
      root_(-1) {
    
    if (n_ > 0) {
        decompose(0, -1, tree);
        
        // Find the root (node with parent -1)
        // If decompose sets root correctly first, we can just capture it.
        // But since it's recursive, the first call to find_centroid determines the global root.
        // Actually decompose call finds centroid of the whole tree first.
        // But we didn't store it explicitly.
        // Let's iterate to find it.
        for(int i=0; i<n_; ++i) {
            if (centroid_parent_[i] == -1) {
                root_ = i;
                break;
            }
        }
    }
}

int CentroidDecomposition::get_centroid_parent(int u) const {
    if (u < 0 || u >= n_) return -1;
    return centroid_parent_[u];
}

int CentroidDecomposition::get_centroid_root() const {
    return root_;
}

void CentroidDecomposition::calc_subtree_sizes(int u, int p, const Graph& g) {
    subtree_size_[u] = 1;
    for (Edge* e = g.get_edges(u); e != nullptr; e = e->next) {
        int v = e->to;
        if (v != p && !blocked_[v]) {
            calc_subtree_sizes(v, u, g);
            subtree_size_[u] += subtree_size_[v];
        }
    }
}

int CentroidDecomposition::find_centroid(int u, int p, int size, const Graph& g) {
    for (Edge* e = g.get_edges(u); e != nullptr; e = e->next) {
        int v = e->to;
        if (v != p && !blocked_[v] && subtree_size_[v] > size / 2) {
            return find_centroid(v, u, size, g);
        }
    }
    return u;
}

void CentroidDecomposition::decompose(int u, int p, const Graph& g) {
    calc_subtree_sizes(u, -1, g);
    int total_size = subtree_size_[u];
    
    int centroid = find_centroid(u, -1, total_size, g);
    
    centroid_parent_[centroid] = p; // Parent in centroid tree
    blocked_[centroid] = true;
    
    for (Edge* e = g.get_edges(centroid); e != nullptr; e = e->next) {
        int v = e->to;
        if (!blocked_[v]) {
            decompose(v, centroid, g);
        }
    }
}

}
