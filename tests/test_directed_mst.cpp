#include <gtest/gtest.h>
#include "graphlib/directed_mst.h"
#include <vector>
#include <set>

using namespace graphlib;

TEST(DirectedMSTTest, SimpleAcyclic) {
    // 0 -> 1 (10)
    // 0 -> 2 (20)
    // 1 -> 2 (5)
    // Best: 0->1 (10), 1->2 (5). Total 15.
    
    std::vector<DirectedEdge> edges;
    edges.push_back({0, 1, 10, 101});
    edges.push_back({0, 2, 20, 102});
    edges.push_back({1, 2, 5, 103});
    
    std::vector<int> res_edges;
    long long cost = directed_mst(3, 0, edges, res_edges);
    
    EXPECT_EQ(cost, 15);
    std::set<int> id_set(res_edges.begin(), res_edges.end());
    EXPECT_TRUE(id_set.count(101));
    EXPECT_TRUE(id_set.count(103));
}

TEST(DirectedMSTTest, SimpleCycle) {
    // 0 -> 1 (10)
    // 1 -> 2 (10)
    // 2 -> 1 (1)  <-- Cycle 1-2 with small weight
    // 2 -> 3 (10)
    
    // Paths from 0:
    // 0->1
    // 1->2 vs 1 (impossible, 2->1)
    // We need to reach 1, 2, 3.
    // 0->1 is mandatory (only way to enter).
    // To reach 2: 1->2 (cost 10).
    // To reach 3: 2->3 (cost 10).
    // Edge 2->1 (cost 1) creates cycle 1-2.
    // If we use 0->1, 1->2, 2->3. Cost 10+10+10 = 30.
    // Is there cheaper?
    // Maybe 0->1, 2->1? But how to reach 2? 1->2. 
    // The cycle 1->2->1 costs 11.
    // Entering the cycle at 1 costs 10 (0->1).
    // Entering at 2? No way from 0 directly to 2.
    
    // Let's add 0->2 with cost 100.
    // Option 1: 0->1 (10), 1->2 (10), 2->3 (10). Cost 30.
    // Option 2: 0->2 (100)... bad.
    
    std::vector<DirectedEdge> edges;
    edges.push_back({0, 1, 10, 1});
    edges.push_back({1, 2, 10, 2});
    edges.push_back({2, 1, 1, 3});
    edges.push_back({2, 3, 10, 4});
    edges.push_back({0, 2, 100, 5});
    
    std::vector<int> res_edges;
    long long cost = directed_mst(4, 0, edges, res_edges);
    
    EXPECT_EQ(cost, 30);
    // Should select 0->1, 1->2, 2->3.
    std::set<int> id_set(res_edges.begin(), res_edges.end());
    EXPECT_TRUE(id_set.count(1));
    EXPECT_TRUE(id_set.count(2));
    EXPECT_TRUE(id_set.count(4));
}

TEST(DirectedMSTTest, ComplexCycleContraction) {
    // 0 -> 1 (10)
    // 1 -> 2 (10)
    // 2 -> 3 (10)
    // 3 -> 1 (10)  <-- Cycle 1-2-3 (cost 30)
    
    // 0 -> 2 (25)  <-- Shortcut to 2.
    
    // If we take 0->1 (10), we enter cycle at 1.
    // Internal cycle edges needed: 1->2, 2->3, 3->1.
    // To limit to tree, we need n-1 edges. 
    // Cycle has 3 nodes {1,2,3}. We need 2 edges from cycle, plus entry.
    // Best entry 0->1 (10).
    // Edges in cycle: 1->2 (10), 2->3 (10), 3->1 (10).
    // If enter at 1, we drop 3->1.
    // Cost: 10 (0->1) + 10 (1->2) + 10 (2->3) = 30.
    
    // Alternative: Enter at 2 via 0->2 (25).
    // We drop 1->2. Use 3->1 and 2->3.
    // Cost: 25 (0->2) + 10 (2->3) + 10 (3->1) = 45. 
    
    // So solution 30 is better.
    
    // Now make 0->2 cheaper. Say 15.
    // Option 1: 10 + 10 + 10 = 30.
    // Option 2: 15 (0->2) + 10 (2->3) + 10 (3->1) = 35. Still worse.
    
    // Make cycle edges heavy.
    // 1->2 (100), 2->3 (100), 3->1 (100).
    // 0->1 (10).
    // Cost 10 + 100 + 100 = 210.
    
    // Add 0->2 (50).
    // Enter at 2. Drop 1->2. Use 3->1 and 2->3.
    // Cost 50 (0->2) + 100 (2->3) + 100 (3->1) = 250.
    
    // Let's create a case where entering a different node is better.
    // Cycle 1-2-3.
    // 1->2 (10), 2->3 (10), 3->1 (100).
    // Entry 0->1 (100).
    // Entry 0->3 (20).
    
    // If enter 1: 100 + 10 + 10 = 120. (Break 3->1)
    // If enter 3: 20 + 100(3->1) + 10(1->2) = 130. (Break 2->3) - Wait.
    // If enter 3, we keep edges pointing to others?
    // Edges available: 1->2, 2->3, 3->1.
    // If root at 3 (locally): 3->1, 1->2. Cost 100+10 = 110.
    // Total 20 + 110 = 130.
    
    // 120 is optimal.
    
    std::vector<DirectedEdge> edges;
    edges.push_back({0, 1, 100, 1});
    edges.push_back({0, 3, 20, 2});
    edges.push_back({1, 2, 10, 3});
    edges.push_back({2, 3, 10, 4});
    edges.push_back({3, 1, 100, 5});
    
    std::vector<int> res;
    long long cost = directed_mst(4, 0, edges, res);
    
    EXPECT_EQ(cost, 120);
    // Should contain 0->1 (1), 1->2 (3), 2->3 (4).
    std::set<int> id_set(res.begin(), res.end());
    EXPECT_TRUE(id_set.count(1));
    EXPECT_TRUE(id_set.count(3));
    EXPECT_TRUE(id_set.count(4));
    EXPECT_FALSE(id_set.count(2)); // 0->3 not used
    EXPECT_FALSE(id_set.count(5)); // 3->1 not used
}

TEST(DirectedMSTTest, Impossible) {
    std::vector<DirectedEdge> edges;
    edges.push_back({0, 1, 10, 1});
    // Node 2 unreachable
    
    std::vector<int> res;
    long long cost = directed_mst(3, 0, edges, res);
    EXPECT_EQ(cost, -1);
}
