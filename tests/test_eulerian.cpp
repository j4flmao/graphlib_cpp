#include <gtest/gtest.h>
#include <graphlib/graphlib.h>
#include <vector>

using graphlib::Graph;

TEST(EulerianTest, UndirectedTrail) {
    // 0-1-2-3
    // Vertices 1,2 degree 2. 0,3 degree 1.
    // Has trail, no cycle.
    Graph g(4, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    
    EXPECT_TRUE(graphlib::has_eulerian_trail_undirected(g));
    EXPECT_FALSE(graphlib::has_eulerian_cycle_undirected(g));
    
    auto trail = graphlib::eulerian_trail_undirected(g);
    EXPECT_EQ(trail.size(), 4); // 3 edges -> 4 vertices in trail
}

TEST(EulerianTest, UndirectedCycle) {
    // Triangle 0-1-2-0
    Graph g(3, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);
    
    EXPECT_TRUE(graphlib::has_eulerian_trail_undirected(g));
    EXPECT_TRUE(graphlib::has_eulerian_cycle_undirected(g));
    
    auto trail = graphlib::eulerian_trail_undirected(g);
    EXPECT_EQ(trail.size(), 4); // 3 edges -> 4 vertices (start==end)
    EXPECT_EQ(trail.front(), trail.back());
}

TEST(EulerianTest, DirectedTrail) {
    // 0->1->2
    Graph g(3, true);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    
    EXPECT_TRUE(graphlib::has_eulerian_trail_directed(g));
    EXPECT_FALSE(graphlib::has_eulerian_cycle_directed(g));
    
    auto trail = graphlib::eulerian_trail_directed(g);
    EXPECT_EQ(trail.size(), 3);
}

TEST(EulerianTest, DirectedCycle) {
    // 0->1->2->0
    Graph g(3, true);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);
    
    EXPECT_TRUE(graphlib::has_eulerian_trail_directed(g));
    EXPECT_TRUE(graphlib::has_eulerian_cycle_directed(g));
    
    auto trail = graphlib::eulerian_trail_directed(g);
    EXPECT_EQ(trail.size(), 4);
    EXPECT_EQ(trail.front(), trail.back());
}

TEST(EulerianTest, KonigsbergBridges) {
    // 4 nodes, 7 edges. All degrees odd. No Eulerian path.
    Graph g(4, false);
    // 0 has 3 edges
    // 1 has 3 edges
    // 2 has 3 edges
    // 3 has 5 edges (let's simulate something similar)
    // Actually standard Konigsberg:
    // 0 connected to 1 (2 edges), to 2 (2 edges), to 3 (1 edge). deg(0)=5
    // 1 connected to 0 (2), to 3 (1). deg(1)=3
    // 2 connected to 0 (2), to 3 (1). deg(2)=3
    // 3 connected to 0, 1, 2. deg(3)=3.
    // All odd degrees.
    
    g.add_edge(0, 1); g.add_edge(0, 1);
    g.add_edge(0, 2); g.add_edge(0, 2);
    g.add_edge(0, 3);
    g.add_edge(1, 3);
    g.add_edge(2, 3);
    
    EXPECT_FALSE(graphlib::has_eulerian_trail_undirected(g));
    EXPECT_FALSE(graphlib::has_eulerian_cycle_undirected(g));
    
    auto trail = graphlib::eulerian_trail_undirected(g);
    EXPECT_TRUE(trail.empty());
}
