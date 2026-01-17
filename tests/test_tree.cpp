#include <gtest/gtest.h>
#include <graphlib/tree.h>

using graphlib::TreeLCA;
using graphlib::TreePathSum;

TEST(TreeLCATest, SimpleLineTree) {
    int n = 4;
    TreeLCA tree(n);
    tree.add_edge(0, 1);
    tree.add_edge(1, 2);
    tree.add_edge(2, 3);

    tree.build(0);

    EXPECT_EQ(tree.lca(0, 1), 0);
    EXPECT_EQ(tree.lca(1, 2), 1);
    EXPECT_EQ(tree.lca(2, 3), 2);
    EXPECT_EQ(tree.lca(0, 3), 0);

    EXPECT_EQ(tree.depth(0), 0);
    EXPECT_EQ(tree.depth(3), 3);

    EXPECT_EQ(tree.distance(0, 3), 3);
    EXPECT_EQ(tree.distance(1, 3), 2);

    EXPECT_TRUE(tree.is_ancestor(0, 3));
    EXPECT_TRUE(tree.is_ancestor(1, 3));
    EXPECT_FALSE(tree.is_ancestor(3, 1));

    EXPECT_EQ(tree.kth_ancestor(3, 0), 3);
    EXPECT_EQ(tree.kth_ancestor(3, 1), 2);
    EXPECT_EQ(tree.kth_ancestor(3, 2), 1);
    EXPECT_EQ(tree.kth_ancestor(3, 3), 0);
    EXPECT_EQ(tree.kth_ancestor(3, 4), -1);

    EXPECT_EQ(tree.subtree_size(0), 4);
    EXPECT_EQ(tree.subtree_size(1), 3);
    EXPECT_EQ(tree.subtree_size(2), 2);
    EXPECT_EQ(tree.subtree_size(3), 1);

    EXPECT_EQ(tree.kth_on_path(0, 3, 0), 0);
    EXPECT_EQ(tree.kth_on_path(0, 3, 1), 1);
    EXPECT_EQ(tree.kth_on_path(0, 3, 2), 2);
    EXPECT_EQ(tree.kth_on_path(0, 3, 3), 3);
    EXPECT_EQ(tree.kth_on_path(0, 3, 4), -1);

    EXPECT_EQ(tree.kth_on_path(3, 0, 0), 3);
    EXPECT_EQ(tree.kth_on_path(3, 0, 1), 2);
    EXPECT_EQ(tree.kth_on_path(3, 0, 2), 1);
    EXPECT_EQ(tree.kth_on_path(3, 0, 3), 0);
    EXPECT_EQ(tree.kth_on_path(3, 0, 4), -1);

    int p0 = tree.hld_pos(0);
    int p1 = tree.hld_pos(1);
    int p2 = tree.hld_pos(2);
    int p3 = tree.hld_pos(3);
    EXPECT_LE(p0, p1);
    EXPECT_LE(p1, p2);
    EXPECT_LE(p2, p3);

    EXPECT_EQ(tree.hld_head(0), 0);
    EXPECT_EQ(tree.hld_head(1), 0);
    EXPECT_EQ(tree.hld_head(2), 0);
    EXPECT_EQ(tree.hld_head(3), 0);

    std::vector<std::pair<int, int>> segments;
    tree.hld_decompose_path(0, 3, segments);
    ASSERT_EQ(static_cast<int>(segments.size()), 1);
    EXPECT_EQ(segments[0].first, p0);
    EXPECT_EQ(segments[0].second, p3);
}

TEST(TreeLCATest, StarTree) {
    int n = 5;
    TreeLCA tree(n);
    for (int v = 1; v < n; ++v) {
        tree.add_edge(0, v);
    }

    tree.build(0);

    for (int v = 1; v < n; ++v) {
        EXPECT_EQ(tree.parent(v), 0);
        EXPECT_EQ(tree.depth(v), 1);
        EXPECT_EQ(tree.lca(0, v), 0);
        EXPECT_EQ(tree.distance(0, v), 1);
    }

    EXPECT_EQ(tree.lca(1, 2), 0);
    EXPECT_EQ(tree.distance(1, 2), 2);

    EXPECT_TRUE(tree.is_ancestor(0, 1));
    EXPECT_TRUE(tree.is_ancestor(0, 2));
    EXPECT_TRUE(tree.is_ancestor(0, 3));
    EXPECT_TRUE(tree.is_ancestor(0, 4));
    EXPECT_FALSE(tree.is_ancestor(1, 2));
    EXPECT_FALSE(tree.is_ancestor(2, 1));

    EXPECT_EQ(tree.subtree_size(0), 5);
    EXPECT_EQ(tree.subtree_size(1), 1);
    EXPECT_EQ(tree.subtree_size(2), 1);
    EXPECT_EQ(tree.subtree_size(3), 1);
    EXPECT_EQ(tree.subtree_size(4), 1);

    EXPECT_EQ(tree.kth_on_path(1, 2, 0), 1);
    EXPECT_EQ(tree.kth_on_path(1, 2, 1), 0);
    EXPECT_EQ(tree.kth_on_path(1, 2, 2), 2);
    EXPECT_EQ(tree.kth_on_path(1, 2, 3), -1);

    std::vector<std::pair<int, int>> segments12;
    tree.hld_decompose_path(1, 2, segments12);
    ASSERT_GE(static_cast<int>(segments12.size()), 1);
}

TEST(TreeLCATest, InvalidQueriesBeforeBuild) {
    int n = 3;
    TreeLCA tree(n);
    tree.add_edge(0, 1);
    tree.add_edge(1, 2);

    EXPECT_EQ(tree.lca(0, 2), -1);
    EXPECT_EQ(tree.distance(0, 2), -1);
    EXPECT_EQ(tree.parent(1), -1);
    EXPECT_EQ(tree.depth(1), 0);
}

TEST(TreePathSumTest, PathAndSubtreeLineTree) {
    int n = 4;
    TreeLCA tree(n);
    tree.add_edge(0, 1);
    tree.add_edge(1, 2);
    tree.add_edge(2, 3);

    tree.build(0);

    std::vector<long long> values(n);
    values[0] = 1;
    values[1] = 2;
    values[2] = 3;
    values[3] = 4;

    TreePathSum ps(tree, values);

    EXPECT_EQ(ps.query_path(0, 0), 1);
    EXPECT_EQ(ps.query_path(0, 1), 3);
    EXPECT_EQ(ps.query_path(0, 3), 10);
    EXPECT_EQ(ps.query_path(1, 3), 9);

    EXPECT_EQ(ps.query_subtree(0), 10);
    EXPECT_EQ(ps.query_subtree(1), 9);
    EXPECT_EQ(ps.query_subtree(2), 7);
    EXPECT_EQ(ps.query_subtree(3), 4);

    ps.add_value(2, 1);
    EXPECT_EQ(ps.get_value(2), 4);
    EXPECT_EQ(ps.query_path(0, 3), 11);
    EXPECT_EQ(ps.query_subtree(1), 10);

    ps.set_value(0, 5);
    EXPECT_EQ(ps.get_value(0), 5);
    EXPECT_EQ(ps.query_path(0, 3), 15);
}
