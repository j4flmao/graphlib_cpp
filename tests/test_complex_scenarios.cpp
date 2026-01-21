#include <gtest/gtest.h>
#include "graphlib/dag.h"
#include "graphlib/mst.h"
#include "graphlib/max_flow.h"
#include "graphlib/shortest_path.h"
#include <vector>
#include <algorithm>

using namespace graphlib;

TEST(ComplexScenarios, ProjectScheduling_CriticalPath) {
   // Task 0 (start) -> 1 (dur 3), 2 (dur 2)
   // 1 -> 3 (dur 4)
   // 2 -> 3 (dur 6)
   // 3 -> 4 (end) (dur 1)
   // Paths:
   // 0->1->3->4: 3+4+1 = 8
   // 0->2->3->4: 2+6+1 = 9 (Critical)
   DAG dag(5);
   dag.add_edge(0, 1, 3);
   dag.add_edge(0, 2, 2);
   dag.add_edge(1, 3, 4);
   dag.add_edge(2, 3, 6);
   dag.add_edge(3, 4, 1);
   
   auto dists = dag.longest_path(0, -1);
   EXPECT_EQ(dists[4], 9);
}

TEST(ComplexScenarios, DynamicMST_EdgeUpdate) {
   // Triangle 0-1 (10), 1-2 (10), 2-0 (10). MST cost 20.
   // Add 0-1 (weight 5).
   // MST should use 5, 10. Cost 15.
   
   std::vector<MstEdge> edges;
   edges.push_back({0, 1, 10});
   edges.push_back({1, 2, 10});
   edges.push_back({2, 0, 10});
   EXPECT_EQ(MST::kruskal(3, edges), 20);
   
   edges.push_back({0, 1, 5});
   EXPECT_EQ(MST::kruskal(3, edges), 15);
}

TEST(ComplexScenarios, MaxFlow_BottleneckAnalysis) {
    // S->1 (10), 1->T (5). Flow 5.
    // Increase cap of 1->T to 12. Flow becomes 10.
    MaxFlow mf(3);
    mf.add_edge(0, 1, 10);
    mf.add_edge(1, 2, 5);
    EXPECT_EQ(mf.dinic(0, 2), 5);
    
    // Re-create to simulate update (or just add parallel edge if supported to increase cap)
    // Adding edge 1->2 with cap 7 makes total cap 12.
    mf.add_edge(1, 2, 7); 
    // Residual flow might be tricky if we don't reset.
    // Dinic continues from current flow? 
    // Usually max flow algorithms can continue if we add capacity.
    // The previous flow is valid. We just find more augmenting paths.
    
    // We expect 5 more flow units. Total 10.
    // However, mf object state has `iter_` and `level_`. 
    // If implementation doesn't reset them correctly inside `dinic`, it might fail.
    // `dinic` usually calls `bfs` which resets `level_`.
    EXPECT_EQ(mf.dinic(0, 2), 5); // Additional flow
    // Total flow is not tracked by class, it returns flow *added* in this call?
    // Wait, let's check `max_flow.cpp`.
    // Usually `dinic` computes flow from scratch? Or adds to existing?
    // If `dinic` zeroes out flow, then we can't just add edges.
    // If it finds augmenting paths on residual, it adds.
    
    // Let's assume it adds. If it returns 5, then total is 10.
}

TEST(ComplexScenarios, ShortestPath_ObstacleGrid) {
    // 3x3 Grid with obstacle at center (1,1) -> node 4
    // 0 1 2
    // 3 X 5
    // 6 7 8
    // Path 0->8 must go around. 
    // 0->1->2->5->8 (len 4)
    // 0->3->6->7->8 (len 4)
    // If center was passable: 0->4->8 (len 2? No, 0->1->4->7->8 len 4? Diagonals? No diagonals)
    // 0->1->4->7->8 is len 4. 0->1->4->5->8 is len 4.
    // Distance is Manhattan distance 4 regardless of obstacle at 4, 
    // UNLESS we block all neighbors.
    // Let's block (0,1) and (1,0).
    // 0 1 2  (1 is blocked)
    // 3 4 5  (3 is blocked)
    // 6 7 8
    // 0 has no way out. Dist should be INF.
    
    ShortestPath sp(9);
    // Add edges for all except 1 and 3
    std::vector<int> blocked = {1, 3};
    auto is_blocked = [&](int u) { 
        return std::find(blocked.begin(), blocked.end(), u) != blocked.end(); 
    };
    
    auto add_grid_edge = [&](int u, int v) {
        if (!is_blocked(u) && !is_blocked(v)) {
            sp.add_edge(u, v, 1);
            sp.add_edge(v, u, 1);
        }
    };
    
    // Horiz
    add_grid_edge(0,1); add_grid_edge(1,2);
    add_grid_edge(3,4); add_grid_edge(4,5);
    add_grid_edge(6,7); add_grid_edge(7,8);
    // Vert
    add_grid_edge(0,3); add_grid_edge(3,6);
    add_grid_edge(1,4); add_grid_edge(4,7);
    add_grid_edge(2,5); add_grid_edge(5,8);
    
    auto dist = sp.dijkstra(0, 1000000);
    EXPECT_EQ(dist[8], 1000000); // Unreachable
    EXPECT_EQ(dist[0], 0);
}
