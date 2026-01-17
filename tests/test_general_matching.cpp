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
    EXPECT_EQ(total, 8);
}
