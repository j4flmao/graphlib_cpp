#ifndef GRAPHLIB_TREE_H
#define GRAPHLIB_TREE_H

#include "export.h"
#include "graph_core.h"
#include <vector>
#include <utility>

namespace graphlib {

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

class GRAPHLIB_API TreeLCA : public Graph {
private:
    int root_;
    int log_;
    std::vector<int> depth_;
    std::vector<std::vector<int>> up_;
    std::vector<int> tin_;
    std::vector<int> tout_;
    std::vector<int> subtree_size_;
    int timer_;
    bool built_;
    std::vector<int> heavy_;
    std::vector<int> head_;
    std::vector<int> pos_;
    int cur_pos_;

    void dfs_build(int v, int p);
    void dfs_decompose(int v, int head);

public:
    explicit TreeLCA(int n);

    void build(int root);
    int lca(int u, int v) const;
    int parent(int v) const;
    int depth(int v) const;
    int distance(int u, int v) const;
    int kth_on_path(int u, int v, int k) const;
    bool is_ancestor(int u, int v) const;
    int kth_ancestor(int v, int k) const;
    int subtree_size(int v) const;
    int hld_pos(int v) const;
    int hld_head(int v) const;
    void hld_decompose_path(int u, int v, std::vector<std::pair<int, int>>& segments) const;
};

class GRAPHLIB_API TreePathSum {
private:
    const TreeLCA* tree_;
    int n_;
    int size_;
    std::vector<long long> data_;
    std::vector<long long> seg_;

    void build_segment_tree();
    void build_from_data();
    void point_update_internal(int idx, long long value);
    long long range_query_internal(int left, int right) const;

public:
    TreePathSum(const TreeLCA& tree, const std::vector<long long>& values);

    int size() const { return n_; }
    long long get_value(int v) const;
    void set_value(int v, long long value);
    void add_value(int v, long long delta);
    long long query_path(int u, int v) const;
    long long query_subtree(int v) const;
};

class GRAPHLIB_API TreePathMax {
private:
    const TreeLCA* tree_;
    int n_;
    int size_;
    std::vector<long long> data_;
    std::vector<long long> seg_;

    void build_segment_tree();
    void build_from_data();
    void point_update_internal(int idx, long long value);
    long long range_query_internal(int left, int right) const;

public:
    TreePathMax(const TreeLCA& tree, const std::vector<long long>& values);

    int size() const { return n_; }
    long long get_value(int v) const;
    void set_value(int v, long long value);
    long long query_path(int u, int v) const;
    long long query_subtree(int v) const;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

}

#endif

