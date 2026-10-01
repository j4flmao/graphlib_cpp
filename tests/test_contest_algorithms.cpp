#include <gtest/gtest.h>
#include "graphlib/contest_algorithms.h"

using namespace graphlib;

TEST(ContestAlgorithms, WidestPath) {
    Graph g(4, true); g.add_edge(0,1,5); g.add_edge(0,2,3); g.add_edge(1,2,4); g.add_edge(2,3,3); g.add_edge(1,3,2);
    auto d = widest_paths(g, 0);
    EXPECT_EQ(d[3], 3);
}

TEST(ContestAlgorithms, CountsShortestPaths) {
    Graph g(4, true); g.add_edge(0,1,1); g.add_edge(0,2,1); g.add_edge(1,3,1); g.add_edge(2,3,1);
    auto ways = count_shortest_paths(g, 0);
    EXPECT_EQ(ways[3], 2);
}

TEST(ContestAlgorithms, AtMostKEdges) {
    Graph g(3, true); g.add_edge(0,1,2); g.add_edge(1,2,2); g.add_edge(0,2,10);
    EXPECT_EQ(shortest_paths_at_most_k_edges(g, 0, 1)[2], 10);
    EXPECT_EQ(shortest_paths_at_most_k_edges(g, 0, 2)[2], 4);
}

TEST(ContestAlgorithms, LexicographicTopologicalOrder) {
    Graph g(4, true); g.add_edge(0,2); g.add_edge(1,2); g.add_edge(2,3);
    bool cycle = false;
    EXPECT_EQ(lexicographically_smallest_topological_order(g, cycle), (std::vector<int>{0,1,2,3}));
    EXPECT_FALSE(cycle);
}

TEST(ContestAlgorithms, CountsTopologicalOrders) {
    Graph g(3, true); g.add_edge(0, 2); g.add_edge(1, 2);
    EXPECT_EQ(count_topological_orders(g), 2);
}
