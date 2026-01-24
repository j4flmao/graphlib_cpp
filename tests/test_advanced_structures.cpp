#include "graphlib/link_cut_tree.h"
#include "graphlib/dsu_rollback.h"
#include <gtest/gtest.h>

using namespace graphlib;

TEST(LinkCutTreeTest, BasicOperations) {
    LinkCutTree lct(5);
    lct.set_value(0, 10);
    lct.set_value(1, 20);
    lct.set_value(2, 30);
    
    EXPECT_EQ(lct.get_value(0), 10);
    EXPECT_EQ(lct.query_path_sum(0, 0), 10);
    
    lct.link(0, 1);
    EXPECT_TRUE(lct.is_connected(0, 1));
    EXPECT_EQ(lct.query_path_sum(0, 1), 30);
    
    lct.link(1, 2);
    EXPECT_TRUE(lct.is_connected(0, 2));
    EXPECT_EQ(lct.query_path_sum(0, 2), 60);
    
    lct.cut(1, 2);
    EXPECT_FALSE(lct.is_connected(0, 2));
    EXPECT_TRUE(lct.is_connected(0, 1));
    EXPECT_EQ(lct.query_path_sum(0, 1), 30);
}

TEST(LinkCutTreeTest, PathMax) {
    LinkCutTree lct(5);
    lct.set_value(0, 10);
    lct.set_value(1, 50);
    lct.set_value(2, 30);
    
    lct.link(0, 1);
    lct.link(1, 2);
    
    EXPECT_EQ(lct.query_path_max(0, 2), 50);
    
    lct.set_value(1, 5);
    EXPECT_EQ(lct.query_path_max(0, 2), 30);
}

TEST(LinkCutTreeTest, CycleAndRoot) {
    LinkCutTree lct(3);
    lct.link(0, 1);
    lct.link(1, 2);
    
    lct.make_root(2);
    EXPECT_EQ(lct.find_root(0), 2);
    
    lct.make_root(0);
    EXPECT_EQ(lct.find_root(2), 0);
}

TEST(DsuRollbackTest, BasicOperations) {
    DsuRollback dsu(5);
    EXPECT_EQ(dsu.component_count(), 5);
    
    dsu.unite(0, 1);
    EXPECT_EQ(dsu.component_count(), 4);
    EXPECT_TRUE(dsu.connected(0, 1));
    
    dsu.unite(1, 2);
    EXPECT_EQ(dsu.component_count(), 3);
    EXPECT_TRUE(dsu.connected(0, 2));
}

TEST(DsuRollbackTest, Rollback) {
    DsuRollback dsu(5);
    dsu.unite(0, 1);
    int snap = dsu.snapshot();
    
    dsu.unite(1, 2);
    EXPECT_TRUE(dsu.connected(0, 2));
    
    dsu.rollback();
    EXPECT_FALSE(dsu.connected(0, 2));
    EXPECT_TRUE(dsu.connected(0, 1));
    
    dsu.rollback_to(snap); // Should do nothing as we already rolled back manually
    EXPECT_TRUE(dsu.connected(0, 1));
    
    dsu.rollback_to(0);
    EXPECT_FALSE(dsu.connected(0, 1));
    EXPECT_EQ(dsu.component_count(), 5);
}
