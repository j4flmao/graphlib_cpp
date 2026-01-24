#ifndef GRAPHLIB_MAX_FLOW_H
#define GRAPHLIB_MAX_FLOW_H

#include "export.h"
#include "graph_core.h"
#include <cstddef>
#include <utility>
#include <vector>

namespace graphlib {

struct CirculationEdge {
    int from;
    int to;
    long long lower;
    long long upper;
    long long cost;
};

class GRAPHLIB_API MaxFlow {
private:
    struct Edge {
        int to;
        long long cap;
        long long cost;
        Edge* rev;
        Edge* next;

        Edge(int to, long long cap, long long cost = 0);
    };

    int n_;
    Edge** graph_;
    int* level_;
    Edge** iter_;

    void clear_graph();
    bool bfs(int source, int sink);
    long long dfs(int v, int sink, long long f);

public:
    explicit MaxFlow(int n);
    ~MaxFlow();

    MaxFlow(const MaxFlow&) = delete;
    MaxFlow& operator=(const MaxFlow&) = delete;
    MaxFlow(MaxFlow&& other) noexcept;
    MaxFlow& operator=(MaxFlow&& other) noexcept;

    void add_edge(int from, int to, long long capacity);
    void add_edge(int from, int to, long long capacity, long long cost);
    void add_undirected_edge(int u, int v, long long capacity);
    void add_undirected_edge(int u, int v, long long capacity, long long cost);
    long long edmonds_karp(int source, int sink);
    long long dinic(int source, int sink);
    long long push_relabel(int source, int sink);
    void min_cut_reachable_from_source(int source, std::vector<char>& reachable) const;
    std::pair<long long, long long> min_cost_max_flow(int source, int sink);
    int vertex_count() const { return n_; }
};

GRAPHLIB_API bool min_cost_circulation(int n, const std::vector<CirculationEdge>& edges, long long& total_cost);

/**
 * @brief Solves the Maximum Weight Closure problem using Min-Cut.
 * 
 * A closure is a set of vertices C such that if u is in C, then all vertices v reachable from u are also in C.
 * The weight of a closure is the sum of weights of vertices in C.
 * 
 * @param weights Weights of vertices (can be positive or negative).
 * @param dependencies Directed edges (u, v) meaning if u is chosen, v must be chosen.
 * @return The maximum weight of a closure.
 */
GRAPHLIB_API long long maximum_weight_closure(const std::vector<long long>& weights, const std::vector<std::pair<int, int>>& dependencies);

/**
 * @brief Finds the Maximum Density Subgraph.
 * Density = |E(S)| / |S|.
 * Uses binary search on density + Min-Cut.
 * 
 * @param g The undirected graph.
 * @return Pair of {max_density, vertices_in_subgraph}.
 */
GRAPHLIB_API std::pair<double, std::vector<int>> maximum_density_subgraph(const Graph& g);

}

#endif
