#include <gtest/gtest.h>
#include <graphlib/graphlib.h>

using graphlib::SplayTree;

TEST(SplayTreeTest, InsertAndSearch) {
    SplayTree tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);

    EXPECT_TRUE(tree.search(5));
    EXPECT_TRUE(tree.search(3));
    EXPECT_TRUE(tree.search(7));
    EXPECT_FALSE(tree.search(10));
}

TEST(SplayTreeTest, Remove) {
    SplayTree tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);

    tree.remove(3);
    EXPECT_FALSE(tree.search(3));
    EXPECT_TRUE(tree.search(5));
    EXPECT_TRUE(tree.search(7));
}

TEST(SplayTreeTest, RangeSumBasic) {
    SplayTree tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(1);
    tree.insert(9);

    int sum1 = tree.range_sum(3, 7);
    EXPECT_EQ(sum1, 3 + 5 + 7);

    int sum2 = tree.range_sum(1, 9);
    EXPECT_EQ(sum2, 1 + 3 + 5 + 7 + 9);
}

