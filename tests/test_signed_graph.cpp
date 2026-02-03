#include <gtest/gtest.h>
#include "graphlib/signed_graph.h"
#include <vector>
#include <algorithm>

using namespace graphlib;

// =============================================================================
// Construction and basic operations
// =============================================================================

TEST(SignedGraphTest, ConstructionBasic) {
    SignedGraph sg(5, false);
    EXPECT_EQ(sg.vertex_count(), 5);
    EXPECT_FALSE(sg.is_directed());
    
    SignedGraph sg_dir(3, true);
    EXPECT_EQ(sg_dir.vertex_count(), 3);
    EXPECT_TRUE(sg_dir.is_directed());
}

TEST(SignedGraphTest, AddEdgeAndGetSign) {
    SignedGraph sg(4, false);
    
    sg.add_edge(0, 1, +1);
    sg.add_edge(1, 2, -1);
    sg.add_edge(2, 3, +1, 10);
    
    EXPECT_EQ(sg.get_sign(0, 1), 1);
    EXPECT_EQ(sg.get_sign(1, 0), 1);  // Undirected
    EXPECT_EQ(sg.get_sign(1, 2), -1);
    EXPECT_EQ(sg.get_sign(2, 1), -1);
    EXPECT_EQ(sg.get_sign(2, 3), 1);
    
    // Non-existent edge
    EXPECT_EQ(sg.get_sign(0, 3), 0);
    EXPECT_EQ(sg.get_sign(0, 2), 0);
}

TEST(SignedGraphTest, AddEdgeDirected) {
    SignedGraph sg(3, true);
    
    sg.add_edge(0, 1, +1);
    sg.add_edge(1, 2, -1);
    
    EXPECT_EQ(sg.get_sign(0, 1), 1);
    EXPECT_EQ(sg.get_sign(1, 2), -1);
    
    // Directed: reverse edge doesn't exist
    EXPECT_EQ(sg.get_sign(1, 0), 0);
    EXPECT_EQ(sg.get_sign(2, 1), 0);
}

// =============================================================================
// add_positive_edge, add_negative_edge
// =============================================================================

TEST(SignedGraphTest, AddPositiveNegativeEdge) {
    SignedGraph sg(4, false);
    
    sg.add_positive_edge(0, 1);
    sg.add_positive_edge(1, 2, 5);
    sg.add_negative_edge(2, 3);
    sg.add_negative_edge(0, 3, 10);
    
    EXPECT_EQ(sg.get_sign(0, 1), 1);
    EXPECT_EQ(sg.get_sign(1, 2), 1);
    EXPECT_EQ(sg.get_sign(2, 3), -1);
    EXPECT_EQ(sg.get_sign(0, 3), -1);
}

// =============================================================================
// is_balanced tests
// =============================================================================

TEST(SignedGraphTest, IsBalancedSimpleBalanced) {
    // Triangle with all positive edges is balanced
    // Can partition into {0,1,2} and {} (one set)
    SignedGraph sg(3, false);
    sg.add_positive_edge(0, 1);
    sg.add_positive_edge(1, 2);
    sg.add_positive_edge(0, 2);
    
    EXPECT_TRUE(sg.is_balanced());
}

TEST(SignedGraphTest, IsBalancedTwoGroups) {
    // Balanced graph: two groups connected by negative edges
    // Group A: {0, 1}, Group B: {2, 3}
    // Positive edges within groups, negative between groups
    SignedGraph sg(4, false);
    
    // Within group A
    sg.add_positive_edge(0, 1);
    // Within group B  
    sg.add_positive_edge(2, 3);
    // Between groups (negative)
    sg.add_negative_edge(0, 2);
    sg.add_negative_edge(0, 3);
    sg.add_negative_edge(1, 2);
    sg.add_negative_edge(1, 3);
    
    EXPECT_TRUE(sg.is_balanced());
}

TEST(SignedGraphTest, IsBalancedUnbalancedTriangle) {
    // Triangle with exactly one negative edge is unbalanced
    SignedGraph sg(3, false);
    sg.add_positive_edge(0, 1);
    sg.add_positive_edge(1, 2);
    sg.add_negative_edge(0, 2);
    
    EXPECT_FALSE(sg.is_balanced());
}

TEST(SignedGraphTest, IsBalancedTwoNegativeEdges) {
    // Triangle with exactly two negative edges is balanced
    // (even number of negative edges in cycle)
    SignedGraph sg(3, false);
    sg.add_positive_edge(0, 1);
    sg.add_negative_edge(1, 2);
    sg.add_negative_edge(0, 2);
    
    EXPECT_TRUE(sg.is_balanced());
}

TEST(SignedGraphTest, IsBalancedAllNegative) {
    // Triangle with all negative edges is unbalanced
    // (odd number of negative edges)
    SignedGraph sg(3, false);
    sg.add_negative_edge(0, 1);
    sg.add_negative_edge(1, 2);
    sg.add_negative_edge(0, 2);
    
    EXPECT_FALSE(sg.is_balanced());
}

TEST(SignedGraphTest, IsBalancedEmptyGraph) {
    SignedGraph sg(5, false);
    EXPECT_TRUE(sg.is_balanced());
}

TEST(SignedGraphTest, IsBalancedSingleEdge) {
    SignedGraph sg(2, false);
    sg.add_negative_edge(0, 1);
    EXPECT_TRUE(sg.is_balanced());  // No cycle, trivially balanced
}

// =============================================================================
// find_balance_partition tests
// =============================================================================

TEST(SignedGraphTest, FindBalancePartitionBalanced) {
    SignedGraph sg(4, false);
    
    // Two groups: {0,1} and {2,3}
    sg.add_positive_edge(0, 1);
    sg.add_positive_edge(2, 3);
    sg.add_negative_edge(0, 2);
    sg.add_negative_edge(1, 3);
    
    std::vector<int> partition;
    bool found = sg.find_balance_partition(partition);
    
    EXPECT_TRUE(found);
    EXPECT_EQ(partition.size(), 4);
    
    // Vertices 0 and 1 should be in same partition
    EXPECT_EQ(partition[0], partition[1]);
    // Vertices 2 and 3 should be in same partition
    EXPECT_EQ(partition[2], partition[3]);
    // Groups should be different
    EXPECT_NE(partition[0], partition[2]);
}

TEST(SignedGraphTest, FindBalancePartitionUnbalanced) {
    SignedGraph sg(3, false);
    sg.add_positive_edge(0, 1);
    sg.add_positive_edge(1, 2);
    sg.add_negative_edge(0, 2);
    
    std::vector<int> partition;
    bool found = sg.find_balance_partition(partition);
    
    EXPECT_FALSE(found);
}

TEST(SignedGraphTest, FindBalancePartitionAllPositive) {
    SignedGraph sg(4, false);
    sg.add_positive_edge(0, 1);
    sg.add_positive_edge(1, 2);
    sg.add_positive_edge(2, 3);
    sg.add_positive_edge(0, 3);
    
    std::vector<int> partition;
    bool found = sg.find_balance_partition(partition);
    
    EXPECT_TRUE(found);
    // All vertices should be in the same partition
    for (int i = 1; i < 4; ++i) {
        EXPECT_EQ(partition[0], partition[i]);
    }
}

// =============================================================================
// Subgraph extraction tests
// =============================================================================

TEST(SignedGraphTest, ToUnsignedGraph) {
    SignedGraph sg(4, false);
    sg.add_positive_edge(0, 1);
    sg.add_negative_edge(1, 2);
    sg.add_positive_edge(2, 3);
    
    Graph g = sg.to_unsigned_graph();
    
    EXPECT_EQ(g.vertex_count(), 4);
    // All 3 edges should be present (ignoring signs)
}

TEST(SignedGraphTest, PositiveSubgraph) {
    SignedGraph sg(4, false);
    sg.add_positive_edge(0, 1);
    sg.add_negative_edge(1, 2);
    sg.add_positive_edge(2, 3);
    sg.add_negative_edge(0, 3);
    
    Graph pos = sg.positive_subgraph();
    
    EXPECT_EQ(pos.vertex_count(), 4);
    // Only edges (0,1) and (2,3) should be in positive subgraph
}

TEST(SignedGraphTest, NegativeSubgraph) {
    SignedGraph sg(4, false);
    sg.add_positive_edge(0, 1);
    sg.add_negative_edge(1, 2);
    sg.add_positive_edge(2, 3);
    sg.add_negative_edge(0, 3);
    
    Graph neg = sg.negative_subgraph();
    
    EXPECT_EQ(neg.vertex_count(), 4);
    // Only edges (1,2) and (0,3) should be in negative subgraph
}

TEST(SignedGraphTest, SubgraphsEmpty) {
    SignedGraph sg(3, false);
    // Only positive edges
    sg.add_positive_edge(0, 1);
    sg.add_positive_edge(1, 2);
    
    Graph neg = sg.negative_subgraph();
    EXPECT_EQ(neg.vertex_count(), 3);
    // No negative edges
}

// =============================================================================
// count_signed_triangles tests
// =============================================================================

TEST(SignedGraphTest, CountSignedTrianglesAllPositive) {
    SignedGraph sg(3, false);
    sg.add_positive_edge(0, 1);
    sg.add_positive_edge(1, 2);
    sg.add_positive_edge(0, 2);
    
    auto [pos, neg] = sg.count_signed_triangles();
    
    // All positive triangle is balanced (positive count)
    EXPECT_EQ(pos, 1);
    EXPECT_EQ(neg, 0);
}

TEST(SignedGraphTest, CountSignedTrianglesMixed) {
    // Two triangles: (0,1,2) and (1,2,3)
    SignedGraph sg(4, false);
    
    // Triangle 0-1-2: all positive (balanced)
    sg.add_positive_edge(0, 1);
    sg.add_positive_edge(1, 2);
    sg.add_positive_edge(0, 2);
    
    // Triangle 1-2-3: one negative (unbalanced)
    sg.add_positive_edge(2, 3);
    sg.add_negative_edge(1, 3);
    
    auto [pos, neg] = sg.count_signed_triangles();
    
    // One balanced, one unbalanced
    EXPECT_GE(pos, 1);
}

TEST(SignedGraphTest, CountSignedTrianglesNoTriangle) {
    SignedGraph sg(4, false);
    sg.add_positive_edge(0, 1);
    sg.add_negative_edge(1, 2);
    sg.add_positive_edge(2, 3);
    // Path, no triangles
    
    auto [pos, neg] = sg.count_signed_triangles();
    
    EXPECT_EQ(pos, 0);
    EXPECT_EQ(neg, 0);
}

// =============================================================================
// make_signed_graph tests
// =============================================================================

TEST(SignedGraphTest, MakeSignedGraphBasic) {
    std::vector<std::pair<int, int>> trust = {{0, 1}, {1, 2}};
    std::vector<std::pair<int, int>> distrust = {{0, 2}};
    
    SignedGraph sg = make_signed_graph(3, trust, distrust);
    
    EXPECT_EQ(sg.vertex_count(), 3);
    EXPECT_EQ(sg.get_sign(0, 1), 1);
    EXPECT_EQ(sg.get_sign(1, 2), 1);
    EXPECT_EQ(sg.get_sign(0, 2), -1);
}

TEST(SignedGraphTest, MakeSignedGraphEmpty) {
    std::vector<std::pair<int, int>> trust = {};
    std::vector<std::pair<int, int>> distrust = {};
    
    SignedGraph sg = make_signed_graph(5, trust, distrust);
    
    EXPECT_EQ(sg.vertex_count(), 5);
    EXPECT_EQ(sg.get_sign(0, 1), 0);
}

TEST(SignedGraphTest, MakeSignedGraphOnlyTrust) {
    std::vector<std::pair<int, int>> trust = {{0, 1}, {1, 2}, {2, 0}};
    std::vector<std::pair<int, int>> distrust = {};
    
    SignedGraph sg = make_signed_graph(3, trust, distrust);
    
    EXPECT_EQ(sg.get_sign(0, 1), 1);
    EXPECT_EQ(sg.get_sign(1, 2), 1);
    EXPECT_EQ(sg.get_sign(2, 0), 1);
}

TEST(SignedGraphTest, MakeSignedGraphOnlyDistrust) {
    std::vector<std::pair<int, int>> trust = {};
    std::vector<std::pair<int, int>> distrust = {{0, 1}, {1, 2}};
    
    SignedGraph sg = make_signed_graph(3, trust, distrust);
    
    EXPECT_EQ(sg.get_sign(0, 1), -1);
    EXPECT_EQ(sg.get_sign(1, 2), -1);
    EXPECT_EQ(sg.get_sign(0, 2), 0);
}

// =============================================================================
// Move semantics test
// =============================================================================

TEST(SignedGraphTest, MoveConstruction) {
    SignedGraph sg1(4, false);
    sg1.add_positive_edge(0, 1);
    sg1.add_negative_edge(1, 2);
    
    SignedGraph sg2(std::move(sg1));
    
    EXPECT_EQ(sg2.vertex_count(), 4);
    EXPECT_EQ(sg2.get_sign(0, 1), 1);
    EXPECT_EQ(sg2.get_sign(1, 2), -1);
}

TEST(SignedGraphTest, MoveAssignment) {
    SignedGraph sg1(4, false);
    sg1.add_positive_edge(0, 1);
    
    SignedGraph sg2(2, true);
    sg2 = std::move(sg1);
    
    EXPECT_EQ(sg2.vertex_count(), 4);
    EXPECT_FALSE(sg2.is_directed());
    EXPECT_EQ(sg2.get_sign(0, 1), 1);
}
