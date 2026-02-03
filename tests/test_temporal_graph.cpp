#include <gtest/gtest.h>
#include "graphlib/temporal_graph.h"
#include <vector>
#include <algorithm>
#include <limits>

using namespace graphlib;

// =============================================================================
// Construction and Basic Accessors
// =============================================================================

TEST(TemporalGraphTest, DefaultConstruction) {
    TemporalGraph g(5);
    EXPECT_EQ(g.vertex_count(), 5);
    EXPECT_EQ(g.edge_count(), 0);
    EXPECT_TRUE(g.is_directed());
}

TEST(TemporalGraphTest, UndirectedConstruction) {
    TemporalGraph g(10, false);
    EXPECT_EQ(g.vertex_count(), 10);
    EXPECT_EQ(g.edge_count(), 0);
    EXPECT_FALSE(g.is_directed());
}

TEST(TemporalGraphTest, EmptyGraphTimestamps) {
    TemporalGraph g(3);
    EXPECT_EQ(g.min_timestamp(), std::numeric_limits<long long>::max());
    EXPECT_EQ(g.max_timestamp(), std::numeric_limits<long long>::min());
}

// =============================================================================
// add_edge and add_edges
// =============================================================================

TEST(TemporalGraphTest, AddSingleEdge) {
    TemporalGraph g(5);
    g.add_edge(0, 1, 100, 10, 5);
    
    EXPECT_EQ(g.edge_count(), 1);
    EXPECT_EQ(g.min_timestamp(), 100);
    EXPECT_EQ(g.max_timestamp(), 100);
    
    const auto& edges = g.edges();
    EXPECT_EQ(edges[0].from, 0);
    EXPECT_EQ(edges[0].to, 1);
    EXPECT_EQ(edges[0].timestamp, 100);
    EXPECT_EQ(edges[0].weight, 10);
    EXPECT_EQ(edges[0].duration, 5);
}

TEST(TemporalGraphTest, AddMultipleEdges) {
    TemporalGraph g(5);
    g.add_edge(0, 1, 10);
    g.add_edge(1, 2, 20);
    g.add_edge(2, 3, 30);
    
    EXPECT_EQ(g.edge_count(), 3);
    EXPECT_EQ(g.min_timestamp(), 10);
    EXPECT_EQ(g.max_timestamp(), 30);
}

TEST(TemporalGraphTest, AddEdgesDefaultWeight) {
    TemporalGraph g(3);
    g.add_edge(0, 1, 50);
    
    const auto& edges = g.edges();
    EXPECT_EQ(edges[0].weight, 1);
    EXPECT_EQ(edges[0].duration, 0);
}

TEST(TemporalGraphTest, AddEdgesBatch) {
    TemporalGraph g(5);
    std::vector<TemporalGraph::TemporalEdge> batch = {
        {0, 1, 1, 100, 0},
        {1, 2, 2, 200, 0},
        {2, 3, 3, 300, 0},
        {3, 4, 4, 400, 0}
    };
    g.add_edges(batch);
    
    EXPECT_EQ(g.edge_count(), 4);
    EXPECT_EQ(g.min_timestamp(), 100);
    EXPECT_EQ(g.max_timestamp(), 400);
}

TEST(TemporalGraphTest, AddEdgesUpdatesTimestampRange) {
    TemporalGraph g(3);
    g.add_edge(0, 1, 500);
    g.add_edge(1, 2, 100);
    g.add_edge(0, 2, 1000);
    
    EXPECT_EQ(g.min_timestamp(), 100);
    EXPECT_EQ(g.max_timestamp(), 1000);
}

// =============================================================================
// sort_by_time
// =============================================================================

TEST(TemporalGraphTest, SortByTime) {
    TemporalGraph g(4);
    g.add_edge(0, 1, 300);
    g.add_edge(1, 2, 100);
    g.add_edge(2, 3, 200);
    
    g.sort_by_time();
    
    const auto& edges = g.edges();
    EXPECT_EQ(edges[0].timestamp, 100);
    EXPECT_EQ(edges[1].timestamp, 200);
    EXPECT_EQ(edges[2].timestamp, 300);
}

TEST(TemporalGraphTest, SortByTimeAlreadySorted) {
    TemporalGraph g(3);
    g.add_edge(0, 1, 10);
    g.add_edge(1, 2, 20);
    g.add_edge(0, 2, 30);
    
    g.sort_by_time();
    
    const auto& edges = g.edges();
    EXPECT_EQ(edges[0].timestamp, 10);
    EXPECT_EQ(edges[1].timestamp, 20);
    EXPECT_EQ(edges[2].timestamp, 30);
}

TEST(TemporalGraphTest, SortByTimeEmpty) {
    TemporalGraph g(5);
    g.sort_by_time();
    EXPECT_EQ(g.edge_count(), 0);
}

// =============================================================================
// snapshot_at and snapshot_window
// =============================================================================

TEST(TemporalGraphTest, SnapshotAtInstantaneous) {
    TemporalGraph g(4, true);
    g.add_edge(0, 1, 100, 1, 0);
    g.add_edge(1, 2, 100, 1, 0);
    g.add_edge(2, 3, 200, 1, 0);
    
    Graph snap = g.snapshot_at(100);
    EXPECT_EQ(snap.vertex_count(), 4);
    
    auto edges_0 = snap.get_edges(0);
    auto edges_1 = snap.get_edges(1);
    auto edges_2 = snap.get_edges(2);
    
    bool has_0_1 = false, has_1_2 = false;
    for (const auto& e : edges_0) if (e.to == 1) has_0_1 = true;
    for (const auto& e : edges_1) if (e.to == 2) has_1_2 = true;
    
    EXPECT_TRUE(has_0_1);
    EXPECT_TRUE(has_1_2);
}

TEST(TemporalGraphTest, SnapshotAtWithDuration) {
    TemporalGraph g(3, true);
    g.add_edge(0, 1, 100, 1, 50);
    g.add_edge(1, 2, 200, 1, 0);
    
    Graph snap = g.snapshot_at(130);
    auto edges_0 = snap.get_edges(0);
    
    bool has_0_1 = false;
    for (const auto& e : edges_0) if (e.to == 1) has_0_1 = true;
    EXPECT_TRUE(has_0_1);
    
    Graph snap2 = g.snapshot_at(160);
    auto edges2_0 = snap2.get_edges(0);
    bool has_0_1_after = false;
    for (const auto& e : edges2_0) if (e.to == 1) has_0_1_after = true;
    EXPECT_FALSE(has_0_1_after);
}

TEST(TemporalGraphTest, SnapshotWindow) {
    TemporalGraph g(5, true);
    g.add_edge(0, 1, 100);
    g.add_edge(1, 2, 150);
    g.add_edge(2, 3, 200);
    g.add_edge(3, 4, 300);
    
    Graph snap = g.snapshot_window(100, 200);
    
    auto e0 = snap.get_edges(0);
    auto e1 = snap.get_edges(1);
    auto e2 = snap.get_edges(2);
    auto e3 = snap.get_edges(3);
    
    bool has_0_1 = std::any_of(e0.begin(), e0.end(), [](const auto& e) { return e.to == 1; });
    bool has_1_2 = std::any_of(e1.begin(), e1.end(), [](const auto& e) { return e.to == 2; });
    bool has_2_3 = std::any_of(e2.begin(), e2.end(), [](const auto& e) { return e.to == 3; });
    bool has_3_4 = std::any_of(e3.begin(), e3.end(), [](const auto& e) { return e.to == 4; });
    
    EXPECT_TRUE(has_0_1);
    EXPECT_TRUE(has_1_2);
    EXPECT_TRUE(has_2_3);
    EXPECT_FALSE(has_3_4);
}

TEST(TemporalGraphTest, SnapshotWindowEmpty) {
    TemporalGraph g(3, true);
    g.add_edge(0, 1, 100);
    g.add_edge(1, 2, 200);
    
    Graph snap = g.snapshot_window(300, 400);
    EXPECT_EQ(snap.vertex_count(), 3);
    
    int total_edges = 0;
    for (int i = 0; i < 3; ++i) {
        total_edges += static_cast<int>(snap.get_edges(i).size());
    }
    EXPECT_EQ(total_edges, 0);
}

TEST(TemporalGraphTest, EdgesInWindow) {
    TemporalGraph g(4);
    g.add_edge(0, 1, 100);
    g.add_edge(1, 2, 150);
    g.add_edge(2, 3, 200);
    g.add_edge(0, 3, 250);
    
    auto edges = g.edges_in_window(100, 200);
    EXPECT_EQ(edges.size(), 3);
    
    auto edges2 = g.edges_in_window(151, 249);
    EXPECT_EQ(edges2.size(), 1);
    EXPECT_EQ(edges2[0].timestamp, 200);
}

// =============================================================================
// earliest_arrival
// =============================================================================

TEST(TemporalGraphTest, EarliestArrivalSimplePath) {
    TemporalGraph g(4, true);
    g.add_edge(0, 1, 10);
    g.add_edge(1, 2, 20);
    g.add_edge(2, 3, 30);
    
    auto arrival = g.earliest_arrival(0, 0);
    
    EXPECT_EQ(arrival[0], 0);
    EXPECT_EQ(arrival[1], 10);
    EXPECT_EQ(arrival[2], 20);
    EXPECT_EQ(arrival[3], 30);
}

TEST(TemporalGraphTest, EarliestArrivalWithStartTime) {
    TemporalGraph g(3, true);
    g.add_edge(0, 1, 10);
    g.add_edge(0, 1, 50);
    g.add_edge(1, 2, 60);
    
    auto arrival = g.earliest_arrival(0, 40);
    
    EXPECT_EQ(arrival[0], 40);
    EXPECT_EQ(arrival[1], 50);
    EXPECT_EQ(arrival[2], 60);
}

TEST(TemporalGraphTest, EarliestArrivalUnreachable) {
    TemporalGraph g(4, true);
    g.add_edge(0, 1, 100);
    g.add_edge(2, 3, 200);
    
    auto arrival = g.earliest_arrival(0, 0);
    
    EXPECT_EQ(arrival[1], 100);
    EXPECT_EQ(arrival[2], -1);
    EXPECT_EQ(arrival[3], -1);
}

TEST(TemporalGraphTest, EarliestArrivalTemporalOrder) {
    TemporalGraph g(3, true);
    g.add_edge(0, 1, 100);
    g.add_edge(1, 2, 50);
    
    auto arrival = g.earliest_arrival(0, 0);
    
    EXPECT_EQ(arrival[1], 100);
    EXPECT_EQ(arrival[2], -1);
}

TEST(TemporalGraphTest, EarliestArrivalMultiplePaths) {
    TemporalGraph g(4, true);
    g.add_edge(0, 1, 10);
    g.add_edge(0, 2, 20);
    g.add_edge(1, 3, 50);
    g.add_edge(2, 3, 30);
    
    auto arrival = g.earliest_arrival(0, 0);
    
    EXPECT_EQ(arrival[3], 30);
}

// =============================================================================
// fastest_path
// =============================================================================

TEST(TemporalGraphTest, FastestPathSimple) {
    TemporalGraph g(3, true);
    g.add_edge(0, 1, 10);
    g.add_edge(1, 2, 20);
    
    auto [travel_time, arrival] = g.fastest_path(0, 2, 0);
    
    EXPECT_EQ(arrival, 20);
    EXPECT_EQ(travel_time, 20);
}

TEST(TemporalGraphTest, FastestPathNoPath) {
    TemporalGraph g(3, true);
    g.add_edge(0, 1, 100);
    
    auto [travel_time, arrival] = g.fastest_path(0, 2, 0);
    
    EXPECT_EQ(travel_time, -1);
    EXPECT_EQ(arrival, -1);
}

TEST(TemporalGraphTest, FastestPathSameVertex) {
    TemporalGraph g(3, true);
    g.add_edge(0, 1, 10);
    
    auto [travel_time, arrival] = g.fastest_path(0, 0, 5);
    
    EXPECT_EQ(travel_time, 0);
    EXPECT_EQ(arrival, 5);
}

TEST(TemporalGraphTest, FastestPathWithWaiting) {
    TemporalGraph g(3, true);
    g.add_edge(0, 1, 10);
    g.add_edge(1, 2, 100);
    
    auto [travel_time, arrival] = g.fastest_path(0, 2, 0);
    
    EXPECT_EQ(arrival, 100);
    EXPECT_EQ(travel_time, 100);
}

// =============================================================================
// temporal_reachability
// =============================================================================

TEST(TemporalGraphTest, TemporalReachabilitySimple) {
    TemporalGraph g(4, true);
    g.add_edge(0, 1, 10);
    g.add_edge(1, 2, 20);
    g.add_edge(1, 3, 30);
    
    auto reachable = g.temporal_reachability(0, 0);
    
    EXPECT_TRUE(reachable[0]);
    EXPECT_TRUE(reachable[1]);
    EXPECT_TRUE(reachable[2]);
    EXPECT_TRUE(reachable[3]);
}

TEST(TemporalGraphTest, TemporalReachabilityPartial) {
    TemporalGraph g(4, true);
    g.add_edge(0, 1, 100);
    g.add_edge(2, 3, 200);
    
    auto reachable = g.temporal_reachability(0, 0);
    
    EXPECT_TRUE(reachable[0]);
    EXPECT_TRUE(reachable[1]);
    EXPECT_FALSE(reachable[2]);
    EXPECT_FALSE(reachable[3]);
}

TEST(TemporalGraphTest, TemporalReachabilityRespectTime) {
    TemporalGraph g(3, true);
    g.add_edge(0, 1, 100);
    g.add_edge(1, 2, 50);
    
    auto reachable = g.temporal_reachability(0, 0);
    
    EXPECT_TRUE(reachable[0]);
    EXPECT_TRUE(reachable[1]);
    EXPECT_FALSE(reachable[2]);
}

TEST(TemporalGraphTest, TemporalReachabilityWithStartTime) {
    TemporalGraph g(3, true);
    g.add_edge(0, 1, 50);
    g.add_edge(0, 1, 150);
    g.add_edge(1, 2, 200);
    
    auto reachable = g.temporal_reachability(0, 100);
    
    EXPECT_TRUE(reachable[0]);
    EXPECT_TRUE(reachable[1]);
    EXPECT_TRUE(reachable[2]);
}

// =============================================================================
// sliding_windows
// =============================================================================

TEST(TemporalGraphTest, SlidingWindowsBasic) {
    TemporalGraph g(4, true);
    g.add_edge(0, 1, 10);
    g.add_edge(1, 2, 50);
    g.add_edge(2, 3, 110);
    
    auto windows = g.sliding_windows(100);
    
    EXPECT_GE(windows.size(), 1);
    for (const auto& w : windows) {
        EXPECT_EQ(w.vertex_count(), 4);
    }
}

TEST(TemporalGraphTest, SlidingWindowsEmpty) {
    TemporalGraph g(3);
    auto windows = g.sliding_windows(50);
    EXPECT_EQ(windows.size(), 0);
}

TEST(TemporalGraphTest, SlidingWindowsAllInOne) {
    TemporalGraph g(3, true);
    g.add_edge(0, 1, 10);
    g.add_edge(1, 2, 20);
    g.add_edge(0, 2, 30);
    
    auto windows = g.sliding_windows(1000);
    
    EXPECT_GE(windows.size(), 1);
    
    if (!windows.empty()) {
        int total_edges = 0;
        for (int i = 0; i < 3; ++i) {
            total_edges += static_cast<int>(windows[0].get_edges(i).size());
        }
        EXPECT_EQ(total_edges, 3);
    }
}

TEST(TemporalGraphTest, SlidingWindowsSmallWindow) {
    TemporalGraph g(4, true);
    g.add_edge(0, 1, 0);
    g.add_edge(1, 2, 100);
    g.add_edge(2, 3, 200);
    
    auto windows = g.sliding_windows(50);
    
    EXPECT_GE(windows.size(), 3);
}

// =============================================================================
// aggregate
// =============================================================================

TEST(TemporalGraphTest, AggregateSum) {
    TemporalGraph g(3, true);
    g.add_edge(0, 1, 10, 5);
    g.add_edge(0, 1, 20, 3);
    g.add_edge(1, 2, 30, 7);
    
    Graph agg = g.aggregate("sum");
    EXPECT_EQ(agg.vertex_count(), 3);
    
    auto edges_0 = agg.get_edges(0);
    bool found = false;
    for (const auto& e : edges_0) {
        if (e.to == 1) {
            EXPECT_EQ(e.weight, 8);
            found = true;
        }
    }
    EXPECT_TRUE(found);
}

TEST(TemporalGraphTest, AggregateCount) {
    TemporalGraph g(3, true);
    g.add_edge(0, 1, 10, 5);
    g.add_edge(0, 1, 20, 3);
    g.add_edge(0, 1, 30, 2);
    
    Graph agg = g.aggregate("count");
    
    auto edges_0 = agg.get_edges(0);
    bool found = false;
    for (const auto& e : edges_0) {
        if (e.to == 1) {
            EXPECT_EQ(e.weight, 3);
            found = true;
        }
    }
    EXPECT_TRUE(found);
}

// =============================================================================
// temporal_scc
// =============================================================================

TEST(TemporalGraphTest, TemporalSCCSimple) {
    TemporalGraph g(3, true);
    g.add_edge(0, 1, 10);
    g.add_edge(1, 2, 20);
    g.add_edge(2, 0, 30);
    
    std::vector<int> component;
    int num_scc = temporal_scc(g, component);
    
    EXPECT_GE(num_scc, 1);
    EXPECT_EQ(component.size(), 3);
}

TEST(TemporalGraphTest, TemporalSCCDisconnected) {
    TemporalGraph g(4, true);
    g.add_edge(0, 1, 10);
    g.add_edge(2, 3, 20);
    
    std::vector<int> component;
    int num_scc = temporal_scc(g, component);
    
    EXPECT_GE(num_scc, 2);
}

// =============================================================================
// Edge cases
// =============================================================================

TEST(TemporalGraphTest, SingleVertex) {
    TemporalGraph g(1);
    EXPECT_EQ(g.vertex_count(), 1);
    EXPECT_EQ(g.edge_count(), 0);
    
    auto arrival = g.earliest_arrival(0, 0);
    EXPECT_EQ(arrival[0], 0);
    
    auto reachable = g.temporal_reachability(0, 0);
    EXPECT_TRUE(reachable[0]);
}

TEST(TemporalGraphTest, SelfLoop) {
    TemporalGraph g(2, true);
    g.add_edge(0, 0, 100);
    g.add_edge(0, 1, 200);
    
    EXPECT_EQ(g.edge_count(), 2);
    
    auto arrival = g.earliest_arrival(0, 0);
    EXPECT_EQ(arrival[1], 200);
}

TEST(TemporalGraphTest, LargeTimestamps) {
    TemporalGraph g(2);
    long long big_time = 1000000000000LL;
    g.add_edge(0, 1, big_time);
    
    EXPECT_EQ(g.min_timestamp(), big_time);
    EXPECT_EQ(g.max_timestamp(), big_time);
}

TEST(TemporalGraphTest, NegativeTimestamps) {
    TemporalGraph g(3);
    g.add_edge(0, 1, -100);
    g.add_edge(1, 2, 0);
    g.add_edge(0, 2, 100);
    
    EXPECT_EQ(g.min_timestamp(), -100);
    EXPECT_EQ(g.max_timestamp(), 100);
}
