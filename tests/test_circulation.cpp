#include <gtest/gtest.h>
#include <graphlib/graphlib.h>

using graphlib::CirculationEdge;

TEST(CirculationTest, SimpleFeasible) {
    int n = 3;
    std::vector<CirculationEdge> edges;

    // 0 -> 1 [1, 3] cost 2
    // 1 -> 2 [0, 2] cost 1
    // 2 -> 0 [0, 2] cost 3
    
    edges.push_back(CirculationEdge{0, 1, 1, 3, 2});
    edges.push_back(CirculationEdge{1, 2, 0, 2, 1});
    edges.push_back(CirculationEdge{2, 0, 0, 2, 3});

    long long total_cost = 0;
    bool ok = graphlib::min_cost_circulation(n, edges, total_cost);

    EXPECT_TRUE(ok);
    // Min valid flow is 1 everywhere. Cost = 1*2 + 1*1 + 1*3 = 6.
    EXPECT_EQ(total_cost, 6);
}

TEST(CirculationTest, Infeasible) {
    int n = 2;
    std::vector<CirculationEdge> edges;
    // 0 -> 1 [2, 3]
    // 1 -> 0 [0, 1]
    // Impossible
    
    edges.push_back(CirculationEdge{0, 1, 2, 3, 0});
    edges.push_back(CirculationEdge{1, 0, 0, 1, 0});
    
    long long total_cost = 0;
    bool ok = graphlib::min_cost_circulation(n, edges, total_cost);
    EXPECT_FALSE(ok);
}
