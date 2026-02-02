#include <gtest/gtest.h>
#include "graphlib/tree_algo.h"

using namespace graphlib;

TEST(TreeAlgoTest, Diameter) {
    // 0-1-2-3-4 (Length 4)
    Graph g(5);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 3); g.add_edge(3, 2);
    g.add_edge(3, 4); g.add_edge(4, 3);
    
    auto res = get_tree_diameter(g);
    EXPECT_EQ(res.length, 4);
    // Ends could be 0,4 or 4,0
    EXPECT_TRUE((res.u == 0 && res.v == 4) || (res.u == 4 && res.v == 0));
}

TEST(TreeAlgoTest, MWIS) {
    // 0(10) - 1(20) - 2(10)
    // Pick 1: 20.
    // Pick 0, 2: 10+10=20.
    // Ties? Code favors picking if sum(children) > dp_picked.
    // Wait, dp0[u] = sum(max).
    // Let's modify weights.
    // 0(100) - 1(10) - 2(100) -> Pick 0, 2 -> 200.
    
    Graph g(3);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    
    std::vector<long long> w = {100, 10, 100};
    
    std::vector<int> nodes;
    long long total = max_weight_independent_set_tree(g, w, nodes);
    
    EXPECT_EQ(total, 200);
    // Check nodes contain 0 and 2
    bool has0 = false, has2 = false;
    for(int x : nodes) { if (x==0) has0=true; if (x==2) has2=true; }
    EXPECT_TRUE(has0 && has2);
    
    // Star graph: Center(100), Leaves(10)
    // 0 - 1, 0 - 2. w[0]=100, w[1]=10, w[2]=10.
    // Pick 0 (100). Or 1,2 (20).
    Graph s(3);
    s.add_edge(0, 1); s.add_edge(1, 0);
    s.add_edge(0, 2); s.add_edge(2, 0);
    total = max_weight_independent_set_tree(s, {100, 10, 10}, nodes);
    EXPECT_EQ(total, 100);
}

TEST(TreeAlgoTest, MDS) {
    // Path 0-1-2.
    // Dom set size 1 (Node 1 covers 0,1,2).
    Graph g(3);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    
    std::vector<int> nodes;
    int size = min_dominating_set_tree(g, nodes);
    
    EXPECT_NE(size, 0); // At least 1 (actually 1)
    
    // Because of greedy bottom-up:
    // Order post-order: 2, 1, 0 (if root 0).
    // 2 not covered -> pick parent 1.
    // 1 covered, 0 covered.
    // Done. Size 1.
    EXPECT_EQ(size, 1);
    EXPECT_EQ(nodes[0], 1);
}

TEST(TreeAlgoTest, Centers) {
    // Path 0-1-2-3 (Centers 1, 2)
    Graph g(4);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 3); g.add_edge(3, 2);
    
    auto centers = get_tree_centers(g);
    EXPECT_EQ(centers.size(), 2);
    std::sort(centers.begin(), centers.end());
    EXPECT_EQ(centers[0], 1);
    EXPECT_EQ(centers[1], 2);
}
