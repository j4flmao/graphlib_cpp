#include "graphlib/bipartite_matching.h"
#include <gtest/gtest.h>
#include <algorithm>
#include <vector>

using namespace graphlib;

TEST(BipartiteMatchingTest, EmptyGraph) {
    BipartiteMatching bm(5, 5);
    EXPECT_EQ(bm.max_matching(), 0);
}

TEST(BipartiteMatchingTest, SingleEdge) {
    BipartiteMatching bm(2, 2);
    bm.add_edge(0, 0);
    EXPECT_EQ(bm.max_matching(), 1);
    auto matching = bm.get_matching();
    EXPECT_EQ(matching.size(), 1);
    EXPECT_EQ(matching[0].first, 0);
    EXPECT_EQ(matching[0].second, 0);
}

TEST(BipartiteMatchingTest, DisjointEdges) {
    BipartiteMatching bm(3, 3);
    bm.add_edge(0, 0);
    bm.add_edge(1, 1);
    bm.add_edge(2, 2);
    EXPECT_EQ(bm.max_matching(), 3);
}

TEST(BipartiteMatchingTest, SharedRightNode) {
    // 0 -> 0
    // 1 -> 0
    // Only one can match
    BipartiteMatching bm(2, 1);
    bm.add_edge(0, 0);
    bm.add_edge(1, 0);
    EXPECT_EQ(bm.max_matching(), 1);
}

TEST(BipartiteMatchingTest, CompleteBipartiteK33) {
    int n = 3;
    BipartiteMatching bm(n, n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            bm.add_edge(i, j);
        }
    }
    EXPECT_EQ(bm.max_matching(), n);
}

TEST(BipartiteMatchingTest, PathStructure) {
    // 0 -> 0
    // 0 -> 1
    // 1 -> 1
    // 1 -> 2
    // 2 -> 2
    // Max matching 3: (0,0), (1,1), (2,2)
    BipartiteMatching bm(3, 3);
    bm.add_edge(0, 0);
    bm.add_edge(0, 1);
    bm.add_edge(1, 1);
    bm.add_edge(1, 2);
    bm.add_edge(2, 2);
    EXPECT_EQ(bm.max_matching(), 3);
}

TEST(BipartiteMatchingTest, HarderCase) {
    // Left: 0,1,2,3
    // Right: 0,1,2,3
    // Edges: (0,0), (0,1), (1,0), (2,1), (2,2), (3,2), (3,3)
    // Matches: (0,1), (1,0), (2,2), (3,3) -> 4
    BipartiteMatching bm(4, 4);
    bm.add_edge(0, 0); bm.add_edge(0, 1);
    bm.add_edge(1, 0);
    bm.add_edge(2, 1); bm.add_edge(2, 2);
    bm.add_edge(3, 2); bm.add_edge(3, 3);
    
    EXPECT_EQ(bm.max_matching(), 4);
}

TEST(WeightedBipartiteMatchingTest, SimpleWeights) {
    // 3x3 matrix
    // Row 0: 10, 5, 5
    // Row 1: 5, 10, 5
    // Row 2: 5, 5, 10
    // Optimal: (0,0)=10, (1,1)=10, (2,2)=10 -> Total 30
    WeightedBipartiteMatching wbm(3, 3);
    wbm.add_edge(0, 0, 10); wbm.add_edge(0, 1, 5); wbm.add_edge(0, 2, 5);
    wbm.add_edge(1, 0, 5); wbm.add_edge(1, 1, 10); wbm.add_edge(1, 2, 5);
    wbm.add_edge(2, 0, 5); wbm.add_edge(2, 1, 5); wbm.add_edge(2, 2, 10);
    
    EXPECT_EQ(wbm.max_weight_matching(), 30);
    
    auto matching = wbm.get_matching();
    EXPECT_EQ(matching.size(), 3);
    // Verify pairs
    std::vector<std::pair<int, int>> expected = {{0,0}, {1,1}, {2,2}};
    // Matching order is not guaranteed, sort to compare
    std::sort(matching.begin(), matching.end());
    EXPECT_EQ(matching, expected);
}

TEST(WeightedBipartiteMatchingTest, Rectangular) {
    // 2x3
    // 0: 10, 20, 0
    // 1: 0, 5, 30
    // Best: (0,1)=20, (1,2)=30 -> Total 50
    WeightedBipartiteMatching wbm(2, 3);
    wbm.add_edge(0, 0, 10);
    wbm.add_edge(0, 1, 20);
    wbm.add_edge(1, 1, 5);
    wbm.add_edge(1, 2, 30);
    
    EXPECT_EQ(wbm.max_weight_matching(), 50);
    
    auto matching = wbm.get_matching();
    EXPECT_EQ(matching.size(), 2);
    std::sort(matching.begin(), matching.end());
    std::vector<std::pair<int, int>> expected = {{0,1}, {1,2}};
    EXPECT_EQ(matching, expected);
}
