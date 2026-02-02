#include <gtest/gtest.h>
#include "graphlib/centroid_decomposition.h"
#include <vector>

using namespace graphlib;

TEST(CentroidTest, PathGraph) {
    // 0-1-2-3-4
    // Centroid of path length 5 is usually the middle (2).
    // Or 1/3?
    // Size = 5. > 2.5? Child sizes: 1->4 (yes), 3->1 (no).
    // So 2 is centroid.
    
    Graph g(5);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 3); g.add_edge(3, 2);
    g.add_edge(3, 4); g.add_edge(4, 3);
    
    CentroidDecomposition cd(g);
    
    // Root should be 2
    int root = cd.get_centroid_root();
    EXPECT_EQ(root, 2);
    
    // Parent of 2 is -1.
    EXPECT_EQ(cd.get_centroid_parent(2), -1);
    
    // Neighbors of 2 are 1 and 3.
    // Component {0, 1}: Size 2. Centroid could be 0 or 1.
    // If 1 is centroid, parent is 2.
    // Component {3, 4}: Size 2. Centroid could be 3 or 4.
    
    int p1 = cd.get_centroid_parent(1);
    int p3 = cd.get_centroid_parent(3);
    
    // They must point to 2 (or if hierarchy is deeper, eventually to 2)
    // Actually in decomp, neighbors of centroid become roots of sub-problems, 
    // and 'centroid' becomes their parent.
    // So 1's parent MUST be 2. 3's parent MUST be 2.
    // Unless 0 was chosen as centroid for {0,1}, then 0's parent is 2.
    // But algorithm typically picks "first" suitable.
    
    EXPECT_TRUE(p1 == 2 || cd.get_centroid_parent(0) == 2);
    EXPECT_TRUE(p3 == 2 || cd.get_centroid_parent(4) == 2);
}

TEST(CentroidTest, StarGraph) {
    // 0 connected to 1, 2, 3, 4
    // Centroid MUST be 0.
    Graph g(5);
    for(int i=1; i<5; ++i) {
        g.add_edge(0, i);
        g.add_edge(i, 0);
    }
    
    CentroidDecomposition cd(g);
    EXPECT_EQ(cd.get_centroid_root(), 0);
    
    for(int i=1; i<5; ++i) {
        EXPECT_EQ(cd.get_centroid_parent(i), 0);
    }
}
