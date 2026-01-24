#include "graphlib/tree_algo.h"
#include <algorithm>
#include <map>
#include <queue>
#include <tuple>

namespace graphlib {

std::vector<int> tree_center(const Graph& tree) {
    int n = tree.vertex_count();
    if (n == 0) return {};
    if (n == 1) return {0};

    std::vector<int> degree(n, 0);
    std::queue<int> leaves;

    for (int i = 0; i < n; ++i) {
        // Count neighbors (undirected: out_degree is enough if symmetric)
        // Graph is adjacency list.
        int d = 0;
        Edge* e = tree.get_edges(i);
        while (e) {
            d++;
            e = e->next;
        }
        degree[i] = d;
        if (d <= 1) {
            leaves.push(i);
        }
    }

    int remaining_nodes = n;
    while (remaining_nodes > 2) {
        int sz = static_cast<int>(leaves.size());
        remaining_nodes -= sz;
        for (int i = 0; i < sz; ++i) {
            int u = leaves.front();
            leaves.pop();
            
            Edge* e = tree.get_edges(u);
            while (e) {
                int v = e->to;
                if (degree[v] > 0) { // Valid neighbor
                    degree[v]--;
                    if (degree[v] == 1) {
                        leaves.push(v);
                    }
                }
                e = e->next;
            }
        }
    }

    std::vector<int> centers;
    while (!leaves.empty()) {
        centers.push_back(leaves.front());
        leaves.pop();
    }
    return centers;
}

static std::string dfs_canonical(const Graph& tree, int u, int p) {
    std::vector<std::string> children;
    Edge* e = tree.get_edges(u);
    while (e) {
        int v = e->to;
        if (v != p) {
            children.push_back(dfs_canonical(tree, v, u));
        }
        e = e->next;
    }
    std::sort(children.begin(), children.end());

    std::string res = "(";
    for (const auto& s : children) {
        res += s;
    }
    res += ")";
    return res;
}

std::string tree_canonical_form(const Graph& tree, int root) {
    return dfs_canonical(tree, root, -1);
}

bool are_trees_isomorphic(const Graph& t1, int root1, const Graph& t2, int root2) {
    return tree_canonical_form(t1, root1) == tree_canonical_form(t2, root2);
}

bool are_unrooted_trees_isomorphic(const Graph& t1, const Graph& t2) {
    if (t1.vertex_count() != t2.vertex_count()) return false;
    if (t1.vertex_count() == 0) return true;

    std::vector<int> c1 = tree_center(t1);
    std::vector<int> c2 = tree_center(t2);

    std::string s1 = tree_canonical_form(t1, c1[0]);
    
    for (int root2 : c2) {
        if (s1 == tree_canonical_form(t2, root2)) return true;
    }
    return false;
}

// -----------------------------------------------------------------------------
// LCA Implementation
// -----------------------------------------------------------------------------

LCA::LCA(const Graph& tree, int root) : n_(tree.vertex_count()), depth_(n_) {
    log_n_ = 0;
    while ((1 << log_n_) <= n_) log_n_++;
    up_.assign(n_, std::vector<int>(log_n_ + 1));
    if (n_ > 0) {
        dfs(tree, root, root, 0);
    }
}

void LCA::dfs(const Graph& tree, int u, int p, int d) {
    depth_[u] = d;
    up_[u][0] = p;
    for (int i = 1; i <= log_n_; i++) {
        up_[u][i] = up_[up_[u][i - 1]][i - 1];
    }
    Edge* e = tree.get_edges(u);
    while (e) {
        int v = e->to;
        if (v != p) {
            dfs(tree, v, u, d + 1);
        }
        e = e->next;
    }
}

int LCA::query(int u, int v) const {
    if (depth_[u] < depth_[v]) std::swap(u, v);
    for (int i = log_n_; i >= 0; i--) {
        if (depth_[u] - (1 << i) >= depth_[v]) {
            u = up_[u][i];
        }
    }
    if (u == v) return u;
    for (int i = log_n_; i >= 0; i--) {
        if (up_[u][i] != up_[v][i]) {
            u = up_[u][i];
            v = up_[v][i];
        }
    }
    return up_[u][0];
}

int LCA::dist(int u, int v) const {
    return depth_[u] + depth_[v] - 2 * depth_[query(u, v)];
}

int LCA::kth_ancestor(int u, int k) const {
    for (int i = 0; i <= log_n_; i++) {
        if ((k >> i) & 1) {
            u = up_[u][i];
        }
    }
    return u;
}

// -----------------------------------------------------------------------------
// HLD Implementation
// -----------------------------------------------------------------------------

HLD::HLD(const Graph& tree, int root) 
    : n_(tree.vertex_count()), parent_(n_), depth_(n_), heavy_(n_, -1), 
      head_(n_), pos_(n_), sz_(n_), cur_pos_(0) {
    if (n_ > 0) {
        dfs_sz(tree, root, root);
        dfs_hld(tree, root, root);
    }
}

void HLD::dfs_sz(const Graph& tree, int u, int p) {
    sz_[u] = 1;
    parent_[u] = p;
    depth_[u] = (u == p ? 0 : depth_[p] + 1);
    int max_sz = 0;
    Edge* e = tree.get_edges(u);
    while (e) {
        int v = e->to;
        if (v != p) {
            dfs_sz(tree, v, u);
            sz_[u] += sz_[v];
            if (sz_[v] > max_sz) {
                max_sz = sz_[v];
                heavy_[u] = v;
            }
        }
        e = e->next;
    }
}

void HLD::dfs_hld(const Graph& tree, int u, int h) {
    head_[u] = h;
    pos_[u] = cur_pos_++;
    if (heavy_[u] != -1) {
        dfs_hld(tree, heavy_[u], h);
    }
    Edge* e = tree.get_edges(u);
    while (e) {
        int v = e->to;
        if (v != parent_[u] && v != heavy_[u]) {
            dfs_hld(tree, v, v);
        }
        e = e->next;
    }
}

int HLD::lca(int u, int v) const {
    while (head_[u] != head_[v]) {
        if (depth_[head_[u]] > depth_[head_[v]]) {
            u = parent_[head_[u]];
        } else {
            v = parent_[head_[v]];
        }
    }
    return depth_[u] < depth_[v] ? u : v;
}

int HLD::dist(int u, int v) const {
    return depth_[u] + depth_[v] - 2 * depth_[lca(u, v)];
}

std::vector<std::pair<int, int>> HLD::get_path_intervals(int u, int v) const {
    std::vector<std::pair<int, int>> res;
    while (head_[u] != head_[v]) {
        if (depth_[head_[u]] < depth_[head_[v]]) std::swap(u, v);
        res.push_back({pos_[head_[u]], pos_[u]});
        u = parent_[head_[u]];
    }
    if (depth_[u] > depth_[v]) std::swap(u, v);
    res.push_back({pos_[u], pos_[v]});
    return res;
}

std::pair<int, int> HLD::get_subtree_interval(int u) const {
    return {pos_[u], pos_[u] + sz_[u] - 1};
}

}
