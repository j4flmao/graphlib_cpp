#include <gtest/gtest.h>
#include <graphlib/graphlib.h>
#include <vector>
#include <random>
#include <algorithm>
#include <limits>
#include <set>

using namespace graphlib;

// Helper to generate random int
int random_int(int min, int max) {
    static std::mt19937 rng(42); // Fixed seed for reproducibility
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

// 1. Shortest Path Stress: Grid Graph
TEST(AdvancedTest, ShortestPathGrid) {
    int rows = 20;
    int cols = 20;
    int n = rows * cols;
    ShortestPath sp(n);
    long long inf = std::numeric_limits<long long>::max() / 4;

    // Create a grid with random positive weights
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int u = r * cols + c;
            // Right
            if (c + 1 < cols) {
                int v = r * cols + (c + 1);
                sp.add_edge(u, v, random_int(1, 10));
                sp.add_edge(v, u, random_int(1, 10));
            }
            // Down
            if (r + 1 < rows) {
                int v = (r + 1) * cols + c;
                sp.add_edge(u, v, random_int(1, 10));
                sp.add_edge(v, u, random_int(1, 10));
            }
        }
    }

    // Compare Dijkstra (optimized) with Bellman-Ford (general)
    // Bellman-Ford is slower, so we only check from source 0
    std::vector<long long> dist_dijkstra = sp.dijkstra(0, inf);
    
    bool has_cycle = false;
    std::vector<long long> dist_bf = sp.bellman_ford(0, inf, has_cycle);

    EXPECT_FALSE(has_cycle);
    ASSERT_EQ(dist_dijkstra.size(), dist_bf.size());
    for (int i = 0; i < n; i++) {
        EXPECT_EQ(dist_dijkstra[i], dist_bf[i]) << "Mismatch at node " << i;
    }
}

// 2. SCC Stress: Large Cycle
TEST(AdvancedTest, SCCLargeCycle) {
    int n = 1000;
    SCC g(n);
    for (int i = 0; i < n; i++) {
        g.add_edge(i, (i + 1) % n);
    }

    std::vector<int> comp;
    int count = g.tarjan(comp);

    EXPECT_EQ(count, 1);
    for (int i = 0; i < n; i++) {
        EXPECT_EQ(comp[i], comp[0]);
    }
}

// 3. SCC Stress: Disconnected Components
TEST(AdvancedTest, SCCDisconnected) {
    int n = 1000;
    SCC g(n);
    // Create 100 separate components of size 10
    for (int k = 0; k < 100; k++) {
        int base = k * 10;
        for (int i = 0; i < 10; i++) {
            g.add_edge(base + i, base + (i + 1) % 10);
        }
    }

    std::vector<int> comp;
    int count = g.tarjan(comp);

    EXPECT_EQ(count, 100);
}

// 4. Max Flow: Disconnected Source Sink
TEST(AdvancedTest, MaxFlowDisconnected) {
    int n = 10;
    MaxFlow mf(n);
    // 0 connected to 1, 2 connected to 3
    mf.add_edge(0, 1, 10);
    mf.add_edge(2, 3, 10);
    
    long long flow = mf.dinic(0, 3);
    EXPECT_EQ(flow, 0);
}

// 5. MST: Equal Weights
TEST(AdvancedTest, MSTEqualWeights) {
    int n = 50;
    std::vector<MstEdge> edges;
    // Complete graph with weight 1
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            edges.push_back(MstEdge{i, j, 1});
        }
    }

    long long total = MST::kruskal(n, edges);
    
    // Total weight should be n-1 because all edges have weight 1
    EXPECT_EQ(total, n - 1);
}

// 6. Shortest Path: Disconnected Graph with Unreachable Nodes
TEST(AdvancedTest, ShortestPathUnreachable) {
    int n = 10;
    ShortestPath sp(n);
    long long inf = std::numeric_limits<long long>::max() / 4;

    sp.add_edge(0, 1, 5);
    sp.add_edge(1, 2, 5);
    // 3 is isolated
    // 4->5
    sp.add_edge(4, 5, 10);

    std::vector<long long> dist = sp.dijkstra(0, inf);

    EXPECT_EQ(dist[0], 0);
    EXPECT_EQ(dist[1], 5);
    EXPECT_EQ(dist[2], 10);
    EXPECT_EQ(dist[3], inf);
    EXPECT_EQ(dist[4], inf);
    EXPECT_EQ(dist[5], inf);
}

// 7. TwoSAT: Unsatisfiable Chain
TEST(AdvancedTest, TwoSATUnsatisfiableChain) {
    int n = 10;
    TwoSAT ts(n);
    // x0 -> x1 -> x2 ... -> x9
    // Implication A -> B is equiv to !A \/ B
    
    // x0 -> x1
    ts.add_clause(0, false, 1, true); 
    // x1 -> x2 ...
    for (int i = 1; i < n - 1; i++) {
        ts.add_clause(i, false, i + 1, true);
    }
    // x9 -> x0
    ts.add_clause(n - 1, false, 0, true);
    
    // Now we have a cycle x0 -> ... -> x0
    // This is satisfiable (all true or all false).
    
    // Force x0 != x1
    // x0 -> !x1  => !x0 \/ !x1
    // x1 -> !x0  => !x1 \/ !x0
    ts.add_clause(0, false, 1, false);
    
    // But x0 -> x1 is already there.
    // x0 -> x1 AND x0 -> !x1 implies x0 -> False, so x0 must be False.
    // If x0 is False, then x9 -> x0 implies x9 is False... all False.
    // Is this unsatisfiable?
    // All False:
    // x0=F, x1=F...
    // x0->x1 (T), x1->x2 (T)...
    // x0!=x1 => F!=F (False).
    // So !x0 \/ !x1 is True if x0=F.
    
    // Wait, let's construct a simpler contradiction.
    // x0 AND !x0
    
    TwoSAT ts2(2);
    ts2.add_unit_clause(0, true);  // x0 must be true
    ts2.add_unit_clause(0, false); // x0 must be false
    
    std::vector<bool> assignment;
    bool sat = ts2.solve(assignment);
    EXPECT_FALSE(sat);
}

// 8. General Matching: Star Graph (Center can match only one)
TEST(AdvancedTest, GeneralMatchingStar) {
    int n = 10;
    GeneralMatching gm(n);
    // Center 0, leaves 1..9
    for (int i = 1; i < n; i++) {
        gm.add_edge(0, i);
    }
    
    int matching = gm.maximum_matching();
    EXPECT_EQ(matching, 1);
}

// 9. LCA: Linear Tree (Deep recursion check)
TEST(AdvancedTest, LCALinear) {
    int n = 1000;
    TreeLCA tree(n);
    // 0-1-2-...-999
    for (int i = 0; i < n - 1; i++) {
        tree.add_edge(i, i + 1);
    }
    tree.build(0);

    EXPECT_EQ(tree.lca(0, n - 1), 0);
    EXPECT_EQ(tree.lca(n - 2, n - 1), n - 2);
    EXPECT_EQ(tree.lca(500, 600), 500);
}

// 10. SplayTree: Random Insertions and Deletions
TEST(AdvancedTest, SplayTreeRandom) {
    SplayTree st;
    std::set<int> std_set;
    int n_ops = 1000;
    
    for (int i = 0; i < n_ops; i++) {
        int val = random_int(0, 1000);
        int op = random_int(0, 2);
        
        if (op == 0) { // Insert
            st.insert(val);
            std_set.insert(val);
        } else if (op == 1) { // Remove
            st.remove(val);
            std_set.erase(val);
        } else { // Search
            bool found = st.search(val);
            bool expected = (std_set.find(val) != std_set.end());
            EXPECT_EQ(found, expected);
        }
    }
    
    // Final check
    for (int val : std_set) {
        EXPECT_TRUE(st.search(val));
    }
}

// 11. Connectivity: Large Tree (All edges are bridges)
TEST(AdvancedTest, ConnectivityTreeBridges) {
    int n = 500;
    Connectivity g(n);
    // Create a path 0-1-2-...
    for (int i = 0; i < n - 1; i++) {
        g.add_edge(i, i + 1);
    }
    
    std::vector<std::pair<int, int>> bridges;
    g.bridges(bridges);
    
    // In a tree, every edge is a bridge
    EXPECT_EQ(bridges.size(), n - 1);
}

// 12. Min Cost Max Flow: Prefer cheaper path
TEST(AdvancedTest, MinCostMaxFlowSimple) {
    int n = 4;
    MaxFlow mf(n);
    // Source 0, Sink 3
    // Path 1: 0->1->3 (Cap 10, Cost 1+1=2)
    mf.add_edge(0, 1, 10, 1);
    mf.add_edge(1, 3, 10, 1);
    
    // Path 2: 0->2->3 (Cap 10, Cost 5+5=10)
    mf.add_edge(0, 2, 10, 5);
    mf.add_edge(2, 3, 10, 5);
    
    // Send flow 5. Should take path 1. Cost 5*2 = 10.
    // Wait, min_cost_max_flow pushes MAX flow.
    // Total capacity is 20. Flow will be 20.
    // 10 units via path 1 (cost 10*2 = 20)
    // 10 units via path 2 (cost 10*10 = 100)
    // Total cost 120.
    
    std::pair<long long, long long> res = mf.min_cost_max_flow(0, 3);
    EXPECT_EQ(res.first, 20); // Max Flow
    EXPECT_EQ(res.second, 120); // Min Cost
}

// 13. Bipartite Matching: Complete Bipartite Graph K_5_5
TEST(AdvancedTest, BipartiteMatchingComplete) {
    int n_left = 5;
    int n_right = 5;
    BipartiteGraph bp(n_left, n_right);
    
    for (int i = 0; i < n_left; i++) {
        for (int j = 0; j < n_right; j++) {
            // Left nodes 0..4, Right nodes 5..9
            bp.add_edge(i, n_left + j);
        }
    }
    
    int matching = bp.maximum_matching();
    
    EXPECT_EQ(matching, 5);
}

// 14. Dynamic SCC: Merging Components
TEST(AdvancedTest, DynamicSCCIncremental) {
    DynamicSCC dscc(5);
    dscc.add_edge(0, 1);
    dscc.add_edge(1, 2);
    dscc.add_edge(2, 0);
    // 0-1-2 is a cycle. Nodes 3, 4 are isolated.
    // Components: {0,1,2}, {3}, {4} -> Total 3.
    
    EXPECT_EQ(dscc.component_count(), 3);
    EXPECT_TRUE(dscc.strongly_connected(0, 1));
    EXPECT_TRUE(dscc.strongly_connected(1, 2));
    EXPECT_FALSE(dscc.strongly_connected(0, 3));
    
    dscc.add_edge(2, 3);
    dscc.add_edge(3, 4);
    dscc.add_edge(4, 2);
    // Now 2->3->4->2 is a cycle.
    // Since 0-1-2 is SCC, and 2 is in both, all 0,1,2,3,4 become one SCC.
    
    EXPECT_EQ(dscc.component_count(), 1);
    EXPECT_TRUE(dscc.strongly_connected(0, 4));
}

// 15. Eulerian Path: Directed Graph
TEST(AdvancedTest, EulerianPathDirected) {
    Graph g(4, true);
    // 0->1, 1->2, 2->1, 1->3
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 1);
    g.add_edge(1, 3);
    
    EXPECT_TRUE(has_eulerian_trail_directed(g));
    EXPECT_FALSE(has_eulerian_cycle_directed(g));
    
    std::vector<int> path = eulerian_trail_directed(g);
    // Path should have N_edges + 1 vertices = 5
    EXPECT_EQ(path.size(), 5);
    
    // Check start and end
    EXPECT_EQ(path.front(), 0);
    EXPECT_EQ(path.back(), 3);
}

// 16. General Matching: Weighted
TEST(AdvancedTest, GeneralMatchingWeighted) {
    GeneralMatching gm(3);
    gm.add_edge(0, 1, 10);
    gm.add_edge(1, 2, 10);
    gm.add_edge(2, 0, 1);
    
    // Max weight matching: can pick only one edge (since 3 vertices).
    // Max weight is 10.
    EXPECT_EQ(gm.maximum_weight_matching(), 10);
    
    // Add another disjoint edge
    GeneralMatching gm2(4);
    gm2.add_edge(0, 1, 10);
    gm2.add_edge(2, 3, 20);
    gm2.add_edge(1, 2, 5); // Bridge
    
    // Should pick 0-1 (10) and 2-3 (20). Total 30.
    EXPECT_EQ(gm2.maximum_weight_matching(), 30);
}

// 17. Global Min Cut: Barbell Graph
TEST(AdvancedTest, GlobalMinCutBarbell) {
    // Two cliques of size 5 connected by one edge.
    int k = 5;
    int n = 2 * k;
    Graph g(n, false);
    
    // Clique 1: 0..4
    for(int i=0; i<k; ++i)
        for(int j=i+1; j<k; ++j)
            g.add_edge(i, j, 100);
            
    // Clique 2: 5..9
    for(int i=k; i<n; ++i)
        for(int j=i+1; j<n; ++j)
            g.add_edge(i, j, 100);
            
    // Bridge: 0-5 with weight 1
    g.add_edge(0, k, 1);
    
    EXPECT_EQ(global_min_cut_undirected(g), 1);
}

// 18. Tree: Kth Ancestor and Path
TEST(AdvancedTest, TreeKthOperations) {
    int n = 5;
    TreeLCA tree(n);
    // 0-1-2-3-4
    for (int i = 0; i < n - 1; i++) {
        tree.add_edge(i, i + 1);
    }
    tree.build(0);
    
    // Kth Ancestor
    EXPECT_EQ(tree.kth_ancestor(4, 1), 3);
    EXPECT_EQ(tree.kth_ancestor(4, 2), 2);
    EXPECT_EQ(tree.kth_ancestor(4, 4), 0);
    EXPECT_EQ(tree.kth_ancestor(4, 5), -1); // Out of bounds
    
    // Kth on Path
    // Path 0 -> 4: 0, 1, 2, 3, 4
    EXPECT_EQ(tree.kth_on_path(0, 4, 0), 0);
    EXPECT_EQ(tree.kth_on_path(0, 4, 2), 2);
    EXPECT_EQ(tree.kth_on_path(0, 4, 4), 4);
    
    // Path 4 -> 0: 4, 3, 2, 1, 0
    EXPECT_EQ(tree.kth_on_path(4, 0, 0), 4);
    EXPECT_EQ(tree.kth_on_path(4, 0, 1), 3);
    EXPECT_EQ(tree.kth_on_path(4, 0, 4), 0);
    
    // Distance
    EXPECT_EQ(tree.distance(0, 4), 4);
}

// 19. DAG: Longest Path with Weights
TEST(AdvancedTest, DAGLongestPath) {
    DAG dag(6);
    // 0->1 (1), 0->2 (2)
    // 1->3 (3), 2->3 (1)
    // 3->4 (4), 3->5 (5)
    // Paths to 3: 0->1->3 (1+3=4), 0->2->3 (2+1=3). Longest is 4.
    // Paths to 4: 4+4 = 8.
    // Paths to 5: 4+5 = 9.
    
    dag.add_edge(0, 1, 1);
    dag.add_edge(0, 2, 2);
    dag.add_edge(1, 3, 3);
    dag.add_edge(2, 3, 1);
    dag.add_edge(3, 4, 4);
    dag.add_edge(3, 5, 5);
    
    std::vector<long long> dist = dag.longest_path(0, -1e18);
    
    EXPECT_EQ(dist[3], 4);
    EXPECT_EQ(dist[4], 8);
    EXPECT_EQ(dist[5], 9);
}

// 20. MST: Consistency Check (Prim vs Kruskal) on Random Graph
TEST(AdvancedTest, MSTConsistencyRandom) {
    int n = 50;
    int m = 200;
    Graph g = make_random_graph(n, m, false, 1, 100, 42);
    
    // Ensure connectivity by adding a path 0-1-...-n-1 with weight 1000 (so they are expensive, 
    // but ensure connectivity if random edges don't cover everything).
    // Actually, use weight 10000 to avoid interfering with MST if possible, 
    // or just rely on random edges.
    // Better: if not connected, Prim won't match Kruskal.
    // Let's just add a path 0-1-2... with weight 100.
    for (int i = 0; i < n - 1; i++) {
        g.add_edge(i, i + 1, 100);
    }
    
    // Convert Graph to MstEdge list for Kruskal
    std::vector<MstEdge> edges;
    for (int u = 0; u < n; ++u) {
        Edge* e = g.get_edges(u);
        while (e) {
            if (u < e->to) { // Undirected, only add once
                edges.push_back({u, e->to, e->weight});
            }
            e = e->next;
        }
    }
    
    // Run Kruskal
    long long w_k = MST::kruskal(n, edges);
    
    // Convert Graph to adj list for Prim
    std::vector<std::vector<std::pair<int, long long>>> adj(n);
    for (int u = 0; u < n; ++u) {
        Edge* e = g.get_edges(u);
        while (e) {
            adj[u].push_back({e->to, e->weight});
            e = e->next;
        }
    }
    
    // Run Prim
    long long w_p = MST::prim(n, 0, adj, 1e18);
    
    EXPECT_EQ(w_k, w_p);
}
