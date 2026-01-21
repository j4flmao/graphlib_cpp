#include <gtest/gtest.h>
#include "graphlib/graph_core.h"
#include "graphlib/bipartite.h"
#include "graphlib/connectivity.h"
#include "graphlib/dag.h"
#include "graphlib/general_matching.h"
#include "graphlib/max_flow.h"
#include "graphlib/mst.h"
#include "graphlib/scc.h"
#include "graphlib/shortest_path.h"
#include "graphlib/splay_tree.h"
#include "graphlib/tree.h"
#include <numeric>
#include <algorithm>

using namespace graphlib;

// --- Graph Core Tests ---

TEST(FullCoverage, MakeCompleteGraph) {
    int n = 5;
    Graph g = make_complete_graph(n, true); // Directed
    EXPECT_EQ(g.vertex_count(), n);
    // n * (n-1) edges
    int edge_count = 0;
    for (int i = 0; i < n; ++i) {
        Edge* e = g.get_edges(i);
        while (e) {
            edge_count++;
            e = e->next;
        }
    }
    EXPECT_EQ(edge_count, n * (n - 1));

    Graph g_undir = make_complete_graph(n, false); // Undirected
    // n * (n-1) edges (represented as 2 * edges in adj list)
    int edge_count_undir = 0;
    for (int i = 0; i < n; ++i) {
        Edge* e = g_undir.get_edges(i);
        while (e) {
            edge_count_undir++;
            e = e->next;
        }
    }
    EXPECT_EQ(edge_count_undir, n * (n - 1));
}

TEST(FullCoverage, MakeGraphFromEdgesOneBased) {
    int n = 3;
    std::vector<std::pair<int, int>> edges = {{1, 2}, {2, 3}, {3, 1}};
    Graph g = make_graph_from_edges_one_based(n, edges, true);
    // 1->2 becomes 0->1
    // 2->3 becomes 1->2
    // 3->1 becomes 2->0
    
    auto check_edge = [&](int u, int v) {
        Edge* e = g.get_edges(u);
        bool found = false;
        while (e) {
            if (e->to == v) found = true;
            e = e->next;
        }
        return found;
    };

    EXPECT_TRUE(check_edge(0, 1));
    EXPECT_TRUE(check_edge(1, 2));
    EXPECT_TRUE(check_edge(2, 0));
    EXPECT_FALSE(check_edge(1, 0));
}

TEST(FullCoverage, BfsMultiSource) {
    int n = 6;
    Graph g(n, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(3, 4);
    
    std::vector<int> sources = {0, 3};
    std::vector<int> dist = bfs_multi_source(g, sources, -1);
    
    EXPECT_EQ(dist[0], 0);
    EXPECT_EQ(dist[1], 1);
    EXPECT_EQ(dist[2], 2);
    EXPECT_EQ(dist[3], 0);
    EXPECT_EQ(dist[4], 1);
    EXPECT_EQ(dist[5], -1); // Unreachable
}

TEST(FullCoverage, GomoryHuTree) {
    // Simple barbell graph: 0-1-2-3-4-5, edges (2,3) is bridge
    // Actually Gomory-Hu is for undirected graphs
    int n = 6;
    Graph g(n, false);
    g.add_edge(0, 1, 10);
    g.add_edge(1, 2, 10);
    g.add_edge(2, 3, 1); // min cut
    g.add_edge(3, 4, 10);
    g.add_edge(4, 5, 10);

    std::vector<int> parent;
    std::vector<long long> min_cut;
    gomory_hu_tree(g, parent, min_cut);
    
    EXPECT_EQ(parent.size(), n);
    EXPECT_EQ(min_cut.size(), n);
    
    // Find cut between 2 and 3 (or generally min cut in graph)
    // The global min cut is 1.
    long long global_min = global_min_cut_undirected(g);
    EXPECT_EQ(global_min, 1);
}

// --- Bipartite Tests ---

TEST(FullCoverage, HungarianMinCost) {
    // 3 workers (0,1,2), 3 tasks (3,4,5)
    BipartiteGraph g(3, 3);
    
    // Edges from Left (0-2) to Right (3-5)
    // Optimal matching: 0->3 (cost 1), 1->4 (cost 1), 2->5 (cost 1) -> Total 3
    g.add_edge(0, 3, 1);
    g.add_edge(1, 4, 1);
    g.add_edge(2, 5, 1);
    
    // Expensive alternatives
    g.add_edge(0, 4, 100);
    g.add_edge(1, 3, 100);
    
    long long cost = g.hungarian_min_cost(1000000);
    EXPECT_EQ(cost, 3);
}

// --- Connectivity Tests ---

TEST(FullCoverage, BiconnectedComponents) {
    // 0-1-2
    // |/
    // 3
    // 1 is articulation point.
    // Edges: (0,1), (1,2), (0,3), (1,3)
    // Biconnected components: {1,2} is a bridge-like, {0,1,3} is a cycle
    
    Connectivity g(4);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(0, 3);
    g.add_edge(1, 3);
    
    std::vector<std::vector<int>> bccs;
    g.biconnected_components(bccs);
    
    // Should have 2 components
    // One containing {1, 2} edges (or vertices?) usually vertices.
    // If vertices: {1, 2} and {0, 1, 3}
    EXPECT_GE(bccs.size(), 2);
}

// --- DAG Tests ---

TEST(FullCoverage, DAGCycleDetection) {
    DAG g(3);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    
    EXPECT_TRUE(g.would_create_cycle(2, 0));
    EXPECT_FALSE(g.would_create_cycle(0, 2));
    
    EXPECT_FALSE(g.add_edge_checked(2, 0)); // Should fail
    EXPECT_TRUE(g.add_edge_checked(0, 2));  // Should succeed
}

TEST(FullCoverage, SCCCondensation) {
    // 0->1, 1->0 (SCC 1), 2->3, 3->2 (SCC 2), 1->2 (Edge between SCCs)
    Graph g(4);
    g.add_edge(0, 1);
    g.add_edge(1, 0);
    g.add_edge(2, 3);
    g.add_edge(3, 2);
    g.add_edge(1, 2);
    
    SCC scc_solver(4);
    scc_solver.add_edge(0, 1);
    scc_solver.add_edge(1, 0);
    scc_solver.add_edge(2, 3);
    scc_solver.add_edge(3, 2);
    scc_solver.add_edge(1, 2);
    
    std::vector<int> components;
    int count = scc_solver.tarjan(components);
    
    DAG dag = build_scc_condensation_dag(g, components, count);
    EXPECT_EQ(dag.vertex_count(), 2);
    
    // Check edge exists from SCC(0/1) to SCC(2/3)
    int scc1 = components[0];
    int scc2 = components[2];
    
    // Verify edge scc1 -> scc2
    Edge* e = dag.get_edges(scc1);
    bool found = false;
    while(e) {
        if (e->to == scc2) found = true;
        e = e->next;
    }
    EXPECT_TRUE(found);
}

// --- Max Flow Tests ---

TEST(FullCoverage, MinCostCirculation) {
    // Force flow through edges using lower bounds
    // 0 -> 1 (lower=1, upper=1, cost=10)
    // 1 -> 0 (lower=1, upper=1, cost=5)
    // Circulation must use both edges. Total cost 15.
    std::vector<CirculationEdge> edges;
    edges.push_back({0, 1, 1, 1, 10});
    edges.push_back({1, 0, 1, 1, 5});
    
    long long total_cost = 0;
    bool feasible = min_cost_circulation(2, edges, total_cost);
    
    EXPECT_TRUE(feasible);
    EXPECT_EQ(total_cost, 15);
}

// --- MST UnionFind ---

TEST(FullCoverage, UnionFindLogic) {
    UnionFind uf(5);
    EXPECT_NE(uf.find(0), uf.find(1));
    uf.unite(0, 1);
    EXPECT_EQ(uf.find(0), uf.find(1));
    uf.unite(2, 3);
    EXPECT_NE(uf.find(0), uf.find(2));
    uf.unite(1, 3);
    EXPECT_EQ(uf.find(0), uf.find(2));
}

// --- Shortest Path Extra ---

TEST(FullCoverage, ZeroOneBFS) {
    ShortestPath sp(3);
    sp.add_edge(0, 1, 0);
    sp.add_edge(1, 2, 1);
    sp.add_edge(0, 2, 5);
    
    std::vector<long long> dist = sp.zero_one_bfs(0, 100);
    EXPECT_EQ(dist[0], 0);
    EXPECT_EQ(dist[1], 0);
    EXPECT_EQ(dist[2], 1);
}

TEST(FullCoverage, AStarHeuristic) {
    // 0 -> 1 -> 2 (weights 1, 1)
    // Heuristic: h(0)=2, h(1)=1, h(2)=0 (Perfect heuristic)
    ShortestPath sp(3);
    sp.add_edge(0, 1, 1);
    sp.add_edge(1, 2, 1);
    
    std::vector<long long> h = {2, 1, 0};
    std::vector<long long> dist = sp.a_star(0, 2, h, 100);
    
    EXPECT_EQ(dist[2], 2);
}

TEST(FullCoverage, ReconstructPath) {
    std::vector<int> parent = {-1, 0, 1, 2}; // 0->1->2->3
    std::vector<int> path = reconstruct_path(0, 3, parent);
    ASSERT_EQ(path.size(), 4);
    EXPECT_EQ(path[0], 0);
    EXPECT_EQ(path[1], 1);
    EXPECT_EQ(path[2], 2);
    EXPECT_EQ(path[3], 3);
}

// --- Splay Tree ---

TEST(FullCoverage, SplayTreeOperations) {
    SplayTree st;
    st.insert(10);
    st.insert(20);
    st.insert(5);
    
    EXPECT_TRUE(st.search(10));
    EXPECT_TRUE(st.search(5));
    EXPECT_FALSE(st.search(15));
    
    st.remove(10);
    EXPECT_FALSE(st.search(10));
    EXPECT_TRUE(st.search(20));
    
    // Split and Join
    SplayTree left, right;
    st.split(15, left, right); // split by key 15
    // 5 should be in left, 20 in right
    EXPECT_TRUE(left.search(5));
    EXPECT_FALSE(left.search(20));
    EXPECT_FALSE(right.search(5));
    EXPECT_TRUE(right.search(20));
    
    SplayTree::join(left, right);
    // joined back into left (implementation detail: result in left usually?)
    // Checking the header: static void join(SplayTree& left, SplayTree& right);
    // Usually merges right into left or similar.
    // Assuming left becomes the result.
    EXPECT_TRUE(left.search(5));
    EXPECT_TRUE(left.search(20));
}

// --- Tree LCA / HLD ---

TEST(FullCoverage, TreeHLD) {
    // 0-1, 0-2, 1-3
    TreeLCA t(4);
    t.add_edge(0, 1);
    t.add_edge(0, 2);
    t.add_edge(1, 3);
    t.build(0);
    
    EXPECT_EQ(t.parent(3), 1);
    EXPECT_EQ(t.parent(1), 0);
    EXPECT_EQ(t.depth(3), 2);
    
    EXPECT_TRUE(t.is_ancestor(0, 3));
    EXPECT_FALSE(t.is_ancestor(3, 0));
    
    EXPECT_EQ(t.kth_ancestor(3, 1), 1);
    EXPECT_EQ(t.kth_ancestor(3, 2), 0);
    
    EXPECT_EQ(t.subtree_size(3), 1);
    EXPECT_GE(t.subtree_size(0), 4);
}

TEST(FullCoverage, TreePathSum) {
    // 0-1-2
    TreeLCA t(3);
    t.add_edge(0, 1);
    t.add_edge(1, 2);
    t.build(0);
    
    std::vector<long long> values = {1, 2, 3}; // val(0)=1, val(1)=2, val(2)=3
    TreePathSum tps(t, values);
    
    EXPECT_EQ(tps.query_path(0, 2), 1 + 2 + 3);
    
    tps.add_value(1, 10); // val(1) becomes 12
    EXPECT_EQ(tps.query_path(0, 2), 1 + 12 + 3);
    
    tps.set_value(2, 0);
    EXPECT_EQ(tps.query_path(0, 2), 1 + 12 + 0);
}
