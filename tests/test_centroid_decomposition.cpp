#include "graphlib/graphlib.h"
#include "graphlib/centroid_decomposition.h"
#include <gtest/gtest.h>
#include <vector>
#include <queue>

using namespace graphlib;

class CentroidDecompositionTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

TEST_F(CentroidDecompositionTest, LineGraph) {
    // 0-1-2-3-4
    int n = 5;
    Graph g(n);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 3); g.add_edge(3, 2);
    g.add_edge(3, 4); g.add_edge(4, 3);

    CentroidDecomposition cd(g);

    int root = cd.get_root();
    // For a line graph, centroid is middle.
    // 0-1-2-3-4 -> 2 is centroid.
    EXPECT_EQ(root, 2);
    EXPECT_EQ(cd.get_parent(2), -1);
    EXPECT_EQ(cd.get_level(2), 0);

    // Neighbors of 2 (1 and 3) become children in centroid tree (level 1)
    // 1 (size 2: 0-1) -> centroid 0 or 1. If 1: children 0.
    // 3 (size 2: 3-4) -> centroid 3 or 4. If 3: children 4.
    
    int p1 = cd.get_parent(1);
    int p3 = cd.get_parent(3);
    EXPECT_TRUE(p1 == 2 || cd.get_parent(p1) == 2); // 1 might be child of 2 or grandchild
    EXPECT_TRUE(p3 == 2 || cd.get_parent(p3) == 2);
    
    // Check tree properties
    // Max level should be O(log N)
    for(int i=0; i<n; ++i) {
        EXPECT_LE(cd.get_level(i), 3);
    }
}

TEST_F(CentroidDecompositionTest, StarGraph) {
    // 0 connected to 1, 2, 3, 4
    int n = 5;
    Graph g(n);
    for(int i=1; i<n; ++i) {
        g.add_edge(0, i);
        g.add_edge(i, 0);
    }

    CentroidDecomposition cd(g);
    
    EXPECT_EQ(cd.get_root(), 0);
    for(int i=1; i<n; ++i) {
        EXPECT_EQ(cd.get_parent(i), 0);
        EXPECT_EQ(cd.get_level(i), 1);
    }
}

TEST_F(CentroidDecompositionTest, HeightProperty) {
    // Random tree or large path
    int n = 100;
    Graph g(n);
    for(int i=0; i<n-1; ++i) {
        g.add_edge(i, i+1);
        g.add_edge(i+1, i);
    }
    
    CentroidDecomposition cd(g);
    
    // Height should be <= log2(100) + C approx 7-8
    int max_level = 0;
    for(int i=0; i<n; ++i) {
        max_level = std::max(max_level, cd.get_level(i));
    }
    EXPECT_LE(max_level, 10);
}
