#include <gtest/gtest.h>
#include <graphlib/dag.h>
#include <graphlib/tree.h>
#include <graphlib/scc.h>
#include <vector>
#include <random>

using namespace graphlib;

// -----------------------------------------------------------------------------
// DAG Advanced Tests
// -----------------------------------------------------------------------------

TEST(DAGAdvancedTest, PascalsTrianglePaths) {
    // Construct a DAG representing Pascal's Triangle depth 4
    // Nodes (row, col) mapped to integer IDs
    // (0,0) -> 0
    // (1,0) -> 1, (1,1) -> 2
    // (2,0) -> 3, (2,1) -> 4, (2,2) -> 5
    // Edges: (r, c) -> (r+1, c) and (r, c) -> (r+1, c+1)
    
    DAG dag(6);
    dag.add_edge(0, 1); dag.add_edge(0, 2);
    dag.add_edge(1, 3); dag.add_edge(1, 4);
    dag.add_edge(2, 4); dag.add_edge(2, 5);
    
    // Number of paths from top (0) to bottom layer nodes
    // (2,0) id=3: 1 path (0->1->3)
    // (2,1) id=4: 2 paths (0->1->4, 0->2->4)
    // (2,2) id=5: 1 path (0->2->5)
    
    EXPECT_EQ(dag.count_paths(0, 3), 1);
    EXPECT_EQ(dag.count_paths(0, 4), 2);
    EXPECT_EQ(dag.count_paths(0, 5), 1);
}

TEST(DAGAdvancedTest, LongestPathCPM) {
    // Critical Path Method example
    // Tasks: Start(0), A(1), B(2), C(3), End(4)
    // Durations as edge weights:
    // Start->A: 3
    // Start->B: 2
    // A->C: 5
    // B->C: 2
    // C->End: 4
    
    DAG dag(5);
    dag.add_edge(0, 1, 3);
    dag.add_edge(0, 2, 2);
    dag.add_edge(1, 3, 5);
    dag.add_edge(2, 3, 2);
    dag.add_edge(3, 4, 4);
    
    std::vector<long long> dist = dag.longest_path(0, -1e18);
    
    EXPECT_EQ(dist[0], 0);
    EXPECT_EQ(dist[1], 3);
    EXPECT_EQ(dist[2], 2);
    EXPECT_EQ(dist[3], 8); // 0->1->3 (3+5=8) > 0->2->3 (2+2=4)
    EXPECT_EQ(dist[4], 12); // 0->1->3->4 (8+4=12)
}

TEST(DAGAdvancedTest, SCCCondensationStructure) {
    // Graph with 3 SCCs:
    // SCC1: 0-1-2 cycle
    // SCC2: 3-4 cycle
    // SCC3: 5 (single node)
    // Edges: SCC1 -> SCC2, SCC2 -> SCC3, SCC1 -> SCC3
    
    SCC scc_solver(6);
    // SCC1
    scc_solver.add_edge(0, 1); scc_solver.add_edge(1, 2); scc_solver.add_edge(2, 0);
    // SCC2
    scc_solver.add_edge(3, 4); scc_solver.add_edge(4, 3);
    // SCC3 is 5
    
    // Inter-SCC edges
    scc_solver.add_edge(1, 3); // SCC1 -> SCC2
    scc_solver.add_edge(4, 5); // SCC2 -> SCC3
    scc_solver.add_edge(2, 5); // SCC1 -> SCC3
    
    std::vector<int> components(6);
    int count = scc_solver.tarjan(components);
    
    EXPECT_EQ(count, 3);
    
    // Condense
    DAG condensation = build_scc_condensation_dag(scc_solver, components, count);
    
    // Check DAG property (no cycles)
    bool has_cycle = false;
    condensation.topological_sort_kahn(has_cycle);
    EXPECT_FALSE(has_cycle);
    
    // Check reachability in condensation DAG
    // component_id of SCC1 should reach component_id of SCC3
    int id1 = components[0];
    int id2 = components[3];
    int id3 = components[5];
    
    EXPECT_GT(condensation.count_paths(id1, id2), 0);
    EXPECT_GT(condensation.count_paths(id2, id3), 0);
    EXPECT_GT(condensation.count_paths(id1, id3), 0); // via direct or transitive
}

// -----------------------------------------------------------------------------
// Tree Advanced Tests (HLD, Path Sum)
// -----------------------------------------------------------------------------

TEST(TreeAdvancedTest, SubtreeQueries) {
    // Tree:
    //      0
    //     / \
    //    1   2
    //   / \
    //  3   4
    
    TreeLCA tree(5);
    tree.add_edge(0, 1);
    tree.add_edge(0, 2);
    tree.add_edge(1, 3);
    tree.add_edge(1, 4);
    tree.build(0);
    
    std::vector<long long> values = {10, 20, 30, 40, 50};
    TreePathSum path_sum(tree, values);
    
    // Subtree 1 includes {1, 3, 4} -> 20 + 40 + 50 = 110
    EXPECT_EQ(path_sum.query_subtree(1), 110);
    
    // Subtree 0 includes all -> 10+20+30+40+50 = 150
    EXPECT_EQ(path_sum.query_subtree(0), 150);
    
    // Update leaf 4 -> 100 (delta +50)
    path_sum.set_value(4, 100);
    // New sum subtree 1: 20 + 40 + 100 = 160
    EXPECT_EQ(path_sum.query_subtree(1), 160);
}

TEST(TreeAdvancedTest, HLDPathDecomposition) {
    // Line graph: 0-1-2-3-4
    TreeLCA tree(5);
    for(int i=0; i<4; ++i) tree.add_edge(i, i+1);
    tree.build(0);
    
    std::vector<std::pair<int, int>> segments;
    tree.hld_decompose_path(0, 4, segments);
    
    // Should be a single segment since it's a heavy path (0 is root, 1 is heavy child of 0, etc.)
    // Wait, HLD logic depends on subtree sizes.
    // 0(sz=5) -> 1(sz=4) -> 2(sz=3) -> 3(sz=2) -> 4(sz=1)
    // All edges are heavy. So 1 segment.
    EXPECT_EQ(segments.size(), 1u);
}

TEST(TreeAdvancedTest, StressSubtreeAndPath) {
    int n = 50;
    TreeLCA tree(n);
    // Random tree
    std::mt19937 rng(42);
    for(int i=1; i<n; ++i) {
        std::uniform_int_distribution<int> dist(0, i-1);
        int p = dist(rng);
        tree.add_edge(p, i);
    }
    tree.build(0);
    
    std::vector<long long> values(n, 1);
    TreePathSum ds(tree, values);
    
    // 100 random ops
    for(int k=0; k<100; ++k) {
        int type = rng() % 3;
        if (type == 0) {
            // Update
            int u = std::uniform_int_distribution<int>(0, n-1)(rng);
            long long val = std::uniform_int_distribution<int>(0, 100)(rng);
            ds.set_value(u, val);
            values[u] = val;
        } else if (type == 1) {
            // Path Query
            int u = std::uniform_int_distribution<int>(0, n-1)(rng);
            int v = std::uniform_int_distribution<int>(0, n-1)(rng);
            
            long long expected = 0;
            int lca = tree.lca(u, v);
            
            // Walk up from u to lca
            int curr = u;
            while(curr != lca) {
                expected += values[curr];
                curr = tree.parent(curr);
            }
            expected += values[lca];
            
            // Walk up from v to lca
            curr = v;
            while(curr != lca) {
                expected += values[curr];
                curr = tree.parent(curr);
            }
            
            EXPECT_EQ(ds.query_path(u, v), expected);
        } else {
            // Subtree Query
            int u = std::uniform_int_distribution<int>(0, n-1)(rng);
            
            long long expected = 0;
            // BFS/DFS to find subtree sum
            std::vector<int> q = {u};
            int head = 0;
            while(head < q.size()) {
                int curr = q[head++];
                expected += values[curr];
                // Children? Need to check all nodes
                // Slow but correct for verification
            }
            // Actually, we can just iterate all nodes and check is_ancestor
            expected = 0;
            for(int i=0; i<n; ++i) {
                if (tree.is_ancestor(u, i)) {
                    expected += values[i];
                }
            }
            
            EXPECT_EQ(ds.query_subtree(u), expected);
        }
    }
}
