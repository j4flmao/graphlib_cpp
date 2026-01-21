#include <gtest/gtest.h>
#include "graphlib/connectivity.h"
#include "graphlib/shortest_path.h"
#include <vector>
#include <algorithm>

using namespace graphlib;

TEST(ConnectivityExtended, BridgesAndArticulationPoints) {
    // 0-1-2-3
    //   |
    //   4
    Connectivity g(5);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(1, 4);
    
    std::vector<std::pair<int, int>> br;
    g.bridges(br);
    // All edges are bridges in a tree
    EXPECT_EQ(br.size(), 4);
    
    std::vector<int> ap;
    g.articulation_points(ap);
    // 1 is AP (removes 0, 4, 2-3) -> actually splits into {0}, {4}, {2,3}
    // 2 is AP (removes 3) -> splits {3} from {0,1,4}
    // 0, 3, 4 are leaves, not AP.
    
    // Sort AP for comparison
    std::sort(ap.begin(), ap.end());
    EXPECT_EQ(ap.size(), 2);
    EXPECT_EQ(ap[0], 1);
    EXPECT_EQ(ap[1], 2);
}

TEST(ConnectivityExtended, BiconnectedComponents) {
    // 0-1-2-0 (cycle) - 3
    Connectivity g(4);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);
    g.add_edge(2, 3);
    
    std::vector<std::vector<int>> bccs;
    g.biconnected_components(bccs);
    
    // Should have 2 BCCs: {0, 1, 2} and {2, 3}
    // Note: BCCs are sets of edges usually, but if vertices, 2 is in both.
    // The implementation likely returns sets of vertices or edges. 
    // Header says vector<vector<int>>, likely vertices.
    
    EXPECT_EQ(bccs.size(), 2);
    
    bool has_cycle = false;
    bool has_edge = false;
    
    for(const auto& comp : bccs) {
        if(comp.size() == 3) has_cycle = true; // 0,1,2
        if(comp.size() == 2) has_edge = true;  // 2,3
    }
    
    EXPECT_TRUE(has_cycle);
    EXPECT_TRUE(has_edge);
}

TEST(GraphModification, ShortestPathUpdate) {
    ShortestPath sp(3);
    sp.add_edge(0, 1, 10);
    sp.add_edge(1, 2, 10);
    
    auto dist1 = sp.dijkstra(0, 1e18);
    EXPECT_EQ(dist1[2], 20);
    
    // Add shortcut
    sp.add_edge(0, 2, 5);
    auto dist2 = sp.dijkstra(0, 1e18);
    EXPECT_EQ(dist2[2], 5);
}

TEST(GraphModification, ConnectivityMerge) {
    Connectivity g(4);
    // 0-1   2-3
    g.add_edge(0, 1);
    g.add_edge(2, 3);
    
    std::vector<int> comp;
    int count1 = g.connected_components(comp);
    EXPECT_EQ(count1, 2);
    
    // Merge
    g.add_edge(1, 2);
    int count2 = g.connected_components(comp);
    EXPECT_EQ(count2, 1);
}
