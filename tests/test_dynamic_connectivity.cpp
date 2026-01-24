#include "graphlib/dynamic_connectivity.h"
#include <gtest/gtest.h>
#include <vector>

using namespace graphlib;

TEST(DynamicConnectivityTest, BasicAddQuery) {
    int n = 5;
    DynamicConnectivity dc(n);
    
    // Add 0-1
    dc.add_edge(0, 1);
    dc.query(0, 1); // Should be true
    dc.query(0, 2); // Should be false
    
    // Add 1-2
    dc.add_edge(1, 2);
    dc.query(0, 2); // Should be true
    
    std::vector<bool> results = dc.solve();
    
    ASSERT_EQ(results.size(), 3);
    EXPECT_TRUE(results[0]);
    EXPECT_FALSE(results[1]);
    EXPECT_TRUE(results[2]);
}

TEST(DynamicConnectivityTest, AddRemoveQuery) {
    int n = 5;
    DynamicConnectivity dc(n);
    
    // Time 0: Add 0-1
    dc.add_edge(0, 1);
    dc.query(0, 1); // T
    
    // Time 1: Remove 0-1
    dc.remove_edge(0, 1);
    dc.query(0, 1); // F
    
    // Time 2: Add 0-2, 2-1
    dc.add_edge(0, 2);
    dc.add_edge(2, 1);
    dc.query(0, 1); // T
    
    std::vector<bool> results = dc.solve();
    
    ASSERT_EQ(results.size(), 3);
    EXPECT_TRUE(results[0]);
    EXPECT_FALSE(results[1]);
    EXPECT_TRUE(results[2]);
}

TEST(DynamicConnectivityTest, ComplexScenario) {
    int n = 4;
    DynamicConnectivity dc(n);
    
    // 0-1, 2-3
    dc.add_edge(0, 1);
    dc.add_edge(2, 3);
    dc.query(0, 1); // T
    dc.query(2, 3); // T
    dc.query(0, 3); // F
    
    // Connect components: 1-2
    dc.add_edge(1, 2);
    dc.query(0, 3); // T
    
    // Break connection: remove 0-1
    dc.remove_edge(0, 1);
    dc.query(0, 3); // F (0 is isolated)
    dc.query(1, 3); // T (1-2-3 still exists)
    
    std::vector<bool> results = dc.solve();
    
    std::vector<bool> expected = {true, true, false, true, false, true};
    ASSERT_EQ(results.size(), expected.size());
    for(size_t i=0; i<results.size(); ++i) {
        EXPECT_EQ(results[i], expected[i]) << "Mismatch at index " << i;
    }
}

TEST(DynamicConnectivityTest, SameEdgeMultipleTimes) {
    // Adding/Removing same edge multiple times
    int n = 3;
    DynamicConnectivity dc(n);
    
    dc.add_edge(0, 1);
    dc.query(0, 1); // T
    
    dc.remove_edge(0, 1);
    dc.query(0, 1); // F
    
    dc.add_edge(0, 1);
    dc.query(0, 1); // T
    
    std::vector<bool> results = dc.solve();
    ASSERT_EQ(results.size(), 3);
    EXPECT_TRUE(results[0]);
    EXPECT_FALSE(results[1]);
    EXPECT_TRUE(results[2]);
}

TEST(DynamicConnectivityTest, EmptyAndTrivial) {
    DynamicConnectivity dc(3);
    dc.query(0, 1);
    std::vector<bool> results = dc.solve();
    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0], false);
}
