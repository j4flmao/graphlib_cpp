#include <gtest/gtest.h>
#include "graphlib/max_flow.h"
#include "graphlib/graph_core.h"
#include <algorithm>
#include <cmath>

using namespace graphlib;

TEST(MaximumDensitySubgraphTest, CompleteGraph) {
    int n = 5;
    Graph g(n, false); // Undirected
    // K5: 10 edges.
    for(int i=0; i<n; ++i) {
        for(int j=i+1; j<n; ++j) {
            g.add_edge(i, j, 1);
        }
    }
    
    auto result = maximum_density_subgraph(g);
    // Density = 10 / 5 = 2.0.
    EXPECT_NEAR(result.first, 2.0, 1e-4);
    EXPECT_EQ(result.second.size(), 5);
}

TEST(MaximumDensitySubgraphTest, CliqueAndIsolated) {
    int n = 10;
    Graph g(n, false);
    // K4 on 0,1,2,3. Edges = 6. Density = 6/4 = 1.5.
    for(int i=0; i<4; ++i) {
        for(int j=i+1; j<4; ++j) {
            g.add_edge(i, j, 1);
        }
    }
    // Isolated 4..9
    
    auto result = maximum_density_subgraph(g);
    EXPECT_NEAR(result.first, 1.5, 1e-4);
    
    // Should select 0,1,2,3
    EXPECT_EQ(result.second.size(), 4);
    std::vector<int> expected = {0, 1, 2, 3};
    std::vector<int> actual = result.second;
    std::sort(actual.begin(), actual.end());
    EXPECT_EQ(actual, expected);
}

TEST(MaximumDensitySubgraphTest, StarGraph) {
    int n = 5; // Center 0, leaves 1,2,3,4
    Graph g(n, false);
    for(int i=1; i<n; ++i) {
        g.add_edge(0, i, 1);
    }
    
    // Max density is (n-1)/n = 4/5 = 0.8.
    auto result = maximum_density_subgraph(g);
    EXPECT_NEAR(result.first, 0.8, 1e-4);
    EXPECT_EQ(result.second.size(), 5);
}

TEST(MaximumDensitySubgraphTest, TwoCliques) {
    // K3 (density 1.0) and K4 (density 1.5) disconnected
    Graph g(7, false);
    
    // K3 on 0,1,2
    g.add_edge(0, 1, 1); g.add_edge(1, 2, 1); g.add_edge(0, 2, 1);
    
    // K4 on 3,4,5,6
    for(int i=3; i<7; ++i) {
        for(int j=i+1; j<7; ++j) {
            g.add_edge(i, j, 1);
        }
    }
    
    auto result = maximum_density_subgraph(g);
    EXPECT_NEAR(result.first, 1.5, 1e-4);
    EXPECT_EQ(result.second.size(), 4);
    
    std::vector<int> expected = {3, 4, 5, 6};
    std::vector<int> actual = result.second;
    std::sort(actual.begin(), actual.end());
    EXPECT_EQ(actual, expected);
}

TEST(MaximumDensitySubgraphTest, EmptyGraph) {
    Graph g(5, false);
    auto result = maximum_density_subgraph(g);
    EXPECT_NEAR(result.first, 0.0, 1e-4);
    // Vertices could be empty or any single vertex (density 0).
    // The algo typically returns empty or all for 0?
    // 0 edges. flow is always 0. min cut is 0.
    // L stays 0.
    // It should handle gracefully.
}
