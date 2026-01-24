#include <gtest/gtest.h>
#include "graphlib/tree.h"
#include <vector>

using namespace graphlib;

class TreePathQueriesTest : public ::testing::Test {
protected:
    std::unique_ptr<TreeLCA> tree;
    
    void SetUp() override {
        // Create a tree:
        //      0
        //     / \
        //    1   2
        //   /|   |
        //  3 4   5
        tree = std::make_unique<TreeLCA>(6);
        tree->add_edge(0, 1);
        tree->add_edge(0, 2);
        tree->add_edge(1, 3);
        tree->add_edge(1, 4);
        tree->add_edge(2, 5);
        tree->build(0);
    }
};

TEST_F(TreePathQueriesTest, PathSum) {
    std::vector<long long> values = {1, 2, 3, 4, 5, 6};
    TreePathSum path_sum(*tree, values);

    // Path 3 -> 1 -> 0 -> 2 -> 5
    // Values: 4 + 2 + 1 + 3 + 6 = 16
    EXPECT_EQ(path_sum.query_path(3, 5), 16);

    // Path 3 -> 4 (LCA is 1)
    // Values: 4 + 2 + 5 = 11
    EXPECT_EQ(path_sum.query_path(3, 4), 11);

    // Update value of 1 to 10
    path_sum.set_value(1, 10);
    // New Path 3 -> 4: 4 + 10 + 5 = 19
    EXPECT_EQ(path_sum.query_path(3, 4), 19);
}

TEST_F(TreePathQueriesTest, PathMax) {
    std::vector<long long> values = {1, 2, 3, 4, 5, 6};
    TreePathMax path_max(*tree, values);

    // Path 3 -> 5
    // Max of {4, 2, 1, 3, 6} is 6
    EXPECT_EQ(path_max.query_path(3, 5), 6);

    // Path 3 -> 4
    // Max of {4, 2, 5} is 5
    EXPECT_EQ(path_max.query_path(3, 4), 5);
}

TEST_F(TreePathQueriesTest, SubtreeQueries) {
    std::vector<long long> values = {1, 2, 3, 4, 5, 6};
    TreePathSum path_sum(*tree, values);
    
    // Subtree of 1: {1, 3, 4}, values {2, 4, 5} sum = 11
    EXPECT_EQ(path_sum.query_subtree(1), 11);
    
    // Subtree of 2: {2, 5}, values {3, 6} sum = 9
    EXPECT_EQ(path_sum.query_subtree(2), 9);
}
