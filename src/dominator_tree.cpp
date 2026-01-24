#include "graphlib/dominator_tree.h"
#include <algorithm>
#include <functional>

namespace graphlib {

DominatorTree::DominatorTree(const Graph& graph, int root)
    : graph_(graph), root_(root), n_(graph.vertex_count()), timer_(0) {
    if (root_ >= 0 && root_ < n_) {
        build();
    } else {
        // Handle invalid root or empty graph gracefully
        dfn_.assign(n_, -1);
        idom_.assign(n_, -1);
    }
}

void DominatorTree::dfs(int u) {
    dfn_[u] = timer_;
    rev_dfn_[timer_] = u;
    timer_++;
    
    // In Lengauer-Tarjan, we need predecessors in DFS tree and non-tree edges.
    // The graph_ is the original graph. We traverse it to build the DFS tree.
    for (Edge* e = graph_.get_edges(u); e != nullptr; e = e->next) {
        int v = e->to;
        if (dfn_[v] == -1) {
            parent_[v] = u;
            dfs(v);
        }
    }
}

int DominatorTree::find(int u, bool compress) {
    if (dsu_ancestor_[u] == u) {
        return u;
    }
    int ancestor = dsu_ancestor_[u];
    if (dsu_ancestor_[ancestor] != ancestor) {
        int root = find(ancestor, true); // Path compression
        if (dfn_[sdom_[dsu_label_[ancestor]]] < dfn_[sdom_[dsu_label_[u]]]) {
            dsu_label_[u] = dsu_label_[ancestor];
        }
        dsu_ancestor_[u] = root;
    }
    return dsu_ancestor_[u];
}

void DominatorTree::build() {
    dfn_.assign(n_, -1);
    rev_dfn_.assign(n_, -1);
    parent_.assign(n_, -1);
    sdom_.resize(n_);
    idom_.assign(n_, -1);
    dsu_ancestor_.resize(n_);
    dsu_label_.resize(n_);

    // 1. DFS to compute DFS numbers and parent pointers
    dfs(root_);

    // Initialize sdom, dsu
    for (int i = 0; i < n_; ++i) {
        sdom_[i] = i;
        dsu_ancestor_[i] = i;
        dsu_label_[i] = i;
    }

    std::vector<std::vector<int>> bucket(n_);
    // Precompute reverse graph (incoming edges) for efficiency if not available
    // But Graph only provides outgoing edges. We need incoming edges for the algorithm.
    std::vector<std::vector<int>> rev_adj(n_);
    for (int u = 0; u < n_; ++u) {
        for (Edge* e = graph_.get_edges(u); e != nullptr; e = e->next) {
            rev_adj[e->to].push_back(u);
        }
    }

    // 2. Iterate in reverse DFS order (excluding root)
    for (int i = timer_ - 1; i >= 1; --i) {
        int w = rev_dfn_[i];
        
        for (int v : rev_adj[w]) {
            if (dfn_[v] != -1) { // Only consider reachable nodes
                // Find node u with minimum sdom in the path from v to w in the DFS tree
                // find(v) performs path compression and updates dsu_label
                find(v); 
                
                int u = dsu_label_[v];
                
                if (dfn_[sdom_[u]] < dfn_[sdom_[w]]) {
                    sdom_[w] = sdom_[u];
                }
            }
        }
        
        bucket[sdom_[w]].push_back(w);
        int p = parent_[w];
        dsu_ancestor_[w] = p; // Link
        
        for (int v : bucket[p]) {
             find(v);
             int u = dsu_label_[v];
             
             if (sdom_[u] == sdom_[v]) {
                 idom_[v] = sdom_[v];
             } else {
                 idom_[v] = u;
             }
        }
        bucket[p].clear();
    }
    
    // 3. Finalize idom
    for (int i = 1; i < timer_; ++i) {
        int w = rev_dfn_[i];
        if (idom_[w] != sdom_[w]) {
            idom_[w] = idom_[idom_[w]];
        }
    }
    
    idom_[root_] = -1; // Root has no dominator
}

int DominatorTree::get_idom(int u) const {
    if (u < 0 || u >= n_) return -1;
    return idom_[u];
}

std::vector<std::vector<int>> DominatorTree::get_tree() const {
    std::vector<std::vector<int>> tree(n_);
    for (int i = 0; i < n_; ++i) {
        if (idom_[i] != -1) {
            tree[idom_[i]].push_back(i);
        }
    }
    return tree;
}

bool DominatorTree::dominates(int u, int v) const {
    if (u == v) return true;
    if (idom_[v] == -1) return false; // v is root or unreachable
    if (u == idom_[v]) return true;
    
    int curr = v;
    while (curr != -1 && curr != u) {
        curr = idom_[curr];
    }
    return curr == u;
}

} // namespace graphlib
