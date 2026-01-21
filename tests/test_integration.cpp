#include <gtest/gtest.h>
#include <graphlib/graphlib.h>
#include <graphlib/dag.h>
#include <graphlib/shortest_path.h>
#include <graphlib/mst.h>
#include <graphlib/scc.h>
#include <vector>
#include <algorithm>

using namespace graphlib;

TEST(Integration, SCC_Condensation_DAG_ShortestPath) {
    // 0->1, 1->0 (SCC 1: {0,1})
    // 2->3, 3->2 (SCC 2: {2,3})
    // 1->2 (Edge between SCC 1 and SCC 2)
    // 2->4 (SCC 3: {4})
    // 4->5 (SCC 4: {5})
    
    // Condensation: {0,1} -> {2,3} -> {4} -> {5}
    // Path lengths in condensation graph (unweighted usually, or we can assume weights).
    // The library function build_scc_condensation_dag likely produces a DAG where edges exist if there is an edge between components.
    
    SCC scc(6);
    scc.add_edge(0, 1); scc.add_edge(1, 0);
    scc.add_edge(2, 3); scc.add_edge(3, 2);
    scc.add_edge(1, 2);
    scc.add_edge(2, 4);
    scc.add_edge(4, 5);
    
    std::vector<int> components;
    int count = scc.tarjan(components);
    
    EXPECT_EQ(count, 4);
    
    // Check components
    EXPECT_EQ(components[0], components[1]);
    EXPECT_EQ(components[2], components[3]);
    EXPECT_NE(components[0], components[2]);
    EXPECT_NE(components[2], components[4]);
    
    DAG condensation = build_scc_condensation_dag(scc, components, count);
    
    // Verify condensation is a DAG
    bool has_cycle = false;
    condensation.topological_sort_kahn(has_cycle);
    EXPECT_FALSE(has_cycle);
    
    // Check reachability in condensation
    // Component of 0 should reach component of 5.
    int c0 = components[0];
    int c5 = components[5];
    
    // BFS in condensation
    auto dists = bfs_distances(condensation, c0);
    EXPECT_NE(dists[c5], -1);
}

TEST(Integration, MST_vs_ShortestPath) {
    // In general, path in MST is NOT the shortest path in the graph.
    // But it is the "Minimax" path (path minimizing the maximum edge weight).
    // Let's verify this property.
    
    int n = 4;
    std::vector<MstEdge> edges;
    // 0-1 (10)
    // 1-2 (10)
    // 0-2 (15)
    // 2-3 (5)
    
    edges.push_back({0, 1, 10});
    edges.push_back({1, 2, 10});
    edges.push_back({0, 2, 15});
    edges.push_back({2, 3, 5});
    
    long long weight = MST::kruskal(n, edges);
    // MST should use 0-1 (10), 1-2 (10), 2-3 (5). Total 25.
    // 0-2 (15) is skipped because 0-1-2 costs max(10,10)=10 < 15.
    EXPECT_EQ(weight, 25);
    
    // Shortest Path 0->2 is 15 (direct edge) vs 20 (via 1).
    ShortestPath sp(n);
    sp.add_edge(0, 1, 10); sp.add_edge(1, 0, 10);
    sp.add_edge(1, 2, 10); sp.add_edge(2, 1, 10);
    sp.add_edge(0, 2, 15); sp.add_edge(2, 0, 15);
    sp.add_edge(2, 3, 5);  sp.add_edge(3, 2, 5);
    
    auto dist = sp.dijkstra(0, 1e18);
    EXPECT_EQ(dist[2], 15); 
    
    // So MST path distance (20) != Shortest Path distance (15).
    // This confirms they are different concepts, ensuring our libraries don't conflate them.
}

TEST(Integration, DAG_LongestPath_Scheduling) {
    // Standard CPM task
    // A(3) -> C(2)
    // B(2) -> C
    // C -> D(5)
    // Start -> A, B
    // Weights are durations of nodes, but we usually put them on edges for DAG longest path or convert node weights.
    // Let's assume edge weights represent "time to traverse" or "task duration of source".
    
    // 0(Start) -> 1(A) weight 0
    // 0(Start) -> 2(B) weight 0
    // 1(A) -> 3(C) weight 3 (duration of A)
    // 2(B) -> 3(C) weight 2 (duration of B)
    // 3(C) -> 4(D) weight 2 (duration of C)
    // 4(D) -> 5(End) weight 5 (duration of D)
    
    DAG dag(6);
    dag.add_edge(0, 1, 0);
    dag.add_edge(0, 2, 0);
    dag.add_edge(1, 3, 3);
    dag.add_edge(2, 3, 2);
    dag.add_edge(3, 4, 2);
    dag.add_edge(4, 5, 5);
    
    auto dists = dag.longest_path(0, -1);
    
    // Path 0->1->3->4->5: 0+3+2+5 = 10
    // Path 0->2->3->4->5: 0+2+2+5 = 9
    // Max is 10.
    EXPECT_EQ(dists[5], 10);
}
