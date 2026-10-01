#ifndef GRAPHLIB_TEST_RANDOM_GRAPHS_H
#define GRAPHLIB_TEST_RANDOM_GRAPHS_H

#include "graphlib/graph_core.h"
#include <algorithm>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <vector>

namespace graphlib::test {

inline Graph erdos_renyi(int n, double probability, std::uint64_t seed, bool directed = false) {
    if (n < 0 || probability < 0.0 || probability > 1.0) throw std::invalid_argument("Invalid random graph parameters");
    Graph graph(n, directed);
    std::mt19937_64 rng(seed);
    std::bernoulli_distribution choose(probability);
    for (int u = 0; u < n; ++u) for (int v = directed ? 0 : u + 1; v < n; ++v)
        if (u != v && choose(rng)) graph.add_edge(u, v);
    return graph;
}

inline Graph random_tree(int n, std::uint64_t seed) {
    if (n < 0) throw std::invalid_argument("Invalid tree size");
    Graph graph(n, false);
    std::mt19937_64 rng(seed);
    for (int v = 1; v < n; ++v) {
        std::uniform_int_distribution<int> parent(0, v - 1);
        graph.add_edge(v, parent(rng));
    }
    return graph;
}

inline Graph random_dag(int n, double probability, std::uint64_t seed) {
    Graph graph(n, true);
    std::mt19937_64 rng(seed);
    std::bernoulli_distribution choose(probability);
    for (int u = 0; u < n; ++u) for (int v = u + 1; v < n; ++v)
        if (choose(rng)) graph.add_edge(u, v);
    return graph;
}

} // namespace graphlib::test

#endif
