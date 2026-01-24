#include "gtest/gtest.h"
#include "graphlib/connectivity.h"
#include "graphlib/max_flow.h"
#include "graphlib/np_hard.h"
#include "graphlib/graph_core.h"
#include "graphlib/min_cut.h"
#include <vector>
#include <numeric>
#include <algorithm>

using namespace graphlib;

// -----------------------------------------------------------------------------
// Global Min Cut (Stoer-Wagner) Tests
// -----------------------------------------------------------------------------

TEST(GlobalMinCutTest, TwoCliquesConnectedByOneEdge) {
    // Two K3 connected by a single edge of weight 2
    // Clique 1: 0, 1, 2. Edges (0,1, 10), (1,2, 10), (2,0, 10)
    // Clique 2: 3, 4, 5. Edges (3,4, 10), (4,5, 10), (5,3, 10)
    // Bridge: (2, 3) weight 2
    // Min cut should be 2.

    Graph g(6, false); // Directed flag false, but we add edges manually
    // Helper to add undirected edge
    auto add_undirected = [&](int u, int v, int w) {
        g.add_edge(u, v, w);
    };

    add_undirected(0, 1, 10);
    add_undirected(1, 2, 10);
    add_undirected(2, 0, 10);

    add_undirected(3, 4, 10);
    add_undirected(4, 5, 10);
    add_undirected(5, 3, 10);

    add_undirected(2, 3, 2);

    long long cut = global_min_cut_undirected(g);
    EXPECT_EQ(cut, 2);
}

TEST(GlobalMinCutTest, SimpleCycle) {
    // 0-1-2-3-0 all weight 1
    // Min cut is 2 (cutting any two edges)
    Graph g(4, false);
    auto add_undirected = [&](int u, int v, int w) {
        g.add_edge(u, v, w);
    };
    add_undirected(0, 1, 1);
    add_undirected(1, 2, 1);
    add_undirected(2, 3, 1);
    add_undirected(3, 0, 1);

    long long cut = global_min_cut_undirected(g);
    EXPECT_EQ(cut, 2);
}

// -----------------------------------------------------------------------------
// Gomory-Hu Tree Tests
// -----------------------------------------------------------------------------

TEST(GomoryHuTreeTest, SimpleGraph) {
    // 0 --(1)-- 1 --(2)-- 2 --(3)-- 3
    // Min cuts:
    // 0-1: 1 (cut edge 0-1)
    // 1-2: 1 (cut edge 0-1), wait? No, s-t min cut.
    // 1-2: cut 0-1 (cost 1) or 1-2 (cost 2) or 2-3 (cost 3). Min is 1? No.
    // Cut must separate 1 and 2.
    // Cut {0,1} vs {2,3} -> cost 2.
    // Cut {0} vs {1,2,3} -> cost 1 (if 0-1 is edge).
    // Let's analyze:
    // Graph: 0-1 (1), 1-2 (2), 2-3 (3).
    // Min cut between 0 and 1: Cut edge (0,1), val=1.
    // Min cut between 1 and 2: Cut edge (1,2) is 2. Cut (0,1) separates 0, but 1 and 2 are together. 
    // Wait, cut must separate s and t.
    // Cut (1,2) partitions {0,1} and {2,3}. Cost 2.
    // Cut (0,1) partitions {0} and {1,2,3}. Separates 0 and 1? Yes. Cost 1.
    // So min cut(0,1) is 1.
    // Min cut(1,2) is 2.
    // Min cut(2,3) is 2 (cut 1-2) or 3 (cut 2-3). Min is 2.
    // Wait. Path 0-1-2-3.
    // s=2, t=3. Paths: 2-3 (cap 3). Also 2-1-0 (no path to 3).
    // Max flow 2->3 is 3? No, edge 2-3 has cap 3. 
    // Is it only edges? Yes.
    // So flow 2->3 is 3. Cut is {0,1,2} vs {3}, cost 3.
    // Wait, flow is limited by min edge on path?
    // If we only have edges (0,1), (1,2), (2,3).
    // 0->3: min(1,2,3) = 1.
    // 1->3: min(2,3) = 2.
    // 2->3: 3.
    // Gomory-Hu tree edges should reflect these min cuts.
    
    Graph g(4, false);
    g.add_edge(0, 1, 1);
    g.add_edge(1, 2, 2);
    g.add_edge(2, 3, 3);

    std::vector<int> parent;
    std::vector<long long> min_cut;
    gomory_hu_tree(g, parent, min_cut);

    // Check tree structure
    // Parent array: parent[i] is neighbor of i in GH tree.
    // GH tree edges have weights = min_cut[i].
    
    // For this graph, GH tree should have edges with weights 1, 2, 3.
    // Actually, GH tree for a path graph is the path graph itself if weights are increasing?
    // Let's check max flow between pairs.
    // 0-1: 1
    // 0-2: 1
    // 0-3: 1
    // 1-2: 2
    // 1-3: 2
    // 2-3: 3
    
    // Tree edges: (0,1) w=1, (1,2) w=2, (2,3) w=3.
    // Then min_cut(u,v) = min weight on path in GH tree.
    // 0-3 path: 0-1-2-3, min is 1. Correct.
    // 1-3 path: 1-2-3, min is 2. Correct.
    
    // Verify parents and weights
    // parent[i] points to some node.
    // min_cut[i] is weight of edge (i, parent[i]).
    
    // We can verify by reconstructing tree and checking edges.
    struct EdgeInfo { int u, v; long long w; };
    std::vector<EdgeInfo> tree_edges;
    for(int i=1; i<4; ++i) {
        tree_edges.push_back({i, parent[i], min_cut[i]});
    }

    // Check if we have weights 1, 2, 3
    std::vector<long long> weights;
    for(auto& e : tree_edges) weights.push_back(e.w);
    std::sort(weights.begin(), weights.end());

    EXPECT_EQ(weights.size(), 3);
    if (weights.size() == 3) {
        EXPECT_EQ(weights[0], 1);
        EXPECT_EQ(weights[1], 2);
        EXPECT_EQ(weights[2], 3);
    }
}

// -----------------------------------------------------------------------------
// Chinese Postman Tests
// -----------------------------------------------------------------------------

TEST(ChinesePostmanTestV2, EulerianGraph) {
    Graph g(3, false);
    g.add_edge(0, 1, 10);
    g.add_edge(1, 2, 20);
    g.add_edge(2, 0, 30);
    // Eulerian, cost = 60
    EXPECT_EQ(chinese_postman(g), 60);
}

TEST(ChinesePostmanTestV2, PathGraph) {
    Graph g(4, false);
    auto add_undirected = [&](int u, int v, int w) {
        g.add_edge(u, v, w);
    };
    add_undirected(0, 1, 10);
    add_undirected(1, 2, 20);
    add_undirected(2, 3, 30);
    // Odd degrees: 0 and 3.
    // Shortest path 0-3 is 10+20+30=60.
    // Total edges cost = 60.
    // Total cost = 60 + 60 = 120.
    EXPECT_EQ(chinese_postman(g), 120);
}

TEST(ChinesePostmanTestV2, SquareWithDiagonal) {
    // 0-1 (10), 1-2 (10), 2-3 (10), 3-0 (10)
    // 0-2 (100)
    // Degrees: 0(3), 1(2), 2(3), 3(2).
    // Odd: 0, 2.
    // Shortest path 0-2: 0-1-2 (20) vs 0-2 (100). Min is 20.
    // Total edges: 4*10 + 100 = 140.
    // Added path: 20.
    // Total: 160.
    Graph g(4, false);
    auto add_undirected = [&](int u, int v, int w) {
        g.add_edge(u, v, w);
    };
    add_undirected(0, 1, 10);
    add_undirected(1, 2, 10);
    add_undirected(2, 3, 10);
    add_undirected(3, 0, 10);
    add_undirected(0, 2, 100);
    EXPECT_EQ(chinese_postman(g), 160);
}
