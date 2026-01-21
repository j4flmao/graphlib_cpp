#include <gtest/gtest.h>
#include <graphlib/mst.h>
#include <vector>

using namespace graphlib;

class MSTTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
};

TEST_F(MSTTest, KruskalSimple) {
    int n = 4;
    std::vector<MstEdge> edges = {
        {0, 1, 10},
        {0, 2, 6},
        {0, 3, 5},
        {1, 3, 15},
        {2, 3, 4}
    };
    
    // Expected MST: (2,3,4), (0,3,5), (0,1,10) is wrong.
    // Sorted: (2,3,4), (0,3,5), (0,2,6), (0,1,10), (1,3,15)
    // 1. (2,3) w=4. Pick. Sets: {2,3}, {0}, {1}
    // 2. (0,3) w=5. Pick. Sets: {0,2,3}, {1}
    // 3. (0,2) w=6. Skip (0,2 already connected via 3).
    // 4. (0,1) w=10. Pick. Sets: {0,1,2,3}.
    // Total weight: 4 + 5 + 10 = 19.
    
    long long cost = MST::kruskal(n, edges);
    EXPECT_EQ(cost, 19);
}

TEST_F(MSTTest, PrimSimple) {
    int n = 4;
    std::vector<std::vector<std::pair<int, long long>>> adj(n);
    auto add_edge = [&](int u, int v, long long w) {
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    };
    
    add_edge(0, 1, 10);
    add_edge(0, 2, 6);
    add_edge(0, 3, 5);
    add_edge(1, 3, 15);
    add_edge(2, 3, 4);
    
    long long inf = 1e18;
    long long cost = MST::prim(n, 0, adj, inf);
    EXPECT_EQ(cost, 19);
}

TEST_F(MSTTest, DisconnectedGraph) {
    int n = 4;
    std::vector<MstEdge> edges = {
        {0, 1, 5},
        {2, 3, 10}
    };
    // Kruskal should return sum of MSTs of components: 5 + 10 = 15
    long long cost = MST::kruskal(n, edges);
    EXPECT_EQ(cost, 15);
}

TEST_F(MSTTest, SingleNode) {
    int n = 1;
    std::vector<MstEdge> edges;
    long long cost = MST::kruskal(n, edges);
    EXPECT_EQ(cost, 0);
}

TEST_F(MSTTest, PrimDisconnected) {
    // Prim starting from 0 will only cover component of 0.
    // The implementation of Prim in typical libraries might only traverse the reachable component.
    // Let's verify behavior. If it only visits component 0, cost is 5.
    int n = 4;
    std::vector<std::vector<std::pair<int, long long>>> adj(n);
    adj[0].push_back({1, 5});
    adj[1].push_back({0, 5});
    adj[2].push_back({3, 10});
    adj[3].push_back({2, 10});
    
    long long inf = 1e18;
    long long cost = MST::prim(n, 0, adj, inf);
    EXPECT_EQ(cost, 5);
}
