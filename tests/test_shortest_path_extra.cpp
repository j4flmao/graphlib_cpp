#include <gtest/gtest.h>
#include "graphlib/shortest_path.h"
#include <vector>

using namespace graphlib;

TEST(YenKShortestPathsTest, SimpleGraph) {
    ShortestPath sp(6);
    // 0 -> 1 -> 3 -> 5 (cost 10+10+10=30)
    // 0 -> 2 -> 3 -> 5 (cost 5+20+10=35)
    // 0 -> 2 -> 4 -> 5 (cost 5+5+30=40)
    
    sp.add_edge(0, 1, 10);
    sp.add_edge(1, 3, 10);
    sp.add_edge(3, 5, 10);
    
    sp.add_edge(0, 2, 5);
    sp.add_edge(2, 3, 20);
    sp.add_edge(2, 4, 5);
    sp.add_edge(4, 5, 30);
    
    // Extra edge to make more paths
    sp.add_edge(1, 2, 2); 
    // New path: 0->1->2->3->5 (10+2+20+10 = 42)
    // New path: 0->1->2->4->5 (10+2+5+30 = 47)
    
    // K=3
    auto paths = sp.k_shortest_paths(0, 5, 3, 1000000);
    
    ASSERT_EQ(paths.size(), 3);
    
    // Path 1: 0->1->3->5 (30)
    std::vector<int> p1 = {0, 1, 3, 5};
    EXPECT_EQ(paths[0], p1);
    
    // Path 2: 0->2->3->5 (35)
    std::vector<int> p2 = {0, 2, 3, 5};
    EXPECT_EQ(paths[1], p2);
    
    // Path 3: 0->2->4->5 (40)
    std::vector<int> p3 = {0, 2, 4, 5};
    EXPECT_EQ(paths[2], p3);
}

TEST(YenKShortestPathsTest, NotEnoughPaths) {
    ShortestPath sp(3);
    sp.add_edge(0, 1, 10);
    sp.add_edge(1, 2, 10);
    
    // Only 1 path: 0->1->2
    auto paths = sp.k_shortest_paths(0, 2, 5, 1000000);
    
    ASSERT_EQ(paths.size(), 1);
    std::vector<int> p1 = {0, 1, 2};
    EXPECT_EQ(paths[0], p1);
}

TEST(YenKShortestPathsTest, Disconnected) {
    ShortestPath sp(3);
    sp.add_edge(0, 1, 10);
    
    auto paths = sp.k_shortest_paths(0, 2, 3, 1000000);
    EXPECT_EQ(paths.size(), 0);
}

TEST(YenKShortestPathsTest, ParallelEdges) {
    ShortestPath sp(2);
    sp.add_edge(0, 1, 10);
    sp.add_edge(0, 1, 20);
    sp.add_edge(0, 1, 5);
    
    auto paths = sp.k_shortest_paths(0, 1, 3, 1000000);
    
    // Standard Yen's implementation (vertex based) might not distinguish parallel edges unless edges are explicit in path.
    // My implementation returns vector<int> (vertices).
    // So it will see 0->1 as ONE path, regardless of which edge is used.
    // The loop in k_shortest_paths checks `existing == p.path`.
    // Since path is vector of vertices, [0, 1] == [0, 1].
    // So it will distinct paths based on vertices.
    // Thus, parallel edges with same vertices will be collapsed into ONE path in the output.
    
    ASSERT_EQ(paths.size(), 1);
    EXPECT_EQ(paths[0], (std::vector<int>{0, 1}));
}
