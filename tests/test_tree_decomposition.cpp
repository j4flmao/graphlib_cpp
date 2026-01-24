#include <gtest/gtest.h>
#include <graphlib/graphlib.h>
#include <graphlib/tree_decomposition.h>
#include <vector>
#include <set>
#include <algorithm>

using namespace graphlib;

// Helper to check tree decomposition validity
void CheckTreeDecomposition(const Graph& g, const TreeDecompositionResult& result) {
    int n = g.vertex_count();
    const Graph& tree = result.tree;
    const std::vector<std::vector<int>>& bags = result.bags;

    // 1. Union of bags is V
    std::vector<bool> covered(n, false);
    for (const auto& bag : bags) {
        for (int u : bag) {
            if (u >= 0 && u < n) covered[u] = true;
        }
    }
    for (int i = 0; i < n; ++i) {
        EXPECT_TRUE(covered[i]) << "Vertex " << i << " is not in any bag.";
    }

    // 2. Every edge is in some bag
    for (int u = 0; u < n; ++u) {
        Edge* e = g.get_edges(u);
        while (e) {
            int v = e->to;
            if (u < v) { // Check each edge once
                bool found = false;
                for (const auto& bag : bags) {
                    bool has_u = false, has_v = false;
                    for (int x : bag) {
                        if (x == u) has_u = true;
                        if (x == v) has_v = true;
                    }
                    if (has_u && has_v) {
                        found = true;
                        break;
                    }
                }
                EXPECT_TRUE(found) << "Edge (" << u << ", " << v << ") is not in any bag.";
            }
            e = e->next;
        }
    }

    // 3. Connectedness condition
    // For each vertex v, the set of nodes {i | v in bags[i]} must form a connected subtree in 'tree'.
    for (int v = 0; v < n; ++v) {
        std::vector<int> nodes_containing_v;
        for (int i = 0; i < n; ++i) {
            for (int x : bags[i]) {
                if (x == v) {
                    nodes_containing_v.push_back(i);
                    break;
                }
            }
        }

        if (nodes_containing_v.empty()) continue;

        // Run BFS/DFS on the tree, restricted to nodes_containing_v
        std::set<int> valid_nodes(nodes_containing_v.begin(), nodes_containing_v.end());
        std::set<int> visited;
        std::vector<int> q;
        q.push_back(nodes_containing_v[0]);
        visited.insert(nodes_containing_v[0]);

        int head = 0;
        while(head < (int)q.size()){
            int curr = q[head++];
            Edge* e = tree.get_edges(curr);
            while(e){
                int next = e->to;
                if(valid_nodes.count(next) && visited.find(next) == visited.end()){
                    visited.insert(next);
                    q.push_back(next);
                }
                e = e->next;
            }
        }

        EXPECT_EQ(visited.size(), valid_nodes.size()) << "Bags containing vertex " << v << " do not form a connected subtree.";
    }
}

TEST(TreeDecompositionTest, PathGraph) {
    int n = 5;
    Graph g(n, false);
    for(int i=0; i<n-1; ++i) g.add_edge(i, i+1);

    TreeDecompositionResult res = tree_decomposition_min_degree(g);
    EXPECT_LE(res.width, 1); // Path has treewidth 1
    CheckTreeDecomposition(g, res);
}

TEST(TreeDecompositionTest, CycleGraph) {
    int n = 5;
    Graph g(n, false);
    for(int i=0; i<n-1; ++i) g.add_edge(i, i+1);
    g.add_edge(n-1, 0);

    TreeDecompositionResult res = tree_decomposition_min_degree(g);
    EXPECT_LE(res.width, 2); // Cycle has treewidth 2
    CheckTreeDecomposition(g, res);
}

TEST(TreeDecompositionTest, CompleteGraph) {
    int n = 5;
    Graph g(n, false);
    for(int i=0; i<n; ++i) {
        for(int j=i+1; j<n; ++j) {
            g.add_edge(i, j);
        }
    }

    TreeDecompositionResult res = tree_decomposition_min_degree(g);
    EXPECT_EQ(res.width, 4); // K5 has treewidth 4
    CheckTreeDecomposition(g, res);
}

TEST(TreeDecompositionTest, Grid2x3) {
    // 0-1-2
    // | | |
    // 3-4-5
    Graph g(6, false);
    g.add_edge(0, 1); g.add_edge(1, 2);
    g.add_edge(3, 4); g.add_edge(4, 5);
    g.add_edge(0, 3); g.add_edge(1, 4); g.add_edge(2, 5);

    TreeDecompositionResult res = tree_decomposition_min_degree(g);
    // Treewidth of 2x3 grid is min(2, 3) = 2.
    // Min-degree heuristic should find it or close to it.
    EXPECT_LE(res.width, 3); // 2 is optimal, 3 is acceptable for heuristic
    CheckTreeDecomposition(g, res);
}
