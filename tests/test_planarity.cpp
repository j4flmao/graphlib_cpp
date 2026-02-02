#include <gtest/gtest.h>
#include "graphlib/planarity.h"
#include <vector>

using namespace graphlib;

TEST(PlanarityTest, K4) {
    // K4 is planar.
    Graph g(4);
    for(int i=0; i<4; ++i)
        for(int j=i+1; j<4; ++j)
            g.add_edge(i, j);
            
    EXPECT_TRUE(is_planar(g));
    auto faces = get_planar_faces(g);
    // K4 has V=4, E=6 -> F = 2 - 4 + 6 = 4.
    EXPECT_EQ(faces.size(), 4);
}

TEST(PlanarityTest, K5) {
    // K5 is NOT planar.
    Graph g(5);
    for(int i=0; i<5; ++i)
        for(int j=i+1; j<5; ++j)
            g.add_edge(i, j);
            
    EXPECT_FALSE(is_planar(g));
    auto faces = get_planar_faces(g);
    EXPECT_TRUE(faces.empty());
}

TEST(PlanarityTest, K33) {
    // K3,3 is NOT planar.
    Graph g(6);
    // Bipartite partitions {0,1,2} and {3,4,5}
    for(int i=0; i<3; ++i)
        for(int j=3; j<6; ++j)
            g.add_edge(i, j);
            
    EXPECT_FALSE(is_planar(g));
    auto faces = get_planar_faces(g);
    EXPECT_TRUE(faces.empty());
}

TEST(PlanarityTest, Tree) {
    // Trees are planar.
    Graph g(5);
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(1, 3);
    g.add_edge(1, 4);
    
    EXPECT_TRUE(is_planar(g));
    // For a tree, our implementation returns a single "outer" face/walk.
    // Faces = E - V + 2 = 4 - 5 + 2 = 1.
    auto faces = get_planar_faces(g);
    EXPECT_EQ(faces.size(), 1);
}

TEST(PlanarityTest, DisconnectedPlanar) {
    // Two K3s
    Graph g(6);
    g.add_edge(0, 1); g.add_edge(1, 2); g.add_edge(2, 0);
    g.add_edge(3, 4); g.add_edge(4, 5); g.add_edge(5, 3);
    
    EXPECT_TRUE(is_planar(g));
    // Each component has V=3, E=3 -> F=2.
    // Total F = 2 + 2 = 4?
    // Note: get_planar_faces returns faces for each component.
    // It concatenates them.
    auto faces = get_planar_faces(g);
    EXPECT_EQ(faces.size(), 4);
}
