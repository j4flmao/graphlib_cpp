#include <gtest/gtest.h>
#include <graphlib/graphlib.h>

using graphlib::GeneralMatching;

TEST(GeneralMatchingTest, CompleteGraphEven) {
    GeneralMatching g(4);
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(0, 3);
    g.add_edge(1, 2);
    g.add_edge(1, 3);
    g.add_edge(2, 3);

    int matching = g.maximum_matching();
    EXPECT_EQ(matching, 2);
}

TEST(GeneralMatchingTest, OddCycle) {
    GeneralMatching g(3);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);

    int matching = g.maximum_matching();
    EXPECT_EQ(matching, 1);
}

TEST(GeneralMatchingTest, WeightedMatching) {
    GeneralMatching g(4);
    g.add_edge(0, 1, 5);
    g.add_edge(0, 2, 2);
    g.add_edge(1, 2, 4);
    g.add_edge(1, 3, 3);
    g.add_edge(2, 3, 1);

    long long total = g.maximum_weight_matching();
    EXPECT_EQ(total, 6);
}

TEST(GeneralMatchingTest, LargeWeightedMatching) {
    // N=24, bypasses brute force (<=20)
    int n = 24;
    GeneralMatching g(n);
    
    // Create 12 pairs (0,1), (2,3), ... with weight 100
    long long expected_weight = 0;
    for (int i = 0; i < n; i += 2) {
        g.add_edge(i, i+1, 100);
        expected_weight += 100;
    }
    
    // Add noise edges with lower weight
    for (int i = 1; i < n - 1; i += 2) {
        g.add_edge(i, i+1, 10);
    }
    
    long long total = g.maximum_weight_matching();
    EXPECT_EQ(total, expected_weight);
}
