#include <gtest/gtest.h>
#include <graphlib/graphlib.h>
#include <graphlib/shortest_path.h>
#include <stdexcept>

using graphlib::ShortestPath;
using graphlib::Graph;

TEST(Robustness, ShortestPathInvalidSource) {
    ShortestPath sp(5);
    long long inf = 1e18;
    
    EXPECT_THROW(sp.dijkstra(-1, inf), std::out_of_range);
    EXPECT_THROW(sp.dijkstra(5, inf), std::out_of_range);
}

TEST(Robustness, GraphNegativeVertices) {
    // Usually standard library vector throws bad_alloc or similar if size is huge negative,
    // but our Graph constructor takes int n.
    // If n < 0, vector constructor might throw std::length_error or bad_array_new_length.
    
    EXPECT_ANY_THROW({
        Graph g(-5);
    });
}

TEST(Robustness, ZeroVertices) {
    // Graph constructor throws invalid_argument for n <= 0
    EXPECT_THROW({
        Graph g(0);
    }, std::invalid_argument);
}

TEST(Robustness, LargeGraph) {
    // Not huge, but enough to trigger some depth/memory usage.
    int n = 1000;
    Graph g(n, false);
    for(int i=0; i<n-1; ++i) {
        g.add_edge(i, i+1);
    }
    EXPECT_TRUE(graphlib::is_connected(g));
    
    auto dists = graphlib::bfs_distances(g, 0);
    EXPECT_EQ(dists[n-1], n-1);
}

TEST(Robustness, DisconnectedComponentsEulerian) {
    // 0-1 and 2-3. Disconnected.
    // Even if degrees are even, it's not Eulerian cycle (must be one component with edges).
    Graph g(4, false);
    g.add_edge(0, 1);
    g.add_edge(1, 0);
    g.add_edge(2, 3);
    g.add_edge(3, 2);
    
    EXPECT_FALSE(graphlib::has_eulerian_cycle_undirected(g));
}
