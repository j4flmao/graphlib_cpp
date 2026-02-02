#include <gtest/gtest.h>
#include "graphlib/link_cut_tree.h"
#include <vector>

using namespace graphlib;

TEST(LinkCutTreeTestV2, BasicOperations) {
    LinkCutTree lct(5);
    
    // Set values
    for(int i=0; i<5; ++i) lct.set_value(i, i+1);
    
    // Initially disconnected
    EXPECT_FALSE(lct.is_connected(0, 1));
    
    // Link 0-1
    lct.link(0, 1);
    EXPECT_TRUE(lct.is_connected(0, 1));
    EXPECT_EQ(lct.query_path_sum(0, 1), 1 + 2);
    
    // Link 1-2
    lct.link(1, 2);
    EXPECT_TRUE(lct.is_connected(0, 2));
    EXPECT_EQ(lct.query_path_sum(0, 2), 1 + 2 + 3);
    EXPECT_EQ(lct.query_path_max(0, 2), 3);
    
    // Cut 1-2
    lct.cut(1, 2);
    EXPECT_FALSE(lct.is_connected(0, 2));
    EXPECT_TRUE(lct.is_connected(0, 1));
    
    // Check sums after cut
    EXPECT_EQ(lct.query_path_sum(0, 1), 3);
    
    // Update value
    lct.set_value(0, 10);
    EXPECT_EQ(lct.query_path_sum(0, 1), 10 + 2);
}

TEST(LinkCutTreeTest, CycleAndReroot) {
    LinkCutTree lct(3);
    lct.link(0, 1);
    lct.link(1, 2);
    // Tree 0-1-2
    
    EXPECT_EQ(lct.find_root(2), 0); // Assuming 0 was root initially or by link order
    
    lct.make_root(2);
    EXPECT_EQ(lct.find_root(0), 2);
    
    // Try to form a cycle (should be handled by used, usually we assert no cycle)
    // lct.link(2, 0); // This operation is technically invalid in standard LCT for trees forest.
    // LCT maintains a FOREST. Linking two nodes in same tree is undefined behavior or no-op.
    
    if (!lct.is_connected(2, 0)) {
        lct.link(2, 0);
    }
}

TEST(LinkCutTreeTest, PathQueries) {
    int n = 10;
    LinkCutTree lct(n);
    for(int i=0; i<n; ++i) lct.set_value(i, 1);
    
    // Line graph 0-1-2-...-9
    for(int i=0; i<n-1; ++i) lct.link(i, i+1);
    
    EXPECT_EQ(lct.query_path_sum(0, 9), 10);
    EXPECT_EQ(lct.query_path_sum(2, 7), 6); // 2,3,4,5,6,7 -> 6 nodes
    
    lct.cut(4, 5);
    EXPECT_EQ(lct.query_path_sum(0, 4), 5);
    EXPECT_FALSE(lct.is_connected(0, 9));
}
