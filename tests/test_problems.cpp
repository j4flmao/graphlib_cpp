#include <gtest/gtest.h>
#include "graphlib/graph_core.h"
#include "graphlib/shortest_path.h"
#include "graphlib/dag.h"
#include "graphlib/connectivity.h"
#include "graphlib/max_flow.h"
#include <vector>
#include <string>
#include <algorithm>

using namespace graphlib;

// Scenario 1: Maze Solving (Grid to Graph)
// S . #
// . # .
// . . E
// Start (0,0), End (2,2). # is wall.
TEST(ProblemSolving, MazeShortestPath) {
    int rows = 3;
    int cols = 3;
    auto node = [&](int r, int c) { return r * cols + c; };
    
    // Grid structure: 9 nodes
    Graph g(9, false); // undirected
    
    // Grid map: 0=path, 1=wall
    int grid[3][3] = {
        {0, 0, 1},
        {0, 1, 0},
        {0, 0, 0}
    };
    
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};
    
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (grid[r][c] == 1) continue;
            for (int i = 0; i < 4; ++i) {
                int nr = r + dr[i];
                int nc = c + dc[i];
                if (nr >= 0 && nr < rows && nc >= 0 && nc < cols && grid[nr][nc] == 0) {
                    g.add_edge(node(r, c), node(nr, nc));
                }
            }
        }
    }
    
    int start = node(0, 0);
    int end = node(2, 2);
    
    std::vector<int> dist = bfs_distances(g, start);
    
    // Path: (0,0)->(1,0)->(2,0)->(2,1)->(2,2) length 4
    EXPECT_EQ(dist[end], 4);
}

// Scenario 2: Project Dependencies (Topological Sort)
// Tasks 0..5.
// 0->1, 0->2, 1->3, 2->3, 3->4, 4->5
TEST(ProblemSolving, TaskScheduling) {
    DAG g(6);
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(1, 3);
    g.add_edge(2, 3);
    g.add_edge(3, 4);
    g.add_edge(4, 5);
    
    bool has_cycle = false;
    std::vector<int> order = g.topological_sort_kahn(has_cycle);
    EXPECT_FALSE(has_cycle);
    
    // Check validity
    std::vector<int> pos(6);
    for (int i = 0; i < 6; ++i) pos[order[i]] = i;
    
    EXPECT_LT(pos[0], pos[1]);
    EXPECT_LT(pos[0], pos[2]);
    EXPECT_LT(pos[1], pos[3]);
    EXPECT_LT(pos[2], pos[3]);
    EXPECT_LT(pos[3], pos[4]);
    EXPECT_LT(pos[4], pos[5]);
}

// Scenario 3: Critical Network Infrastructure (Articulation Points)
// 0-1-2-3-4
//   |
//   5
// 1 and 2 should be articulation points?
// Wait, removing 1 disconnects 0 and 5 from rest.
// Removing 2 disconnects (0,1,5) from (3,4).
TEST(ProblemSolving, CriticalInfrastructure) {
    Connectivity con(6);
    con.add_edge(0, 1);
    con.add_edge(1, 2);
    con.add_edge(2, 3);
    con.add_edge(3, 4);
    con.add_edge(1, 5);
    
    std::vector<int> aps;
    con.articulation_points(aps);
    std::sort(aps.begin(), aps.end());
    
    // Expected: 1, 2, 3
    // Removing 1 -> {0}, {5}, {2,3,4} disconnected
    // Removing 2 -> {0,1,5}, {3,4} disconnected
    // Removing 3 -> {0,1,5,2}, {4} disconnected
    
    EXPECT_EQ(aps.size(), 3);
    EXPECT_TRUE(std::binary_search(aps.begin(), aps.end(), 1));
    EXPECT_TRUE(std::binary_search(aps.begin(), aps.end(), 2));
    EXPECT_TRUE(std::binary_search(aps.begin(), aps.end(), 3));
}

// Scenario 4: Traffic Routing (Max Flow)
// Multiple routes with capacities
TEST(ProblemSolving, TrafficBottleneck) {
    // 0(Source) -> 1(CityA) cap 10
    // 0 -> 2(CityB) cap 10
    // 1 -> 3(Dest) cap 5
    // 2 -> 3(Dest) cap 15
    // 1 -> 2 cap 5 (Bridge A->B)
    
    MaxFlow g(4);
    g.add_edge(0, 1, 10);
    g.add_edge(0, 2, 10);
    g.add_edge(1, 3, 5);
    g.add_edge(2, 3, 15);
    g.add_edge(1, 2, 5);
    
    // Path 0->1->3: flow 5 (bottleneck 1->3)
    // Path 0->2->3: flow 10 (bottleneck 0->2)
    // Path 0->1->2->3: flow 5 (remaining 1->2 capacity and 2->3 capacity)
    // Total should be 20?
    // 0->1: 10 total. 5 goes to 3 directly. 5 goes to 2.
    // 0->2: 10 total.
    // At 2: 10 (from 0) + 5 (from 1) = 15.
    // 2->3: Cap 15. All 15 can pass.
    // Total flow = 5 (1->3) + 15 (2->3) = 20.
    
    EXPECT_EQ(g.dinic(0, 3), 20);
}

// Scenario 5: Shortest Path with Turn Penalties (Modeled as Node Splitting or just weights)
// Simplified: Just verifying complex weights handling
TEST(ProblemSolving, NegativeWeightsCycle) {
    // 0 -> 1 (1)
    // 1 -> 2 (1)
    // 2 -> 0 (-3) -> Net cycle -1
    // Bellman Ford should detect negative cycle
    
    ShortestPath sp(3);
    sp.add_edge(0, 1, 1);
    sp.add_edge(1, 2, 1);
    sp.add_edge(2, 0, -3);
    
    bool has_cycle = false;
    long long inf = std::numeric_limits<long long>::max();
    std::vector<long long> dist = sp.bellman_ford(0, inf, has_cycle);
    
    EXPECT_TRUE(has_cycle);
}
