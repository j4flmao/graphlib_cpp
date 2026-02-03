#include <gtest/gtest.h>
#include "graphlib/cycle_basis.h"
#include "graphlib/graph_core.h"
#include <set>
#include <algorithm>

using namespace graphlib;

class CycleBasisTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// ============================================================================
// minimum_cycle_basis tests
// ============================================================================

TEST_F(CycleBasisTest, MinimumCycleBasisSimpleCycle) {
    // Simple cycle: 0-1-2-0
    Graph g(3);
    g.add_edge(0, 1, 1); g.add_edge(1, 0, 1);
    g.add_edge(1, 2, 1); g.add_edge(2, 1, 1);
    g.add_edge(2, 0, 1); g.add_edge(0, 2, 1);
    
    auto basis = minimum_cycle_basis(g);
    EXPECT_EQ(basis.size(), 1u);
    EXPECT_EQ(basis[0].edges.size(), 3u);
    EXPECT_EQ(basis[0].weight, 3);
}

TEST_F(CycleBasisTest, MinimumCycleBasisSquare) {
    // Square: 0-1-2-3-0
    Graph g(4);
    g.add_edge(0, 1, 1); g.add_edge(1, 0, 1);
    g.add_edge(1, 2, 1); g.add_edge(2, 1, 1);
    g.add_edge(2, 3, 1); g.add_edge(3, 2, 1);
    g.add_edge(3, 0, 1); g.add_edge(0, 3, 1);
    
    auto basis = minimum_cycle_basis(g);
    EXPECT_EQ(basis.size(), 1u);
    EXPECT_EQ(basis[0].edges.size(), 4u);
}

TEST_F(CycleBasisTest, MinimumCycleBasisTwoTriangles) {
    // Two triangles sharing an edge: bowtie
    // Triangle 1: 0-1-2-0
    // Triangle 2: 1-2-3-1
    Graph g(4);
    g.add_edge(0, 1, 1); g.add_edge(1, 0, 1);
    g.add_edge(1, 2, 1); g.add_edge(2, 1, 1);
    g.add_edge(2, 0, 1); g.add_edge(0, 2, 1);
    g.add_edge(2, 3, 1); g.add_edge(3, 2, 1);
    g.add_edge(3, 1, 1); g.add_edge(1, 3, 1);
    
    auto basis = minimum_cycle_basis(g);
    EXPECT_EQ(basis.size(), 2u);
}

TEST_F(CycleBasisTest, MinimumCycleBasisNoCycle) {
    // Tree: no cycles
    Graph g(4);
    g.add_edge(0, 1, 1); g.add_edge(1, 0, 1);
    g.add_edge(0, 2, 1); g.add_edge(2, 0, 1);
    g.add_edge(0, 3, 1); g.add_edge(3, 0, 1);
    
    auto basis = minimum_cycle_basis(g);
    EXPECT_EQ(basis.size(), 0u);
}

// ============================================================================
// fundamental_cycles tests
// ============================================================================

TEST_F(CycleBasisTest, FundamentalCyclesSimpleCycle) {
    // Cycle: 0-1-2-0
    Graph g(3);
    g.add_edge(0, 1, 1); g.add_edge(1, 0, 1);
    g.add_edge(1, 2, 1); g.add_edge(2, 1, 1);
    g.add_edge(2, 0, 1); g.add_edge(0, 2, 1);
    
    auto cycles = fundamental_cycles(g);
    EXPECT_EQ(cycles.size(), 1u);
}

TEST_F(CycleBasisTest, FundamentalCyclesK4) {
    // K4 has m - n + 1 = 6 - 4 + 1 = 3 fundamental cycles
    Graph g(4);
    for (int i = 0; i < 4; ++i) {
        for (int j = i + 1; j < 4; ++j) {
            g.add_edge(i, j, 1);
            g.add_edge(j, i, 1);
        }
    }
    
    auto cycles = fundamental_cycles(g);
    EXPECT_EQ(cycles.size(), 3u);
}

TEST_F(CycleBasisTest, FundamentalCyclesTree) {
    // Tree has no cycles
    Graph g(5);
    g.add_edge(0, 1, 1); g.add_edge(1, 0, 1);
    g.add_edge(0, 2, 1); g.add_edge(2, 0, 1);
    g.add_edge(1, 3, 1); g.add_edge(3, 1, 1);
    g.add_edge(1, 4, 1); g.add_edge(4, 1, 1);
    
    auto cycles = fundamental_cycles(g);
    EXPECT_EQ(cycles.size(), 0u);
}

// ============================================================================
// girth tests
// ============================================================================

TEST_F(CycleBasisTest, GirthK3) {
    // K3 has girth 3
    Graph g(3);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    
    EXPECT_EQ(girth(g), 3);
}

TEST_F(CycleBasisTest, GirthK4) {
    // K4 has girth 3 (triangles)
    Graph g(4);
    for (int i = 0; i < 4; ++i) {
        for (int j = i + 1; j < 4; ++j) {
            g.add_edge(i, j);
            g.add_edge(j, i);
        }
    }
    
    EXPECT_EQ(girth(g), 3);
}

TEST_F(CycleBasisTest, GirthSquare) {
    // Square (cycle of 4) has girth 4
    Graph g(4);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 3); g.add_edge(3, 2);
    g.add_edge(3, 0); g.add_edge(0, 3);
    
    EXPECT_EQ(girth(g), 4);
}

TEST_F(CycleBasisTest, GirthPath) {
    // Path (no cycle) has girth -1
    Graph g(4);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 3); g.add_edge(3, 2);
    
    EXPECT_EQ(girth(g), -1);
}

TEST_F(CycleBasisTest, GirthTree) {
    // Tree has no cycle
    Graph g(5);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(0, 2); g.add_edge(2, 0);
    g.add_edge(1, 3); g.add_edge(3, 1);
    g.add_edge(1, 4); g.add_edge(4, 1);
    
    EXPECT_EQ(girth(g), -1);
}

TEST_F(CycleBasisTest, GirthPentagon) {
    // Pentagon (cycle of 5) has girth 5
    Graph g(5);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 3); g.add_edge(3, 2);
    g.add_edge(3, 4); g.add_edge(4, 3);
    g.add_edge(4, 0); g.add_edge(0, 4);
    
    EXPECT_EQ(girth(g), 5);
}

// ============================================================================
// find_all_cycles tests
// ============================================================================

TEST_F(CycleBasisTest, FindAllCyclesTriangle) {
    Graph g(3);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    
    auto cycles = find_all_cycles(g);
    EXPECT_GE(cycles.size(), 1u);
}

TEST_F(CycleBasisTest, FindAllCyclesNoCycle) {
    // Tree
    Graph g(4);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(0, 2); g.add_edge(2, 0);
    g.add_edge(0, 3); g.add_edge(3, 0);
    
    auto cycles = find_all_cycles(g);
    EXPECT_EQ(cycles.size(), 0u);
}

TEST_F(CycleBasisTest, FindAllCyclesWithLimit) {
    // K4 has multiple cycles, limit the output
    Graph g(4);
    for (int i = 0; i < 4; ++i) {
        for (int j = i + 1; j < 4; ++j) {
            g.add_edge(i, j);
            g.add_edge(j, i);
        }
    }
    
    auto cycles = find_all_cycles(g, 2);
    EXPECT_LE(cycles.size(), 2u);
}

TEST_F(CycleBasisTest, FindAllCyclesSquare) {
    Graph g(4);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 3); g.add_edge(3, 2);
    g.add_edge(3, 0); g.add_edge(0, 3);
    
    auto cycles = find_all_cycles(g);
    EXPECT_GE(cycles.size(), 1u);
}

// ============================================================================
// longest_cycle tests
// ============================================================================

TEST_F(CycleBasisTest, LongestCycleTriangle) {
    Graph g(3);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    
    auto cycle = longest_cycle(g);
    EXPECT_EQ(cycle.size(), 3u);
}

TEST_F(CycleBasisTest, LongestCycleSquare) {
    Graph g(4);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 3); g.add_edge(3, 2);
    g.add_edge(3, 0); g.add_edge(0, 3);
    
    auto cycle = longest_cycle(g);
    EXPECT_EQ(cycle.size(), 4u);
}

TEST_F(CycleBasisTest, LongestCycleNoCycle) {
    // Tree has no cycle
    Graph g(4);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(0, 2); g.add_edge(2, 0);
    g.add_edge(0, 3); g.add_edge(3, 0);
    
    auto cycle = longest_cycle(g);
    EXPECT_EQ(cycle.size(), 0u);
}

TEST_F(CycleBasisTest, LongestCycleK4) {
    // K4: Hamiltonian cycle has length 4
    Graph g(4);
    for (int i = 0; i < 4; ++i) {
        for (int j = i + 1; j < 4; ++j) {
            g.add_edge(i, j);
            g.add_edge(j, i);
        }
    }
    
    auto cycle = longest_cycle(g);
    EXPECT_EQ(cycle.size(), 4u);
}

TEST_F(CycleBasisTest, LongestCyclePentagon) {
    Graph g(5);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 3); g.add_edge(3, 2);
    g.add_edge(3, 4); g.add_edge(4, 3);
    g.add_edge(4, 0); g.add_edge(0, 4);
    
    auto cycle = longest_cycle(g);
    EXPECT_EQ(cycle.size(), 5u);
}
