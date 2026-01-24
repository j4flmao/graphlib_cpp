#ifndef GRAPHLIB_SHORTEST_PATH_H
#define GRAPHLIB_SHORTEST_PATH_H

#include "export.h"
#include <vector>

namespace graphlib {

class GRAPHLIB_API ShortestPath {
private:
    struct Edge {
        int to;
        long long weight;
        bool enabled;
        Edge* next;

        Edge(int to, long long weight);
    };

    int n_;
    Edge** adj_;

    void clear_graph();

public:
    explicit ShortestPath(int n);
    ~ShortestPath();

    ShortestPath(const ShortestPath&) = delete;
    ShortestPath& operator=(const ShortestPath&) = delete;
    ShortestPath(ShortestPath&& other) noexcept;
    ShortestPath& operator=(ShortestPath&& other) noexcept;

    void add_edge(int from, int to, long long weight);

    std::vector<long long> dijkstra(int source, long long inf);
    std::vector<long long> zero_one_bfs(int source, long long inf);
    std::vector<long long> bellman_ford(int source, long long inf, bool& has_negative_cycle);
    std::vector<std::vector<long long>> floyd_warshall(long long inf);
    std::vector<long long> a_star(int source, int target, const std::vector<long long>& heuristic, long long inf);
    std::vector<std::vector<long long>> johnson(long long inf, bool& has_negative_cycle);
    std::vector<long long> multi_source_dijkstra(const std::vector<int>& sources, long long inf);
    long double minimum_mean_cycle(bool& has_cycle);
    
    // Returns the K shortest paths from source to target.
    // Each path is a vector of vertex indices.
    // Returns empty vector if no path exists or k <= 0.
    std::vector<std::vector<int>> k_shortest_paths(int source, int target, int k, long long inf);

private:
    std::vector<long long> dijkstra_with_path(int source, int target, long long inf, std::vector<int>& parent);
};

GRAPHLIB_API std::vector<int> reconstruct_path(int source, int target, const std::vector<int>& parent);

}

#endif
