#include <gtest/gtest.h>
#include "graphlib/tree.h"
#include <vector>
#include <limits>

using namespace graphlib;

TEST(TreePathMaxTest, SimpleTree) {
    TreeLCA tree(5);
    tree.add_edge(0, 1);
    tree.add_edge(0, 2);
    tree.add_edge(1, 3);
    tree.add_edge(1, 4);
    tree.build(0);
    
    std::vector<long long> values = {10, 20, 5, 30, 40};
    TreePathMax tpm(tree, values);
    
    // Path 3 -> 1 -> 0 -> 2
    // Values: 30, 20, 10, 5. Max = 30.
    EXPECT_EQ(tpm.query_path(3, 2), 30);
    
    // Path 3 -> 4 (through 1)
    // Values: 30, 20, 40. Max = 40.
    EXPECT_EQ(tpm.query_path(3, 4), 40);
    
    // Update 1 to 100
    tpm.set_value(1, 100);
    EXPECT_EQ(tpm.query_path(3, 2), 100); // 30, 100, 10, 5 -> Max 100
}

TEST(TreePathMaxTest, SubtreeQuery) {
    TreeLCA tree(5);
    tree.add_edge(0, 1);
    tree.add_edge(0, 2);
    tree.add_edge(1, 3);
    tree.add_edge(1, 4);
    tree.build(0);
    
    std::vector<long long> values = {1, 2, 3, 4, 5};
    TreePathMax tpm(tree, values);
    
    // Subtree of 1: {1, 3, 4} -> values {2, 4, 5}. Max = 5.
    EXPECT_EQ(tpm.query_subtree(1), 5);
    
    // Subtree of 2: {2} -> value 3. Max = 3.
    EXPECT_EQ(tpm.query_subtree(2), 3);
}

TEST(TreePathMaxTest, NegativeValues) {
    TreeLCA tree(3);
    tree.add_edge(0, 1);
    tree.add_edge(1, 2);
    tree.build(0);
    
    std::vector<long long> values = {-10, -50, -5};
    TreePathMax tpm(tree, values);
    
    EXPECT_EQ(tpm.query_path(0, 2), -5);
    
    tpm.set_value(2, -100);
    EXPECT_EQ(tpm.query_path(0, 2), -10);
}
