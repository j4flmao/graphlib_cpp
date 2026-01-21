#include <gtest/gtest.h>
#include <graphlib/tree.h>
#include <vector>
#include <random>
#include <algorithm>
#include <queue>

using namespace graphlib;

class AdvancedTreeTest : public ::testing::Test {
protected:
    // Helper to generate random tree
    // Generates a tree rooted at 0 where parents are always < child indices (DAG-like construction ensures no cycles)
    void generate_random_tree(int n, TreeLCA& tree) {
        std::mt19937 rng(42);
        for(int i=1; i<n; ++i) {
            std::uniform_int_distribution<int> dist(0, i-1);
            int parent = dist(rng);
            tree.add_edge(parent, i);
        }
    }
    
    // Naive LCA using parent pointers
    int naive_lca(const TreeLCA& tree, int u, int v) {
        int d_u = tree.depth(u);
        int d_v = tree.depth(v);
        
        while(d_u > d_v) {
            u = tree.parent(u);
            d_u--;
        }
        while(d_v > d_u) {
            v = tree.parent(v);
            d_v--;
        }
        while(u != v) {
            u = tree.parent(u);
            v = tree.parent(v);
        }
        return u;
    }
};

TEST_F(AdvancedTreeTest, RandomTreeLCAStress) {
    int n = 100;
    TreeLCA tree(n);
    generate_random_tree(n, tree);
    tree.build(0);
    
    for(int i=0; i<1000; ++i) {
        int u = rand() % n;
        int v = rand() % n;
        int lca = tree.lca(u, v);
        int naive = naive_lca(tree, u, v);
        EXPECT_EQ(lca, naive) << "LCA mismatch for " << u << ", " << v;
        
        // Check distance property: dist(u,v) = depth(u) + depth(v) - 2*depth(lca)
        int dist = tree.distance(u, v);
        int calc_dist = tree.depth(u) + tree.depth(v) - 2 * tree.depth(lca);
        EXPECT_EQ(dist, calc_dist);
    }
}

TEST_F(AdvancedTreeTest, KthAncestor) {
    int n = 50;
    TreeLCA tree(n);
    // Line graph: 0-1-2-...-49
    for(int i=0; i<n-1; ++i) {
        tree.add_edge(i, i+1);
    }
    tree.build(0);
    
    for(int i=0; i<n; ++i) {
        for(int k=0; k<=i; ++k) {
            EXPECT_EQ(tree.kth_ancestor(i, k), i-k);
        }
        EXPECT_EQ(tree.kth_ancestor(i, i+1), -1);
    }
}

TEST_F(AdvancedTreeTest, PathSumRandomOps) {
    int n = 50;
    TreeLCA tree(n);
    generate_random_tree(n, tree);
    tree.build(0);
    
    std::vector<long long> values(n);
    for(int i=0; i<n; ++i) values[i] = i; // Initial values
    
    TreePathSum tps(tree, values);
    
    // Random updates and queries
    for(int k=0; k<500; ++k) {
        int type = rand() % 3;
        if(type == 0) { // Set Value
            int u = rand() % n;
            long long val = rand() % 100;
            tps.set_value(u, val);
            values[u] = val;
        } else if (type == 1) { // Add Value
            int u = rand() % n;
            long long delta = rand() % 10;
            tps.add_value(u, delta);
            values[u] += delta;
        } else { // Query Path
            int u = rand() % n;
            int v = rand() % n;
            int l = tree.lca(u, v);
            
            long long expected = 0;
            int curr = u;
            while(curr != l) {
                expected += values[curr];
                curr = tree.parent(curr);
            }
            curr = v;
            while(curr != l) {
                expected += values[curr];
                curr = tree.parent(curr);
            }
            expected += values[l];
            
            long long actual = tps.query_path(u, v);
            EXPECT_EQ(actual, expected) << "Path sum mismatch " << u << "->" << v;
        }
    }
}

TEST_F(AdvancedTreeTest, SubtreeSizeAndProperties) {
    int n = 20;
    TreeLCA tree(n);
    // Star graph: 0 is center
    for(int i=1; i<n; ++i) tree.add_edge(0, i);
    tree.build(0);
    
    EXPECT_EQ(tree.subtree_size(0), n);
    for(int i=1; i<n; ++i) {
        EXPECT_EQ(tree.subtree_size(i), 1);
        EXPECT_TRUE(tree.is_ancestor(0, i));
        EXPECT_FALSE(tree.is_ancestor(i, 0));
    }
}
