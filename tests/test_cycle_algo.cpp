#include <gtest/gtest.h>
#include "graphlib/graph_core.h"
#include "graphlib/cycle_algo.h"
#include <limits>
#include <cmath>

using namespace graphlib;

class CycleAlgoTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(CycleAlgoTest, NoCycleDAG) {
    Graph g(3, true);
    g.add_edge(0, 1, 1);
    g.add_edge(1, 2, 1);
    
    EXPECT_EQ(minimum_mean_cycle(g), std::numeric_limits<double>::infinity());
}

TEST_F(CycleAlgoTest, SimpleCycle) {
    Graph g(3, true);
    g.add_edge(0, 1, 1);
    g.add_edge(1, 2, 2);
    g.add_edge(2, 0, 3);
    
    // Cycle 0-1-2-0 weight 1+2+3 = 6. Length 3. Mean 2.
    EXPECT_DOUBLE_EQ(minimum_mean_cycle(g), 2.0);
}

TEST_F(CycleAlgoTest, MultipleCycles) {
    Graph g(4, true);
    // Cycle 1: 0-1-0 weight 2, length 2 -> mean 1
    g.add_edge(0, 1, 1);
    g.add_edge(1, 0, 1);
    
    // Cycle 2: 2-3-2 weight 10, length 2 -> mean 5
    g.add_edge(2, 3, 5);
    g.add_edge(3, 2, 5);
    
    // Connect them
    g.add_edge(1, 2, 100);
    
    EXPECT_DOUBLE_EQ(minimum_mean_cycle(g), 1.0);
}

TEST_F(CycleAlgoTest, NegativeCycle) {
    Graph g(2, true);
    g.add_edge(0, 1, -2);
    g.add_edge(1, 0, -2);
    
    // Cycle 0-1-0 weight -4, length 2 -> mean -2
    EXPECT_DOUBLE_EQ(minimum_mean_cycle(g), -2.0);
}

TEST_F(CycleAlgoTest, DisconnectedGraph) {
    Graph g(4, true);
    // Component 1: No cycle
    g.add_edge(0, 1, 1);
    
    // Component 2: Cycle 2-3-2 weight 4, length 2 -> mean 2
    g.add_edge(2, 3, 2);
    g.add_edge(3, 2, 2);
    
    EXPECT_DOUBLE_EQ(minimum_mean_cycle(g), 2.0);
}

TEST_F(CycleAlgoTest, SelfLoop) {
    Graph g(1, true);
    g.add_edge(0, 0, 5);
    
    EXPECT_DOUBLE_EQ(minimum_mean_cycle(g), 5.0);
}

TEST_F(CycleAlgoTest, ComplexGraph) {
    Graph g(6, true);
    // 0 -> 1 -> 2 -> 0 (weight 1+1+1=3, mean 1)
    g.add_edge(0, 1, 1);
    g.add_edge(1, 2, 1);
    g.add_edge(2, 0, 1);
    
    // 3 -> 4 -> 5 -> 3 (weight 2+2+2=6, mean 2)
    g.add_edge(3, 4, 2);
    g.add_edge(4, 5, 2);
    g.add_edge(5, 3, 2);
    
    // Cross edges
    g.add_edge(2, 3, 10);
    g.add_edge(5, 0, 10);
    
    // The min cycle is 0-1-2-0 (mean 1)
    EXPECT_DOUBLE_EQ(minimum_mean_cycle(g), 1.0);
    
    // Add a better cycle: 1 -> 4 -> 1 (using existing nodes 1,4? No edge 1->4)
    // Add edge 1->4 weight -10. Edge 4->1 weight -10.
    // Cycle 1-4-1 weight -20, length 2, mean -10.
    g.add_edge(1, 4, -10);
    g.add_edge(4, 1, -10);
    
    EXPECT_DOUBLE_EQ(minimum_mean_cycle(g), -10.0);
}
