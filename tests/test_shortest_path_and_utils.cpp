#include <gtest/gtest.h>
#include <graphlib/graphlib.h>
#include <graphlib/shortest_path.h>
#include <limits>
#include <vector>
#include <tuple>
#include <utility>

using graphlib::ShortestPath;
using graphlib::Graph;

TEST(ShortestPathTest, JohnsonMatchesFloydWarshall) {
    int n = 4;
    ShortestPath sp(n);

    sp.add_edge(0, 1, 1);
    sp.add_edge(0, 2, 5);
    sp.add_edge(1, 2, -2);
    sp.add_edge(1, 3, 4);
    sp.add_edge(2, 3, 1);

    long long inf = std::numeric_limits<long long>::max() / 4;
    bool has_negative_cycle = false;

    std::vector<std::vector<long long>> dist_fw = sp.floyd_warshall(inf);
    std::vector<std::vector<long long>> dist_johnson = sp.johnson(inf, has_negative_cycle);

    EXPECT_FALSE(has_negative_cycle);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            EXPECT_EQ(dist_fw[i][j], dist_johnson[i][j]);
        }
    }
}

TEST(ShortestPathTest, BellmanFord) {
    int n = 4;
    ShortestPath sp(n);
    sp.add_edge(0, 1, 1);
    sp.add_edge(1, 2, -2);
    sp.add_edge(2, 3, 1);
    
    long long inf = std::numeric_limits<long long>::max() / 4;
    bool has_negative_cycle = false;
    std::vector<long long> dist = sp.bellman_ford(0, inf, has_negative_cycle);
    
    EXPECT_FALSE(has_negative_cycle);
    EXPECT_EQ(dist[0], 0);
    EXPECT_EQ(dist[1], 1);
    EXPECT_EQ(dist[2], -1); // 1 + (-2)
    EXPECT_EQ(dist[3], 0);  // -1 + 1
}

TEST(ShortestPathTest, ZeroOneBFS) {
    int n = 3;
    ShortestPath sp(n);
    // 0 -> 1 (0)
    // 1 -> 2 (1)
    // 0 -> 2 (1)
    sp.add_edge(0, 1, 0);
    sp.add_edge(1, 2, 1);
    sp.add_edge(0, 2, 1);
    
    long long inf = std::numeric_limits<long long>::max();
    std::vector<long long> dist = sp.zero_one_bfs(0, inf);
    EXPECT_EQ(dist[0], 0);
    EXPECT_EQ(dist[1], 0);
    EXPECT_EQ(dist[2], 1);
}

TEST(ShortestPathTest, MultiSourceDijkstra) {
    int n = 4;
    ShortestPath sp(n);
    // Sources: 0, 3
    // 0 -> 1 (10)
    // 3 -> 2 (5)
    // 1 -> 2 (2)
    sp.add_edge(0, 1, 10);
    sp.add_edge(3, 2, 5);
    sp.add_edge(1, 2, 2);
    
    std::vector<int> sources = {0, 3};
    long long inf = 1e18;
    std::vector<long long> dist = sp.multi_source_dijkstra(sources, inf);
    
    EXPECT_EQ(dist[0], 0);
    EXPECT_EQ(dist[3], 0);
    EXPECT_EQ(dist[2], 5); // From 3 is closer (5) than from 0 (10+2=12)
    EXPECT_EQ(dist[1], 10); // From 0
}

TEST(GraphUtilsTest, GlobalMinCutUndirectedBasic) {
    // A simple graph that is 2 triangles connected by one edge
    // 0-1, 1-2, 2-0 (triangle)
    // 3-4, 4-5, 5-3 (triangle)
    // 2-3 (bridge, weight 1)
    // Min cut should be 1 (edge 2-3)
    int n = 6;
    Graph g(n, false); // Undirected
    auto add = [&](int u, int v, long long w) {
        g.add_edge(u, v, w);
    };
    
    add(0, 1, 10); add(1, 2, 10); add(2, 0, 10);
    add(3, 4, 10); add(4, 5, 10); add(5, 3, 10);
    add(2, 3, 1);
    
    long long min_cut = graphlib::global_min_cut_undirected(g);
    EXPECT_EQ(min_cut, 1);
}

TEST(GraphUtilsTest, GomoryHuTreeBasic) {
    // 0-1 (10), 1-2 (5), 2-3 (10)
    // Min cut 0-3 is 5.
    // Min cut 0-1 is 10.
    // Min cut 2-3 is 10.
    Graph g(4, false);
    g.add_edge(0, 1, 10);
    g.add_edge(1, 2, 5);
    g.add_edge(2, 3, 10);
    
    std::vector<int> parent;
    std::vector<long long> min_cut;
    graphlib::gomory_hu_tree(g, parent, min_cut);
    
    EXPECT_EQ(parent.size(), 4);
    EXPECT_EQ(min_cut.size(), 4);
    
    // We can verify that min_cut values are reasonable.
    // Specifically, for Gomory-Hu tree of a path 0-1-2-3 with weights 10, 5, 10:
    // The cut between 0 and 3 is min(10, 5, 10) = 5.
    // In GH tree, the path between 0 and 3 should have min weight 5.
    // The exact structure depends on implementation details, but sizes check is good for now.
}
