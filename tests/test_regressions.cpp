#include <gtest/gtest.h>

#include "graphlib/directed_mst.h"
#include "graphlib/graph_core.h"
#include "graphlib/max_flow.h"
#include "graphlib/planarity.h"

#include <stdexcept>
#include <vector>

using graphlib::DirectedEdge;
using graphlib::MaxFlow;

TEST(RegressionFlowTest, RejectsEqualTerminalsAcrossAlgorithms) {
    MaxFlow flow(3);
    flow.add_edge(0, 1, 5);

    EXPECT_THROW(flow.dinic(0, 0), std::invalid_argument);
    EXPECT_THROW(flow.edmonds_karp(1, 1), std::invalid_argument);
    EXPECT_THROW(flow.push_relabel(2, 2), std::invalid_argument);
    EXPECT_THROW(flow.min_cost_max_flow(0, 0), std::invalid_argument);
}

TEST(RegressionFlowTest, RejectsInvalidEdges) {
    EXPECT_THROW(MaxFlow(-1), std::invalid_argument);
    MaxFlow flow(2);
    EXPECT_THROW(flow.add_edge(-1, 0, 1), std::out_of_range);
    EXPECT_THROW(flow.add_edge(0, 2, 1), std::out_of_range);
    EXPECT_THROW(flow.add_edge(0, 1, -1), std::invalid_argument);
}

TEST(RegressionFlowTest, DetectsReachableNegativeCycle) {
    MaxFlow flow(4);
    flow.add_edge(0, 1, 1, 1);
    flow.add_edge(1, 2, 1, -5);
    flow.add_edge(2, 1, 1, -5);
    flow.add_edge(2, 3, 1, 1);
    EXPECT_THROW(flow.min_cost_max_flow(0, 3), std::runtime_error);
}

TEST(RegressionDirectedMSTTest, ContractsCycle) {
    std::vector<DirectedEdge> edges = {
        {0, 1, 1, 10}, {1, 2, 1, 11}, {2, 1, 1, 12},
        {0, 2, 5, 13}, {2, 3, 1, 14}, {0, 3, 10, 15}
    };
    std::vector<int> selected;
    EXPECT_EQ(graphlib::directed_mst(4, 0, edges, selected), 3);
    ASSERT_EQ(selected.size(), 3u);
}

TEST(RegressionDirectedMSTTest, RejectsUnreachableVertex) {
    std::vector<DirectedEdge> edges = {{0, 1, 1, 1}};
    std::vector<int> selected;
    EXPECT_EQ(graphlib::directed_mst(3, 0, edges, selected), -1);
    EXPECT_TRUE(selected.empty());
}

TEST(RegressionGraphTest, EmptyGraphIsValidValue) {
    Graph graph(0);
    EXPECT_EQ(graph.vertex_count(), 0);
    EXPECT_TRUE(graph.is_directed());
    EXPECT_TRUE(graphlib::is_planar(graph));
    EXPECT_TRUE(graphlib::get_planar_faces(graph).empty());
}

TEST(RegressionGraphTest, NegativeGraphSizeIsRejected) {
    EXPECT_THROW(Graph(-1), std::invalid_argument);
}

TEST(RegressionGraphTest, CopyIsDeepAndPreservesDirection) {
    Graph original(3, false);
    original.add_edge(0, 1, 7);
    original.add_edge(1, 2, 9);
    Graph copied = original;
    EXPECT_FALSE(copied.is_directed());
    EXPECT_EQ(copied.vertex_count(), 3);
    copied.add_edge(2, 0, 11);
    EXPECT_EQ(original.get_edges(2)->to, 1);
    EXPECT_EQ(copied.get_edges(2)->to, 0);

    Graph assigned(1);
    assigned = original;
    EXPECT_FALSE(assigned.is_directed());
    EXPECT_EQ(assigned.vertex_count(), 3);
}
