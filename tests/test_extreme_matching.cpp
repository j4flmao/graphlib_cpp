#include "graphlib/general_matching.h"
#include "graphlib/graph_generator.h"
#include <gtest/gtest.h>
#include <vector>
#include <numeric>

using namespace graphlib;

TEST(GeneralMatchingTest, CompleteGraphK20) {
    // K20 has 20 vertices, all degrees 19 (odd).
    // Maximum matching should have size 10.
    int n = 20;
    GeneralMatching gm(n);
    for(int i=0; i<n; ++i) {
        for(int j=i+1; j<n; ++j) {
            gm.add_edge(i, j, 1);
        }
    }
    
    // Unweighted matching check
    EXPECT_EQ(gm.maximum_matching(), 10);
    
    // Weighted matching check
    // All weights 1. Max weight matching should be 10.
    EXPECT_EQ(gm.maximum_weight_matching(), 10);
}

TEST(GeneralMatchingTest, WeightedK20_VariableWeights) {
    int n = 20;
    GeneralMatching gm(n);
    // Edges (2i, 2i+1) have weight 100. Others have weight 1.
    // Optimal matching should pick (0,1), (2,3), ... (18,19) -> total 1000.
    
    for(int i=0; i<n; ++i) {
        for(int j=i+1; j<n; ++j) {
            int w = 1;
            if ((i / 2 == j / 2) && (i % 2 == 0) && (j == i + 1)) {
                w = 100;
            }
            gm.add_edge(i, j, w);
        }
    }
    
    EXPECT_EQ(gm.maximum_weight_matching(), 1000);
}
