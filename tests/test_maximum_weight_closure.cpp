#include "graphlib/max_flow.h"
#include <gtest/gtest.h>
#include <vector>

using namespace graphlib;

TEST(MaximumWeightClosureTest, BasicCase) {
    // Weights: 0: 10, 1: 10, 2: -5
    // Dependencies: 0->2, 1->2
    // If we pick 0, we must pick 2. Net: 10 - 5 = 5.
    // If we pick 1, we must pick 2. Net: 10 - 5 = 5.
    // If we pick 0 and 1, we must pick 2. Net: 10 + 10 - 5 = 15.
    // Optimal: Pick all.
    
    std::vector<long long> weights = {10, 10, -5};
    std::vector<std::pair<int, int>> deps = {{0, 2}, {1, 2}};
    
    EXPECT_EQ(maximum_weight_closure(weights, deps), 15);
}

TEST(MaximumWeightClosureTest, NegativeCostAvoidance) {
    // Weights: 0: 10, 1: -20
    // Dependencies: 0->1
    // If 0 is picked, 1 must be picked. Net: 10 - 20 = -10.
    // Better to pick nothing (0).
    
    std::vector<long long> weights = {10, -20};
    std::vector<std::pair<int, int>> deps = {{0, 1}};
    
    EXPECT_EQ(maximum_weight_closure(weights, deps), 0);
}

TEST(MaximumWeightClosureTest, ComplexCase) {
    // 0(10) -> 2(-5) -> 4(5)
    // 1(20) -> 3(-10) -> 4(5)
    // 
    // Groups:
    // {0, 2, 4}: 10 - 5 + 5 = 10
    // {1, 3, 4}: 20 - 10 + 5 = 15
    // {0, 1, 2, 3, 4}: 10 + 20 - 5 - 10 + 5 = 20
    // {4}: 5
    // {2, 4}: 0
    // {3, 4}: -5
    
    std::vector<long long> weights = {10, 20, -5, -10, 5};
    std::vector<std::pair<int, int>> deps = {
        {0, 2}, {2, 4},
        {1, 3}, {3, 4}
    };
    
    EXPECT_EQ(maximum_weight_closure(weights, deps), 20);
}

TEST(MaximumWeightClosureTest, CycleCase) {
    // 0(10) -> 1(-5) -> 0
    // Must pick both or neither.
    // Net: 5. Pick both.
    
    std::vector<long long> weights = {10, -5};
    std::vector<std::pair<int, int>> deps = {{0, 1}, {1, 0}};
    
    EXPECT_EQ(maximum_weight_closure(weights, deps), 5);
}
