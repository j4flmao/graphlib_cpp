#include "graphlib/graphlib.h"
#include "graphlib/dominator_tree.h"
#include <gtest/gtest.h>
#include <vector>

using namespace graphlib;

class DominatorTreeTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

TEST_F(DominatorTreeTest, SimpleGraph) {
    // Example from Lengauer-Tarjan paper or standard textbook
    //      0 (R)
    //     / \
    //    1   2
    //    |   |
    //    3   4
    //     \ /
    //      5
    
    int n = 6;
    Graph g(n, true);
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(1, 3);
    g.add_edge(2, 4);
    g.add_edge(3, 5);
    g.add_edge(4, 5);
    
    DominatorTree dt(g, 0);
    
    // Immediate dominators
    EXPECT_EQ(dt.get_idom(0), -1);
    EXPECT_EQ(dt.get_idom(1), 0);
    EXPECT_EQ(dt.get_idom(2), 0);
    EXPECT_EQ(dt.get_idom(3), 1);
    EXPECT_EQ(dt.get_idom(4), 2);
    EXPECT_EQ(dt.get_idom(5), 0); // Both paths merge at 5, so 0 dominates 5. 1 and 2 do not.
    
    EXPECT_TRUE(dt.dominates(0, 5));
    EXPECT_FALSE(dt.dominates(1, 5));
    EXPECT_FALSE(dt.dominates(2, 5));
}

TEST_F(DominatorTreeTest, CrossEdge) {
    //      0
    //     / \
    //    1-->2
    //    |
    //    3
    int n = 4;
    Graph g(n, true);
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(1, 2);
    g.add_edge(1, 3);
    
    DominatorTree dt(g, 0);
    
    EXPECT_EQ(dt.get_idom(1), 0);
    EXPECT_EQ(dt.get_idom(2), 0); // 0 dominates 2 directly. 1->2 is just a cross edge.
    EXPECT_EQ(dt.get_idom(3), 1);
    
    EXPECT_TRUE(dt.dominates(0, 2));
    EXPECT_TRUE(dt.dominates(0, 3));
    EXPECT_TRUE(dt.dominates(1, 3));
}

TEST_F(DominatorTreeTest, UnreachableNodes) {
    int n = 3;
    Graph g(n, true);
    g.add_edge(0, 1);
    // 2 is isolated
    
    DominatorTree dt(g, 0);
    
    EXPECT_EQ(dt.get_idom(1), 0);
    EXPECT_EQ(dt.get_idom(2), -1);
    EXPECT_FALSE(dt.dominates(0, 2));
}

TEST_F(DominatorTreeTest, ComplexLoop) {
    // Wikipedia example or similar
    // 0 -> 1 -> 2 -> 3
    //      |    ^    |
    //      v    |    v
    //      4 -> 5 -> 6
    // 5->1
    
    int n = 7;
    Graph g(n, true);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(1, 4);
    g.add_edge(2, 3);
    g.add_edge(3, 6);
    g.add_edge(4, 5);
    g.add_edge(5, 2); // 5->2
    g.add_edge(5, 6); // 5->6 (optional, let's say 4->5->6)
    
    // Dominators:
    // 0 dom 1
    // 1 dom 4, 5
    // 1 dom 2 (via 1->2 and 1->4->5->2)
    // 2 dom 3
    // 1 dom 6 (via 1->2->3->6 and 1->4->5->6)
    
    DominatorTree dt(g, 0);
    
    EXPECT_EQ(dt.get_idom(1), 0);
    EXPECT_EQ(dt.get_idom(2), 1);
    EXPECT_EQ(dt.get_idom(3), 2);
    EXPECT_EQ(dt.get_idom(4), 1);
    EXPECT_EQ(dt.get_idom(5), 4);
    
    // 6 is reached via 3 (from 2) and 5 (from 4).
    // paths: 0-1-2-3-6, 0-1-4-5-6, 0-1-4-5-2-3-6
    // Dominator of 6 is 1.
    EXPECT_EQ(dt.get_idom(6), 1); 
}
