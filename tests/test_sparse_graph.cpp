#include <gtest/gtest.h>
#include <graphlib/sparse_graph.h>
#include <graphlib/graph_core.h>
#include <vector>
#include <tuple>
#include <algorithm>
#include <set>

using namespace graphlib;

class SparseGraphTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

// =============================================================================
// CSRGraph Tests
// =============================================================================

TEST_F(SparseGraphTest, CSRConstructionFromEdgeList) {
    std::vector<std::tuple<int, int, long long>> edges = {
        {0, 1, 10},
        {0, 2, 20},
        {1, 2, 30},
        {2, 3, 40}
    };
    
    CSRGraph csr(4, edges, true);
    
    EXPECT_EQ(csr.vertex_count(), 4);
    EXPECT_EQ(csr.edge_count(), 4);
    EXPECT_TRUE(csr.is_directed());
}

TEST_F(SparseGraphTest, CSRConstructionUndirected) {
    std::vector<std::tuple<int, int, long long>> edges = {
        {0, 1, 10},
        {1, 2, 20}
    };
    
    CSRGraph csr(3, edges, false);
    
    EXPECT_EQ(csr.vertex_count(), 3);
    EXPECT_EQ(csr.edge_count(), 2);
    EXPECT_FALSE(csr.is_directed());
}

TEST_F(SparseGraphTest, CSRConstructionFromGraph) {
    Graph g(4, true);
    g.add_edge(0, 1, 10);
    g.add_edge(0, 2, 20);
    g.add_edge(1, 3, 30);
    
    CSRGraph csr(g);
    
    EXPECT_EQ(csr.vertex_count(), 4);
    EXPECT_EQ(csr.edge_count(), 3);
    EXPECT_TRUE(csr.is_directed());
}

TEST_F(SparseGraphTest, CSRDegree) {
    std::vector<std::tuple<int, int, long long>> edges = {
        {0, 1, 1},
        {0, 2, 1},
        {0, 3, 1},
        {1, 2, 1}
    };
    
    CSRGraph csr(4, edges, true);
    
    EXPECT_EQ(csr.degree(0), 3);
    EXPECT_EQ(csr.degree(1), 1);
    EXPECT_EQ(csr.degree(2), 0);
    EXPECT_EQ(csr.degree(3), 0);
}

TEST_F(SparseGraphTest, CSRNeighbors) {
    std::vector<std::tuple<int, int, long long>> edges = {
        {0, 1, 10},
        {0, 2, 20},
        {0, 3, 30}
    };
    
    CSRGraph csr(4, edges, true);
    
    auto [begin, end] = csr.neighbors(0);
    std::set<int> neighbors(begin, end);
    
    EXPECT_EQ(neighbors.size(), 3u);
    EXPECT_TRUE(neighbors.count(1));
    EXPECT_TRUE(neighbors.count(2));
    EXPECT_TRUE(neighbors.count(3));
}

TEST_F(SparseGraphTest, CSRRowPtrColIdxValues) {
    std::vector<std::tuple<int, int, long long>> edges = {
        {0, 1, 10},
        {0, 2, 20},
        {1, 2, 30}
    };
    
    CSRGraph csr(3, edges, true);
    
    const auto& row_ptr = csr.row_ptr();
    const auto& col_idx = csr.col_idx();
    const auto& values = csr.values();
    
    EXPECT_EQ(row_ptr.size(), 4u);
    EXPECT_EQ(col_idx.size(), 3u);
    EXPECT_EQ(values.size(), 3u);
    
    // Vertex 0 has 2 outgoing edges
    EXPECT_EQ(row_ptr[1] - row_ptr[0], 2);
    // Vertex 1 has 1 outgoing edge
    EXPECT_EQ(row_ptr[2] - row_ptr[1], 1);
    // Vertex 2 has 0 outgoing edges
    EXPECT_EQ(row_ptr[3] - row_ptr[2], 0);
}

TEST_F(SparseGraphTest, CSRTranspose) {
    std::vector<std::tuple<int, int, long long>> edges = {
        {0, 1, 10},
        {0, 2, 20},
        {1, 2, 30}
    };
    
    CSRGraph csr(3, edges, true);
    CSRGraph transposed = csr.transpose();
    
    EXPECT_EQ(transposed.vertex_count(), 3);
    EXPECT_EQ(transposed.edge_count(), 3);
    
    // In transposed: 1->0, 2->0, 2->1
    EXPECT_EQ(transposed.degree(0), 0);
    EXPECT_EQ(transposed.degree(1), 1);
    EXPECT_EQ(transposed.degree(2), 2);
    
    EXPECT_TRUE(transposed.has_edge(1, 0));
    EXPECT_TRUE(transposed.has_edge(2, 0));
    EXPECT_TRUE(transposed.has_edge(2, 1));
}

TEST_F(SparseGraphTest, CSRHasEdge) {
    std::vector<std::tuple<int, int, long long>> edges = {
        {0, 1, 10},
        {0, 2, 20},
        {1, 2, 30}
    };
    
    CSRGraph csr(3, edges, true);
    
    EXPECT_TRUE(csr.has_edge(0, 1));
    EXPECT_TRUE(csr.has_edge(0, 2));
    EXPECT_TRUE(csr.has_edge(1, 2));
    EXPECT_FALSE(csr.has_edge(1, 0));
    EXPECT_FALSE(csr.has_edge(2, 0));
    EXPECT_FALSE(csr.has_edge(2, 1));
    EXPECT_FALSE(csr.has_edge(0, 0));
}

TEST_F(SparseGraphTest, CSRGetWeight) {
    std::vector<std::tuple<int, int, long long>> edges = {
        {0, 1, 10},
        {0, 2, 20},
        {1, 2, 30}
    };
    
    CSRGraph csr(3, edges, true);
    
    EXPECT_EQ(csr.get_weight(0, 1), 10);
    EXPECT_EQ(csr.get_weight(0, 2), 20);
    EXPECT_EQ(csr.get_weight(1, 2), 30);
    EXPECT_EQ(csr.get_weight(1, 0), 0);  // Non-existent edge
    EXPECT_EQ(csr.get_weight(2, 0), 0);  // Non-existent edge
}

TEST_F(SparseGraphTest, CSRToGraph) {
    std::vector<std::tuple<int, int, long long>> edges = {
        {0, 1, 10},
        {0, 2, 20},
        {1, 2, 30}
    };
    
    CSRGraph csr(3, edges, true);
    Graph g = csr.to_graph();
    
    EXPECT_EQ(g.vertex_count(), 3);
    EXPECT_TRUE(g.is_directed());
    
    // Check edges exist
    bool found_0_1 = false, found_0_2 = false, found_1_2 = false;
    for (Edge* e = g.get_edges(0); e; e = e->next) {
        if (e->to == 1 && e->weight == 10) found_0_1 = true;
        if (e->to == 2 && e->weight == 20) found_0_2 = true;
    }
    for (Edge* e = g.get_edges(1); e; e = e->next) {
        if (e->to == 2 && e->weight == 30) found_1_2 = true;
    }
    
    EXPECT_TRUE(found_0_1);
    EXPECT_TRUE(found_0_2);
    EXPECT_TRUE(found_1_2);
}

TEST_F(SparseGraphTest, CSREmptyGraph) {
    std::vector<std::tuple<int, int, long long>> edges;
    CSRGraph csr(5, edges, true);
    
    EXPECT_EQ(csr.vertex_count(), 5);
    EXPECT_EQ(csr.edge_count(), 0);
    
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(csr.degree(i), 0);
    }
}

TEST_F(SparseGraphTest, CSRSingleVertex) {
    std::vector<std::tuple<int, int, long long>> edges;
    CSRGraph csr(1, edges, true);
    
    EXPECT_EQ(csr.vertex_count(), 1);
    EXPECT_EQ(csr.edge_count(), 0);
    EXPECT_EQ(csr.degree(0), 0);
}

// =============================================================================
// CSCGraph Tests
// =============================================================================

TEST_F(SparseGraphTest, CSCConstructionFromEdgeList) {
    std::vector<std::tuple<int, int, long long>> edges = {
        {0, 1, 10},
        {0, 2, 20},
        {1, 2, 30},
        {2, 3, 40}
    };
    
    CSCGraph csc(4, edges, true);
    
    EXPECT_EQ(csc.vertex_count(), 4);
    EXPECT_EQ(csc.edge_count(), 4);
    EXPECT_TRUE(csc.is_directed());
}

TEST_F(SparseGraphTest, CSCConstructionFromGraph) {
    Graph g(4, true);
    g.add_edge(0, 1, 10);
    g.add_edge(0, 2, 20);
    g.add_edge(1, 2, 30);
    
    CSCGraph csc(g);
    
    EXPECT_EQ(csc.vertex_count(), 4);
    EXPECT_EQ(csc.edge_count(), 3);
}

TEST_F(SparseGraphTest, CSCConstructionFromCSR) {
    std::vector<std::tuple<int, int, long long>> edges = {
        {0, 1, 10},
        {0, 2, 20},
        {1, 2, 30}
    };
    
    CSRGraph csr(3, edges, true);
    CSCGraph csc(csr);
    
    EXPECT_EQ(csc.vertex_count(), 3);
    EXPECT_EQ(csc.edge_count(), 3);
}

TEST_F(SparseGraphTest, CSCInDegree) {
    std::vector<std::tuple<int, int, long long>> edges = {
        {0, 2, 1},
        {1, 2, 1},
        {3, 2, 1},
        {0, 1, 1}
    };
    
    CSCGraph csc(4, edges, true);
    
    EXPECT_EQ(csc.in_degree(0), 0);
    EXPECT_EQ(csc.in_degree(1), 1);
    EXPECT_EQ(csc.in_degree(2), 3);
    EXPECT_EQ(csc.in_degree(3), 0);
}

TEST_F(SparseGraphTest, CSCPredecessors) {
    std::vector<std::tuple<int, int, long long>> edges = {
        {0, 2, 10},
        {1, 2, 20},
        {3, 2, 30}
    };
    
    CSCGraph csc(4, edges, true);
    
    auto [begin, end] = csc.predecessors(2);
    std::set<int> preds(begin, end);
    
    EXPECT_EQ(preds.size(), 3u);
    EXPECT_TRUE(preds.count(0));
    EXPECT_TRUE(preds.count(1));
    EXPECT_TRUE(preds.count(3));
}

TEST_F(SparseGraphTest, CSCToGraph) {
    std::vector<std::tuple<int, int, long long>> edges = {
        {0, 1, 10},
        {1, 2, 20}
    };
    
    CSCGraph csc(3, edges, true);
    Graph g = csc.to_graph();
    
    EXPECT_EQ(g.vertex_count(), 3);
    EXPECT_TRUE(g.is_directed());
}

TEST_F(SparseGraphTest, CSCToCSR) {
    std::vector<std::tuple<int, int, long long>> edges = {
        {0, 1, 10},
        {0, 2, 20},
        {1, 2, 30}
    };
    
    CSCGraph csc(3, edges, true);
    CSRGraph csr = csc.to_csr();
    
    EXPECT_EQ(csr.vertex_count(), 3);
    EXPECT_EQ(csr.edge_count(), 3);
    EXPECT_TRUE(csr.has_edge(0, 1));
    EXPECT_TRUE(csr.has_edge(0, 2));
    EXPECT_TRUE(csr.has_edge(1, 2));
}

// =============================================================================
// COOGraph Tests
// =============================================================================

TEST_F(SparseGraphTest, COOConstructionEmpty) {
    COOGraph coo(5, true);
    
    EXPECT_EQ(coo.vertex_count(), 5);
    EXPECT_EQ(coo.edge_count(), 0);
    EXPECT_TRUE(coo.is_directed());
}

TEST_F(SparseGraphTest, COOAddEdge) {
    COOGraph coo(4, true);
    
    coo.add_edge(0, 1, 10);
    coo.add_edge(0, 2, 20);
    coo.add_edge(1, 3, 30);
    
    EXPECT_EQ(coo.edge_count(), 3);
    
    const auto& row = coo.row();
    const auto& col = coo.col();
    const auto& data = coo.data();
    
    EXPECT_EQ(row.size(), 3u);
    EXPECT_EQ(col.size(), 3u);
    EXPECT_EQ(data.size(), 3u);
}

TEST_F(SparseGraphTest, COOReserve) {
    COOGraph coo(10, true);
    coo.reserve(100);
    
    // After reserve, we can add edges without reallocation
    for (int i = 0; i < 50; ++i) {
        coo.add_edge(i % 10, (i + 1) % 10, i);
    }
    
    EXPECT_EQ(coo.edge_count(), 50);
}

TEST_F(SparseGraphTest, COOConstructionFromGraph) {
    Graph g(3, true);
    g.add_edge(0, 1, 10);
    g.add_edge(1, 2, 20);
    
    COOGraph coo(g);
    
    EXPECT_EQ(coo.vertex_count(), 3);
    EXPECT_EQ(coo.edge_count(), 2);
}

TEST_F(SparseGraphTest, COOConstructionFromCSR) {
    std::vector<std::tuple<int, int, long long>> edges = {
        {0, 1, 10},
        {0, 2, 20},
        {1, 2, 30}
    };
    
    CSRGraph csr(3, edges, true);
    COOGraph coo(csr);
    
    EXPECT_EQ(coo.vertex_count(), 3);
    EXPECT_EQ(coo.edge_count(), 3);
}

TEST_F(SparseGraphTest, COOSort) {
    COOGraph coo(3, true);
    
    // Add edges in non-sorted order
    coo.add_edge(2, 0, 30);
    coo.add_edge(0, 1, 10);
    coo.add_edge(1, 2, 20);
    coo.add_edge(0, 2, 15);
    
    coo.sort();
    
    const auto& row = coo.row();
    const auto& col = coo.col();
    
    // After sort, edges should be ordered by (row, col)
    bool sorted = true;
    for (size_t i = 1; i < row.size(); ++i) {
        if (row[i] < row[i-1] || (row[i] == row[i-1] && col[i] < col[i-1])) {
            sorted = false;
            break;
        }
    }
    EXPECT_TRUE(sorted);
}

TEST_F(SparseGraphTest, COORemoveDuplicates) {
    COOGraph coo(3, true);
    
    coo.add_edge(0, 1, 10);
    coo.add_edge(0, 1, 20);  // Duplicate
    coo.add_edge(0, 1, 30);  // Duplicate
    coo.add_edge(1, 2, 40);
    
    EXPECT_EQ(coo.edge_count(), 4);
    
    coo.sort();
    coo.remove_duplicates();
    
    EXPECT_EQ(coo.edge_count(), 2);
}

TEST_F(SparseGraphTest, COOToCSR) {
    COOGraph coo(3, true);
    coo.add_edge(0, 1, 10);
    coo.add_edge(0, 2, 20);
    coo.add_edge(1, 2, 30);
    
    CSRGraph csr = coo.to_csr();
    
    EXPECT_EQ(csr.vertex_count(), 3);
    EXPECT_EQ(csr.edge_count(), 3);
    EXPECT_TRUE(csr.has_edge(0, 1));
    EXPECT_TRUE(csr.has_edge(0, 2));
    EXPECT_TRUE(csr.has_edge(1, 2));
}

TEST_F(SparseGraphTest, COOToCSC) {
    COOGraph coo(3, true);
    coo.add_edge(0, 1, 10);
    coo.add_edge(0, 2, 20);
    coo.add_edge(1, 2, 30);
    
    CSCGraph csc = coo.to_csc();
    
    EXPECT_EQ(csc.vertex_count(), 3);
    EXPECT_EQ(csc.edge_count(), 3);
    EXPECT_EQ(csc.in_degree(1), 1);
    EXPECT_EQ(csc.in_degree(2), 2);
}

TEST_F(SparseGraphTest, COOToGraph) {
    COOGraph coo(3, true);
    coo.add_edge(0, 1, 10);
    coo.add_edge(1, 2, 20);
    
    Graph g = coo.to_graph();
    
    EXPECT_EQ(g.vertex_count(), 3);
    EXPECT_TRUE(g.is_directed());
}

// =============================================================================
// Conversion Functions Tests
// =============================================================================

TEST_F(SparseGraphTest, GraphToCSR) {
    Graph g(4, true);
    g.add_edge(0, 1, 10);
    g.add_edge(0, 2, 20);
    g.add_edge(1, 3, 30);
    
    CSRGraph csr = graph_to_csr(g);
    
    EXPECT_EQ(csr.vertex_count(), 4);
    EXPECT_EQ(csr.edge_count(), 3);
    EXPECT_TRUE(csr.has_edge(0, 1));
    EXPECT_TRUE(csr.has_edge(0, 2));
    EXPECT_TRUE(csr.has_edge(1, 3));
}

TEST_F(SparseGraphTest, GraphToCSC) {
    Graph g(4, true);
    g.add_edge(0, 1, 10);
    g.add_edge(0, 2, 20);
    g.add_edge(1, 2, 30);
    
    CSCGraph csc = graph_to_csc(g);
    
    EXPECT_EQ(csc.vertex_count(), 4);
    EXPECT_EQ(csc.edge_count(), 3);
    EXPECT_EQ(csc.in_degree(1), 1);
    EXPECT_EQ(csc.in_degree(2), 2);
}

TEST_F(SparseGraphTest, GraphToCOO) {
    Graph g(3, true);
    g.add_edge(0, 1, 10);
    g.add_edge(1, 2, 20);
    
    COOGraph coo = graph_to_coo(g);
    
    EXPECT_EQ(coo.vertex_count(), 3);
    EXPECT_EQ(coo.edge_count(), 2);
}

// =============================================================================
// Round-trip Conversion Tests
// =============================================================================

TEST_F(SparseGraphTest, CSRToGraphToCSR) {
    std::vector<std::tuple<int, int, long long>> edges = {
        {0, 1, 10},
        {0, 2, 20},
        {1, 2, 30},
        {2, 3, 40}
    };
    
    CSRGraph csr1(4, edges, true);
    Graph g = csr1.to_graph();
    CSRGraph csr2(g);
    
    EXPECT_EQ(csr2.vertex_count(), csr1.vertex_count());
    EXPECT_EQ(csr2.edge_count(), csr1.edge_count());
    
    // Check all edges preserved
    for (const auto& [u, v, w] : edges) {
        EXPECT_TRUE(csr2.has_edge(u, v));
        EXPECT_EQ(csr2.get_weight(u, v), w);
    }
}

TEST_F(SparseGraphTest, CSRToCSCToCSR) {
    std::vector<std::tuple<int, int, long long>> edges = {
        {0, 1, 10},
        {0, 2, 20},
        {1, 2, 30}
    };
    
    CSRGraph csr1(3, edges, true);
    CSCGraph csc(csr1);
    CSRGraph csr2 = csc.to_csr();
    
    EXPECT_EQ(csr2.vertex_count(), csr1.vertex_count());
    EXPECT_EQ(csr2.edge_count(), csr1.edge_count());
    
    for (const auto& [u, v, w] : edges) {
        EXPECT_TRUE(csr2.has_edge(u, v));
    }
}

TEST_F(SparseGraphTest, COOToCSRToCOO) {
    COOGraph coo1(3, true);
    coo1.add_edge(0, 1, 10);
    coo1.add_edge(0, 2, 20);
    coo1.add_edge(1, 2, 30);
    
    CSRGraph csr = coo1.to_csr();
    COOGraph coo2(csr);
    
    EXPECT_EQ(coo2.vertex_count(), coo1.vertex_count());
    EXPECT_EQ(coo2.edge_count(), coo1.edge_count());
}

// =============================================================================
// Edge Cases
// =============================================================================

TEST_F(SparseGraphTest, SelfLoops) {
    std::vector<std::tuple<int, int, long long>> edges = {
        {0, 0, 10},  // Self-loop
        {0, 1, 20},
        {1, 1, 30}   // Self-loop
    };
    
    CSRGraph csr(2, edges, true);
    
    EXPECT_EQ(csr.edge_count(), 3);
    EXPECT_TRUE(csr.has_edge(0, 0));
    EXPECT_TRUE(csr.has_edge(1, 1));
    EXPECT_EQ(csr.get_weight(0, 0), 10);
    EXPECT_EQ(csr.get_weight(1, 1), 30);
}

TEST_F(SparseGraphTest, LargeWeights) {
    std::vector<std::tuple<int, int, long long>> edges = {
        {0, 1, 1000000000000LL},
        {1, 2, -1000000000000LL}
    };
    
    CSRGraph csr(3, edges, true);
    
    EXPECT_EQ(csr.get_weight(0, 1), 1000000000000LL);
    EXPECT_EQ(csr.get_weight(1, 2), -1000000000000LL);
}

TEST_F(SparseGraphTest, IsolatedVertices) {
    std::vector<std::tuple<int, int, long long>> edges = {
        {0, 1, 10}
    };
    
    CSRGraph csr(5, edges, true);  // Vertices 2, 3, 4 are isolated
    
    EXPECT_EQ(csr.vertex_count(), 5);
    EXPECT_EQ(csr.degree(0), 1);
    EXPECT_EQ(csr.degree(1), 0);
    EXPECT_EQ(csr.degree(2), 0);
    EXPECT_EQ(csr.degree(3), 0);
    EXPECT_EQ(csr.degree(4), 0);
}

TEST_F(SparseGraphTest, DenseGraph) {
    // Create a complete graph K4
    std::vector<std::tuple<int, int, long long>> edges;
    int n = 4;
    long long w = 1;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                edges.push_back({i, j, w++});
            }
        }
    }
    
    CSRGraph csr(n, edges, true);
    
    EXPECT_EQ(csr.vertex_count(), 4);
    EXPECT_EQ(csr.edge_count(), 12);  // 4 * 3 = 12 directed edges
    
    for (int i = 0; i < n; ++i) {
        EXPECT_EQ(csr.degree(i), 3);
    }
}

TEST_F(SparseGraphTest, UndirectedCSRNeighbors) {
    std::vector<std::tuple<int, int, long long>> edges = {
        {0, 1, 10},
        {1, 2, 20}
    };
    
    CSRGraph csr(3, edges, false);  // Undirected
    
    // In undirected graph, both directions should be stored
    auto [begin0, end0] = csr.neighbors(0);
    std::set<int> neighbors0(begin0, end0);
    EXPECT_TRUE(neighbors0.count(1));
    
    auto [begin1, end1] = csr.neighbors(1);
    std::set<int> neighbors1(begin1, end1);
    EXPECT_TRUE(neighbors1.count(0));
    EXPECT_TRUE(neighbors1.count(2));
    
    auto [begin2, end2] = csr.neighbors(2);
    std::set<int> neighbors2(begin2, end2);
    EXPECT_TRUE(neighbors2.count(1));
}
