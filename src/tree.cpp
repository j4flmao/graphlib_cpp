#include "graphlib/tree.h"
#include <queue>
#include <stdexcept>

namespace graphlib {

TreeLCA::TreeLCA(int n)
    : Graph(n, false),
      root_(0),
      log_(0),
      depth_(),
      up_(),
      tin_(),
      tout_(),
      subtree_size_(),
      timer_(0),
      built_(false),
      heavy_(),
      head_(),
      pos_(),
      cur_pos_(0) {
}

void TreeLCA::build(int root) {
    int n = vertex_count();
    if (n <= 0) {
        throw std::invalid_argument("TreeLCA requires positive number of vertices");
    }
    if (root < 0 || root >= n) {
        throw std::out_of_range("Root index out of range in TreeLCA::build");
    }

    root_ = root;
    log_ = 1;
    while ((1 << log_) <= n) {
        log_++;
    }

    depth_.assign(n, 0);
    up_.assign(log_, std::vector<int>(n, -1));
    tin_.assign(n, -1);
    tout_.assign(n, 0);
    subtree_size_.assign(n, 0);
    timer_ = 0;
    heavy_.assign(n, -1);
    head_.assign(n, 0);
    pos_.assign(n, 0);
    cur_pos_ = 0;

    dfs_build(root_, -1);

    for (int k = 1; k < log_; k++) {
        for (int v = 0; v < n; v++) {
            int mid = up_[k - 1][v];
            if (mid == -1) {
                up_[k][v] = -1;
            } else {
                up_[k][v] = up_[k - 1][mid];
            }
        }
    }

    dfs_decompose(root_, root_);

    built_ = true;
}

void TreeLCA::dfs_build(int v, int p) {
    tin_[v] = timer_++;
    up_[0][v] = p;
    subtree_size_[v] = 1;
    int best_size = 0;
    Edge* e = get_edges(v);
    while (e) {
        int to = e->to;
        if (to != p) {
            if (tin_[to] != -1) {
                // Cycle detected or already visited (not a tree)
                // For robustness, we can ignore it (DFS tree) or throw.
                // Given strict requirements, let's ignore it to avoid infinite loop,
                // effectively building a DFS tree of the component.
                e = e->next;
                continue; 
            }
            depth_[to] = depth_[v] + 1;
            dfs_build(to, v);
            subtree_size_[v] += subtree_size_[to];
            if (subtree_size_[to] > best_size) {
                best_size = subtree_size_[to];
                heavy_[v] = to;
            }
        }
        e = e->next;
    }
    tout_[v] = timer_++;
}

void TreeLCA::dfs_decompose(int v, int head) {
    head_[v] = head;
    pos_[v] = cur_pos_;
    cur_pos_++;

    int h = heavy_[v];
    if (h != -1) {
        dfs_decompose(h, head);
    }

    Edge* e = get_edges(v);
    while (e) {
        int to = e->to;
        // Only visit children in the DFS tree
        if (to != up_[0][v] && to != h && up_[0][to] == v) {
            dfs_decompose(to, to);
        }
        e = e->next;
    }
}

int TreeLCA::lca(int u, int v) const {
    if (!built_) {
        return -1;
    }
    int n = vertex_count();
    if (u < 0 || v < 0 || u >= n || v >= n) {
        return -1;
    }

    int a = u;
    int b = v;
    if (depth_[a] < depth_[b]) {
        int tmp = a;
        a = b;
        b = tmp;
    }

    int diff = depth_[a] - depth_[b];
    for (int k = 0; k < log_; k++) {
        if (diff & (1 << k)) {
            a = up_[k][a];
            if (a == -1) {
                break;
            }
        }
    }

    if (a == b) {
        return a;
    }

    for (int k = log_ - 1; k >= 0; k--) {
        if (up_[k][a] != up_[k][b]) {
            a = up_[k][a];
            b = up_[k][b];
        }
    }

    return up_[0][a];
}

int TreeLCA::parent(int v) const {
    if (!built_) {
        return -1;
    }
    int n = vertex_count();
    if (v < 0 || v >= n) {
        return -1;
    }
    return up_[0][v];
}

int TreeLCA::depth(int v) const {
    if (!built_) {
        return 0;
    }
    int n = vertex_count();
    if (v < 0 || v >= n) {
        return 0;
    }
    return depth_[v];
}

int TreeLCA::distance(int u, int v) const {
    int w = lca(u, v);
    if (w == -1) {
        return -1;
    }
    int du = depth(u);
    int dv = depth(v);
    int dw = depth(w);
    return du + dv - 2 * dw;
}

int TreeLCA::kth_on_path(int u, int v, int k) const {
    if (!built_) {
        return -1;
    }
    int n = vertex_count();
    if (u < 0 || v < 0 || u >= n || v >= n) {
        return -1;
    }
    if (k < 0) {
        return -1;
    }

    int w = lca(u, v);
    if (w == -1) {
        return -1;
    }

    int du = depth(u);
    int dv = depth(v);
    int dw = depth(w);
    int total = du + dv - 2 * dw;
    if (k > total) {
        return -1;
    }

    int dist_uw = du - dw;
    if (k <= dist_uw) {
        return kth_ancestor(u, k);
    }

    int steps_from_v_up = total - k;
    return kth_ancestor(v, steps_from_v_up);
}

bool TreeLCA::is_ancestor(int u, int v) const {
    if (!built_) {
        return false;
    }
    int n = vertex_count();
    if (u < 0 || v < 0 || u >= n || v >= n) {
        return false;
    }
    if (tin_[u] <= tin_[v] && tout_[v] <= tout_[u]) {
        return true;
    }
    return false;
}

int TreeLCA::kth_ancestor(int v, int k) const {
    if (!built_) {
        return -1;
    }
    int n = vertex_count();
    if (v < 0 || v >= n || k < 0) {
        return -1;
    }
    int x = v;
    for (int i = 0; i < log_; i++) {
        if (k & (1 << i)) {
            x = up_[i][x];
            if (x == -1) {
                return -1;
            }
        }
    }
    return x;
}

int TreeLCA::subtree_size(int v) const {
    if (!built_) {
        return 0;
    }
    int n = vertex_count();
    if (v < 0 || v >= n) {
        return 0;
    }
    return subtree_size_[v];
}

int TreeLCA::hld_pos(int v) const {
    if (!built_) {
        return -1;
    }
    int n = vertex_count();
    if (v < 0 || v >= n) {
        return -1;
    }
    return pos_[v];
}

int TreeLCA::hld_head(int v) const {
    if (!built_) {
        return -1;
    }
    int n = vertex_count();
    if (v < 0 || v >= n) {
        return -1;
    }
    return head_[v];
}

void TreeLCA::hld_decompose_path(int u, int v, std::vector<std::pair<int, int>>& segments) const {
    segments.clear();
    if (!built_) {
        return;
    }
    int n = vertex_count();
    if (u < 0 || v < 0 || u >= n || v >= n) {
        return;
    }

    int a = u;
    int b = v;
    while (head_[a] != head_[b]) {
        if (depth_[head_[a]] < depth_[head_[b]]) {
            int tmp = a;
            a = b;
            b = tmp;
        }
        int h = head_[a];
        int l = pos_[h];
        int r = pos_[a];
        segments.push_back(std::pair<int, int>(l, r));
        a = up_[0][h];
        if (a == -1) {
            break;
        }
    }

    if (head_[a] == head_[b]) {
        if (depth_[a] < depth_[b]) {
            int tmp = a;
            a = b;
            b = tmp;
        }
        int l = pos_[b];
        int r = pos_[a];
        segments.push_back(std::pair<int, int>(l, r));
    }
}

TreePathSum::TreePathSum(const TreeLCA& tree, const std::vector<long long>& values)
    : tree_(&tree),
      n_(tree.vertex_count()),
      size_(1),
      data_(),
      seg_() {
    if (n_ <= 0) {
        throw std::invalid_argument("TreePathSum requires positive number of vertices");
    }
    if (static_cast<int>(values.size()) != n_) {
        throw std::invalid_argument("TreePathSum values size must match tree vertex count");
    }
    if (tree.hld_pos(0) == -1) {
        throw std::logic_error("TreePathSum requires TreeLCA::build to be called before construction");
    }
    while (size_ < n_) {
        size_ <<= 1;
    }
    data_.assign(n_, 0);
    seg_.assign(2 * size_, 0);
    for (int v = 0; v < n_; v++) {
        int p = tree.hld_pos(v);
        if (p < 0 || p >= n_) {
            throw std::logic_error("Invalid HLD position in TreePathSum construction");
        }
        data_[p] = values[v];
    }
    build_from_data();
}

void TreePathSum::build_segment_tree() {
    for (int i = 0; i < size_; i++) {
        long long value = 0;
        if (i < n_) {
            value = data_[i];
        }
        seg_[size_ + i] = value;
    }
    for (int i = size_ - 1; i > 0; i--) {
        seg_[i] = seg_[i * 2] + seg_[i * 2 + 1];
    }
}

void TreePathSum::build_from_data() {
    build_segment_tree();
}

void TreePathSum::point_update_internal(int idx, long long value) {
    if (idx < 0 || idx >= n_) {
        return;
    }
    data_[idx] = value;
    int pos = size_ + idx;
    seg_[pos] = value;
    while (pos > 1) {
        pos >>= 1;
        seg_[pos] = seg_[pos * 2] + seg_[pos * 2 + 1];
    }
}

long long TreePathSum::range_query_internal(int left, int right) const {
    if (left < 0) {
        left = 0;
    }
    if (right >= n_) {
        right = n_ - 1;
    }
    if (left > right) {
        return 0;
    }
    int l = left + size_;
    int r = right + size_;
    long long res = 0;
    while (l <= r) {
        if (l & 1) {
            res += seg_[l];
            l++;
        }
        if (!(r & 1)) {
            res += seg_[r];
            r--;
        }
        l >>= 1;
        r >>= 1;
    }
    return res;
}

long long TreePathSum::get_value(int v) const {
    if (v < 0 || v >= n_) {
        return 0;
    }
    int idx = tree_->hld_pos(v);
    if (idx < 0 || idx >= n_) {
        return 0;
    }
    return data_[idx];
}

void TreePathSum::set_value(int v, long long value) {
    if (v < 0 || v >= n_) {
        return;
    }
    int idx = tree_->hld_pos(v);
    if (idx < 0 || idx >= n_) {
        return;
    }
    point_update_internal(idx, value);
}

void TreePathSum::add_value(int v, long long delta) {
    if (v < 0 || v >= n_) {
        return;
    }
    int idx = tree_->hld_pos(v);
    if (idx < 0 || idx >= n_) {
        return;
    }
    long long current = data_[idx];
    point_update_internal(idx, current + delta);
}

long long TreePathSum::query_path(int u, int v) const {
    if (u < 0 || v < 0 || u >= n_ || v >= n_) {
        return 0;
    }
    std::vector<std::pair<int, int>> segments;
    tree_->hld_decompose_path(u, v, segments);
    long long res = 0;
    for (const auto& seg : segments) {
        int l = seg.first;
        int r = seg.second;
        if (l > r) {
            int tmp = l;
            l = r;
            r = tmp;
        }
        res += range_query_internal(l, r);
    }
    return res;
}

long long TreePathSum::query_subtree(int v) const {
    if (v < 0 || v >= n_) {
        return 0;
    }
    int idx = tree_->hld_pos(v);
    if (idx < 0 || idx >= n_) {
        return 0;
    }
    int sz = tree_->subtree_size(v);
    if (sz <= 0) {
        return 0;
    }
    int l = idx;
    int r = idx + sz - 1;
    return range_query_internal(l, r);
}

TreePathMax::TreePathMax(const TreeLCA& tree, const std::vector<long long>& values)
    : tree_(&tree),
      n_(tree.vertex_count()),
      size_(1),
      data_(),
      seg_() {
    if (n_ <= 0) {
        throw std::invalid_argument("TreePathMax requires positive number of vertices");
    }
    if (static_cast<int>(values.size()) != n_) {
        throw std::invalid_argument("TreePathMax values size must match tree vertex count");
    }
    if (tree.hld_pos(0) == -1) {
        throw std::logic_error("TreePathMax requires TreeLCA::build to be called before construction");
    }
    while (size_ < n_) {
        size_ <<= 1;
    }
    data_.assign(n_, 0);
    seg_.assign(2 * size_, std::numeric_limits<long long>::min());
    for (int v = 0; v < n_; v++) {
        int p = tree.hld_pos(v);
        if (p < 0 || p >= n_) {
            throw std::logic_error("Invalid HLD position in TreePathMax construction");
        }
        data_[p] = values[v];
    }
    build_from_data();
}

void TreePathMax::build_segment_tree() {
    for (int i = 0; i < size_; i++) {
        long long value = std::numeric_limits<long long>::min();
        if (i < n_) {
            value = data_[i];
        }
        seg_[size_ + i] = value;
    }
    for (int i = size_ - 1; i > 0; i--) {
        seg_[i] = std::max(seg_[i * 2], seg_[i * 2 + 1]);
    }
}

void TreePathMax::build_from_data() {
    build_segment_tree();
}

void TreePathMax::point_update_internal(int idx, long long value) {
    if (idx < 0 || idx >= n_) {
        return;
    }
    data_[idx] = value;
    int pos = size_ + idx;
    seg_[pos] = value;
    while (pos > 1) {
        pos >>= 1;
        seg_[pos] = std::max(seg_[pos * 2], seg_[pos * 2 + 1]);
    }
}

long long TreePathMax::range_query_internal(int left, int right) const {
    if (left < 0) {
        left = 0;
    }
    if (right >= n_) {
        right = n_ - 1;
    }
    if (left > right) {
        return std::numeric_limits<long long>::min();
    }
    int l = left + size_;
    int r = right + size_;
    long long res = std::numeric_limits<long long>::min();
    while (l <= r) {
        if (l & 1) {
            res = std::max(res, seg_[l]);
            l++;
        }
        if (!(r & 1)) {
            res = std::max(res, seg_[r]);
            r--;
        }
        l >>= 1;
        r >>= 1;
    }
    return res;
}

long long TreePathMax::get_value(int v) const {
    if (v < 0 || v >= n_) {
        return std::numeric_limits<long long>::min();
    }
    int idx = tree_->hld_pos(v);
    if (idx < 0 || idx >= n_) {
        return std::numeric_limits<long long>::min();
    }
    return data_[idx];
}

void TreePathMax::set_value(int v, long long value) {
    if (v < 0 || v >= n_) {
        return;
    }
    int idx = tree_->hld_pos(v);
    if (idx < 0 || idx >= n_) {
        return;
    }
    point_update_internal(idx, value);
}

long long TreePathMax::query_path(int u, int v) const {
    if (u < 0 || v < 0 || u >= n_ || v >= n_) {
        return std::numeric_limits<long long>::min();
    }
    std::vector<std::pair<int, int>> segments;
    tree_->hld_decompose_path(u, v, segments);
    long long res = std::numeric_limits<long long>::min();
    for (const auto& seg : segments) {
        int l = seg.first;
        int r = seg.second;
        if (l > r) {
            int tmp = l;
            l = r;
            r = tmp;
        }
        res = std::max(res, range_query_internal(l, r));
    }
    return res;
}

long long TreePathMax::query_subtree(int v) const {
    if (v < 0 || v >= n_) {
        return std::numeric_limits<long long>::min();
    }
    int idx = tree_->hld_pos(v);
    if (idx < 0 || idx >= n_) {
        return std::numeric_limits<long long>::min();
    }
    int sz = tree_->subtree_size(v);
    if (sz <= 0) {
        return std::numeric_limits<long long>::min();
    }
    int l = idx;
    int r = idx + sz - 1;
    return range_query_internal(l, r);
}

}
