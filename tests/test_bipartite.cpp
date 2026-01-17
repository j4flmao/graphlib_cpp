#include <gtest/gtest.h>
#include <graphlib/graphlib.h>

using graphlib::BipartiteGraph;

TEST(BipartiteTest, BipartiteCheckTrue) {
    BipartiteGraph g(2, 2);
    g.add_edge(0, 2);
    g.add_edge(1, 3);
    EXPECT_TRUE(g.is_bipartite());
}

TEST(BipartiteTest, BipartiteCheckFalse) {
    graphlib::Graph g(3, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);
    EXPECT_EQ(g.vertex_count(), 3);
}

TEST(BipartiteTest, MaximumMatchingSimple) {
    BipartiteGraph g(2, 2);
    g.add_edge(0, 2);
    g.add_edge(0, 3);
    g.add_edge(1, 2);
    int matching = g.maximum_matching();
    EXPECT_EQ(matching, 2);
}

TEST(BipartiteTest, HungarianMinCost) {
    BipartiteGraph g(2, 2);
    long long inf = 1000000000LL;

    g.add_edge(0, 2, 1);
    g.add_edge(0, 3, 3);
    g.add_edge(1, 2, 4);
    g.add_edge(1, 3, 2);

    long long cost = g.hungarian_min_cost(inf);
    EXPECT_EQ(cost, 3);
}

