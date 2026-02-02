#include <gtest/gtest.h>
#include "graphlib/isomorphism.h"
#include <vector>

using namespace graphlib;

TEST(TreeIsomorphismTest, IsomorphicStars) {
    // Star graphs are isomorphic if they have same number of leaves.
    // Center is 0.
    Graph t1(5);
    for(int i=1; i<5; ++i) { t1.add_edge(0, i); t1.add_edge(i, 0); }
    
    Graph t2(5);
    for(int i=1; i<5; ++i) { t2.add_edge(4, i-1); t2.add_edge(i-1, 4); } // Center at 4
    
    EXPECT_TRUE(is_tree_isomorphic(t1, t2));
}

TEST(TreeIsomorphismTest, IsomorphicPath) {
    // 0-1-2-3
    Graph t1(4);
    t1.add_edge(0, 1); t1.add_edge(1, 0);
    t1.add_edge(1, 2); t1.add_edge(2, 1);
    t1.add_edge(2, 3); t1.add_edge(3, 2);
    
    // 3-2-0-1 (Same structure 0-0-0-0)
    Graph t2(4);
    t2.add_edge(3, 2); t2.add_edge(2, 3);
    t2.add_edge(2, 0); t2.add_edge(0, 2);
    t2.add_edge(0, 1); t2.add_edge(1, 0);
    
    EXPECT_TRUE(is_tree_isomorphic(t1, t2));
}

TEST(TreeIsomorphismTest, NonIsomorphic) {
    // Path 4 (0-1-2-3) vs Star 4 (0 connected to 1,2,3)
    Graph t1(4); // Path
    t1.add_edge(0, 1); t1.add_edge(1, 0);
    t1.add_edge(1, 2); t1.add_edge(2, 1);
    t1.add_edge(2, 3); t1.add_edge(3, 2);
    
    Graph t2(4); // Star
    t2.add_edge(0, 1); t2.add_edge(1, 0);
    t2.add_edge(0, 2); t2.add_edge(2, 0);
    t2.add_edge(0, 3); t2.add_edge(3, 0);
    
    EXPECT_FALSE(is_tree_isomorphic(t1, t2));
}

TEST(TreeIsomorphismTest, TwoCenters) {
    // Path 0-1 is isomorphic to Path 1-0.
    // Centers are {0, 1}.
    Graph t1(2);
    t1.add_edge(0, 1); t1.add_edge(1, 0);
    
    Graph t2(2);
    t2.add_edge(1, 0); t2.add_edge(0, 1);
    
    EXPECT_TRUE(is_tree_isomorphic(t1, t2));
}
