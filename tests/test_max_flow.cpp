#include <gtest/gtest.h>
#include <graphlib/graphlib.h>

using graphlib::MaxFlow;

TEST(MaxFlowTest, SingleEdge) {
    MaxFlow flow(2);
    flow.add_edge(0, 1, 10);
    EXPECT_EQ(flow.edmonds_karp(0, 1), 10);
    
    MaxFlow flow2(2);
    flow2.add_edge(0, 1, 10);
    EXPECT_EQ(flow2.dinic(0, 1), 10);
}

TEST(MaxFlowTest, ParallelPaths) {
    MaxFlow flow(4);
    flow.add_edge(0, 1, 10);
    flow.add_edge(0, 2, 10);
    flow.add_edge(1, 3, 10);
    flow.add_edge(2, 3, 10);

    EXPECT_EQ(flow.edmonds_karp(0, 3), 20);
    
    MaxFlow flow2(4);
    flow2.add_edge(0, 1, 10);
    flow2.add_edge(0, 2, 10);
    flow2.add_edge(1, 3, 10);
    flow2.add_edge(2, 3, 10);

    EXPECT_EQ(flow2.dinic(0, 3), 20);
}

TEST(MaxFlowTest, NoPath) {
    MaxFlow flow(3);
    flow.add_edge(0, 1, 5);
    EXPECT_EQ(flow.edmonds_karp(0, 2), 0);
    
    MaxFlow flow2(3);
    flow2.add_edge(0, 1, 5);
    EXPECT_EQ(flow2.dinic(0, 2), 0);
}

TEST(MaxFlowTest, MinCostMaxFlowBasic) {
    MaxFlow flow(4);
    flow.add_edge(0, 1, 2, 1);
    flow.add_edge(0, 2, 1, 2);
    flow.add_edge(1, 3, 2, 1);
    flow.add_edge(2, 3, 1, 1);

    std::pair<long long, long long> result = flow.min_cost_max_flow(0, 3);
    EXPECT_EQ(result.first, 3);
    EXPECT_EQ(result.second, 7);
}

TEST(MaxFlowTest, UndirectedEdgeConvenience) {
    MaxFlow flow(2);
    flow.add_undirected_edge(0, 1, 5);
    EXPECT_EQ(flow.dinic(0, 1), 5);
}
