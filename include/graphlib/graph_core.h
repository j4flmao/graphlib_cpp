#ifndef GRAPHLIB_GRAPH_CORE_H
#define GRAPHLIB_GRAPH_CORE_H

#include "export.h"
#include <cstddef>
#include <vector>
#include <tuple>

namespace graphlib {

struct Edge {
    int to;
    long long weight;
    int reverse_idx;
    Edge* next;

    Edge(int to, long long weight = 1);
    ~Edge() {}
};

class GRAPHLIB_API Graph {
protected:
    int n_;
    Edge** adj_;
    bool directed_;

    void clear_adj_list();

public:
    Graph();
    explicit Graph(int n, bool directed = true);
    virtual ~Graph();

    Graph(const Graph&) = delete;
    Graph& operator=(const Graph&) = delete;
    Graph(Graph&& other) noexcept;
    Graph& operator=(Graph&& other) noexcept;

    void add_edge(int from, int to, long long weight = 1);
    int vertex_count() const { return n_; }
    bool is_directed() const { return directed_; }

    Edge* get_edges(int vertex) const { return adj_[vertex]; }
};

GRAPHLIB_API Graph make_complete_graph(int n, bool directed = false);
GRAPHLIB_API Graph make_random_graph(int n, int m, bool directed, long long min_weight, long long max_weight, unsigned int seed);
GRAPHLIB_API bool is_connected(const Graph& g);
GRAPHLIB_API bool is_tree(const Graph& g);
GRAPHLIB_API Graph make_graph_from_edges(int n, const std::vector<std::pair<int, int>>& edges, bool directed = false);
GRAPHLIB_API Graph make_graph_from_edges_one_based(int n, const std::vector<std::pair<int, int>>& edges, bool directed = false);
GRAPHLIB_API Graph make_weighted_graph_from_edges(int n, const std::vector<std::tuple<int, int, long long>>& edges, bool directed = false);
GRAPHLIB_API Graph make_weighted_graph_from_edges_one_based(int n, const std::vector<std::tuple<int, int, long long>>& edges, bool directed = false);
GRAPHLIB_API std::vector<int> bfs_distances(const Graph& g, int source, int unreachable = -1);
GRAPHLIB_API std::vector<int> bfs_multi_source(const Graph& g, const std::vector<int>& sources, int unreachable = -1);
GRAPHLIB_API bool has_eulerian_trail_undirected(const Graph& g);
GRAPHLIB_API bool has_eulerian_cycle_undirected(const Graph& g);
GRAPHLIB_API std::vector<int> eulerian_trail_undirected(const Graph& g);
GRAPHLIB_API bool has_eulerian_trail_directed(const Graph& g);
GRAPHLIB_API bool has_eulerian_cycle_directed(const Graph& g);
GRAPHLIB_API std::vector<int> eulerian_trail_directed(const Graph& g);

} 

#endif
