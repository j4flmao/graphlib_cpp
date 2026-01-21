#include <gtest/gtest.h>
#include <graphlib/shortest_path.h>
#include <vector>
#include <limits>

using namespace graphlib;

class ShortestPathExtraTest : public ::testing::Test {
protected:
    const long long INF = 1e18;
    void SetUp() override {
    }
};

TEST_F(ShortestPathExtraTest, BellmanFordNegativeCycle) {
    int n = 3;
    ShortestPath sp(n);
    // 0 -> 1 (1)
    // 1 -> 2 (-5)
    // 2 -> 0 (2)
    // Cycle: 0->1->2->0 weight: 1 - 5 + 2 = -2
    sp.add_edge(0, 1, 1);
    sp.add_edge(1, 2, -5);
    sp.add_edge(2, 0, 2);

    bool has_cycle = false;
    sp.bellman_ford(0, INF, has_cycle);
    EXPECT_TRUE(has_cycle);
}

TEST_F(ShortestPathExtraTest, BellmanFordNoCycle) {
    int n = 3;
    ShortestPath sp(n);
    // 0 -> 1 (1)
    // 1 -> 2 (-2)
    // 2 -> 0 (5)
    // Cycle: 0->1->2->0 weight: 1 - 2 + 5 = 4 > 0
    sp.add_edge(0, 1, 1);
    sp.add_edge(1, 2, -2);
    sp.add_edge(2, 0, 5);

    bool has_cycle = false;
    auto dist = sp.bellman_ford(0, INF, has_cycle);
    EXPECT_FALSE(has_cycle);
    EXPECT_EQ(dist[0], 0);
    EXPECT_EQ(dist[1], 1);
    EXPECT_EQ(dist[2], -1);
}

TEST_F(ShortestPathExtraTest, FloydWarshallSimple) {
    int n = 4;
    ShortestPath sp(n);
    sp.add_edge(0, 1, 5);
    sp.add_edge(1, 2, 3);
    sp.add_edge(2, 3, 1);
    sp.add_edge(0, 3, 10);
    
    // 0 -> 3 direct is 10
    // 0 -> 1 -> 2 -> 3 is 5+3+1 = 9
    
    auto dist = sp.floyd_warshall(INF);
    
    EXPECT_EQ(dist[0][3], 9);
    EXPECT_EQ(dist[0][1], 5);
    EXPECT_EQ(dist[0][2], 8);
    EXPECT_EQ(dist[1][3], 4);
}

TEST_F(ShortestPathExtraTest, FloydWarshallDisconnect) {
    int n = 2;
    ShortestPath sp(n);
    auto dist = sp.floyd_warshall(INF);
    EXPECT_GE(dist[0][1], INF/2);
}

TEST_F(ShortestPathExtraTest, JohnsonNegativeEdges) {
    int n = 3;
    ShortestPath sp(n);
    sp.add_edge(0, 1, 1);
    sp.add_edge(1, 2, -2);
    // 0->2 cost -1
    
    bool has_cycle = false;
    auto dist = sp.johnson(INF, has_cycle);
    
    EXPECT_FALSE(has_cycle);
    EXPECT_EQ(dist[0][2], -1);
}

TEST_F(ShortestPathExtraTest, AStarSimple) {
    int n = 5;
    ShortestPath sp(n);
    sp.add_edge(0, 1, 1);
    sp.add_edge(1, 2, 1);
    sp.add_edge(2, 3, 1);
    sp.add_edge(3, 4, 1);
    
    // Heuristic: dist to 4.
    // real dists: 4, 3, 2, 1, 0
    // admissible heuristic: h(u) <= real(u, target)
    std::vector<long long> h = {4, 3, 2, 1, 0};
    
    auto dist = sp.a_star(0, 4, h, INF);
    EXPECT_EQ(dist[4], 4);
}

TEST_F(ShortestPathExtraTest, MultiSourceDijkstra) {
    int n = 6;
    ShortestPath sp(n);
    // Sources: 0, 1
    // 0 -> 2 (10)
    // 1 -> 3 (5)
    // 2 -> 4 (2)
    // 3 -> 4 (2)
    
    sp.add_edge(0, 2, 10);
    sp.add_edge(1, 3, 5);
    sp.add_edge(2, 4, 2);
    sp.add_edge(3, 4, 2);
    
    std::vector<int> sources = {0, 1};
    auto dist = sp.multi_source_dijkstra(sources, INF);
    
    // dist[4]: min(dist(0,4), dist(1,4)) = min(12, 7) = 7
    EXPECT_EQ(dist[4], 7);
    EXPECT_EQ(dist[2], 10);
    EXPECT_EQ(dist[3], 5);
}
