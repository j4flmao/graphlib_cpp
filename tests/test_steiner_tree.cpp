#include <gtest/gtest.h>
#include "graphlib/steiner_tree.h"
#include <algorithm>

using namespace graphlib;

TEST(SteinerTreeTest, BasicStar) {
    // 0 is center. 1, 2, 3 are leaves connected to 0 with weight 1.
    // Terminals: 1, 2, 3.
    // MST of terminals using 0 is edges (0,1), (0,2), (0,3). Total weight 3.
    Graph g(4);
    g.add_edge(0, 1, 1); g.add_edge(1, 0, 1);
    g.add_edge(0, 2, 1); g.add_edge(2, 0, 1);
    g.add_edge(0, 3, 1); g.add_edge(3, 0, 1);
    
    std::vector<int> terminals = {1, 2, 3};
    long long cost = steiner_tree(g, terminals);
    EXPECT_EQ(cost, 3);
}

TEST(SteinerTreeTest, Path) {
    // 0-1-2-3-4. Weights 1.
    // Terminals {0, 4}.
    // Cost should be 4.
    Graph g(5);
    g.add_edge(0, 1, 1); g.add_edge(1, 0, 1);
    g.add_edge(1, 2, 1); g.add_edge(2, 1, 1);
    g.add_edge(2, 3, 1); g.add_edge(3, 2, 1);
    g.add_edge(3, 4, 1); g.add_edge(4, 3, 1);
    
    std::vector<int> terminals = {0, 4};
    long long cost = steiner_tree(g, terminals);
    EXPECT_EQ(cost, 4);
    
    // Add terminal 2. Cost same (2 is on path).
    terminals = {0, 2, 4};
    cost = steiner_tree(g, terminals);
    EXPECT_EQ(cost, 4);
}

TEST(SteinerTreeTest, TriangleMetric) {
    // 0, 1, 2. All connected weight 2.
    // Terminals {0, 1, 2}. Cost 4.
    Graph g(3);
    g.add_edge(0, 1, 2); g.add_edge(1, 0, 2);
    g.add_edge(1, 2, 2); g.add_edge(2, 1, 2);
    g.add_edge(2, 0, 2); g.add_edge(0, 2, 2);
    
    long long cost = steiner_tree(g, {0, 1, 2});
    EXPECT_EQ(cost, 4);
}

// Additional Tests

TEST(SteinerTreeTest, DisconnectedGraph) {
    Graph g(4);
    g.add_edge(0, 1, 1); g.add_edge(1, 0, 1);
    g.add_edge(2, 3, 1); g.add_edge(3, 2, 1);
    // 0-1 and 2-3 are disconnected components
    
    std::vector<int> terminals = {0, 2};
    // Should be unreachable (sentinel value or error)
    // Assuming implementation returns -1 or large value for disconnected
    long long cost = steiner_tree(g, terminals);
    
    // Check if the cost indicates unreachable (e.g. max long long or similar)
    // Or if the implementation handles it.
    // Let's assume it should be greater than any possible valid path or -1.
    // If your implementation uses a specific sentinel, adjust here.
    bool unreachable = (cost == -1 || cost > 1000000); 
    EXPECT_TRUE(unreachable); 
}

TEST(SteinerTreeTest, SingleTerminal) {
    Graph g(3);
    g.add_edge(0, 1, 5); g.add_edge(1, 0, 5);
    
    // Steiner tree for 1 terminal is just that node, cost 0.
    std::vector<int> terminals = {0};
    long long cost = steiner_tree(g, terminals);
    EXPECT_EQ(cost, 0);
}

TEST(SteinerTreeTest, NoTerminals) {
    Graph g(3);
    long long cost = steiner_tree(g, {});
    EXPECT_EQ(cost, 0);
}

TEST(SteinerTreeTest, ComplexCase) {
    // A graph where Steiner Tree uses non-terminal nodes (Steiner points)
    /*
        1 --(10)-- 2
        | \      / |
       (5)  (2) (2) (5)
        |    \ /   |
        3 --(2)- S(0) --(2)-- 4
    */
    // Let 0 be S (center).
    // Terminals: 1, 2, 3, 4.
    // Edges: 
    // 0-1 (5), 0-2 (5), 0-3 (2), 0-4 (2)
    // Also 1-2 (10)
    //
    // Actually let's construct explicit Steiner Point case:
    // Terminals: 1, 2, 3.
    // Center 0.
    // dist(0,1)=1, dist(0,2)=1, dist(0,3)=1.
    // dist(1,2)=2 (via 0), dist(2,3)=2 (via 0), dist(1,3)=2 (via 0).
    // Direct edges between terminals with high weight > 2.
    
    Graph g(4);
    // Connect center 0 to Terminals 1, 2, 3 with weight 2
    g.add_edge(0, 1, 2); g.add_edge(1, 0, 2);
    g.add_edge(0, 2, 2); g.add_edge(2, 0, 2);
    g.add_edge(0, 3, 2); g.add_edge(3, 0, 2);
    
    // Connect Terminals directly with weight 5
    g.add_edge(1, 2, 5); g.add_edge(2, 1, 5);
    g.add_edge(2, 3, 5); g.add_edge(3, 2, 5);
    g.add_edge(1, 3, 5); g.add_edge(3, 1, 5);
    
    // MST of terminals (using only edges between them): 1-2 (5) + 2-3 (5) = 10.
    // Steiner Tree using node 0: 0-1 (2) + 0-2 (2) + 0-3 (2) = 6.
    
    long long cost = steiner_tree(g, {1, 2, 3});
    EXPECT_EQ(cost, 6);
}

TEST(SteinerTreeTest, LargeK) {
    // If K (number of terminals) is small (e.g. <= 10), it should be fast.
    int n = 20;
    Graph g(n);
    // Line graph 0-1-...-19
    for(int i=0; i<n-1; ++i) {
        g.add_edge(i, i+1, 1);
        g.add_edge(i+1, i, 1);
    }
    
    std::vector<int> terminals;
    for(int i=0; i<10; ++i) terminals.push_back(i*2); // 0, 2, 4, ...
    // Terminals 0, 2, 4... 18.
    // Path covers 0 to 18.
    // Length is 18.
    
    long long cost = steiner_tree(g, terminals);
    EXPECT_EQ(cost, 18);
}
