﻿﻿﻿﻿﻿﻿#include <gtest/gtest.h>
#include <graphlib/graphlib.h>
#include <limits>
#include <vector>
#include <tuple>
#include <utility>

using graphlib::ShortestPath;
using graphlib::Graph;
using graphlib::MaxFlow;

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

TEST(ShortestPathTest, JohnsonDetectsNegativeCycle) {
    int n = 3;
    ShortestPath sp(n);

    sp.add_edge(0, 1, 1);
    sp.add_edge(1, 2, -2);
    sp.add_edge(2, 0, -2);

    long long inf = std::numeric_limits<long long>::max() / 4;
    bool has_negative_cycle = false;

    std::vector<std::vector<long long>> dist_johnson = sp.johnson(inf, has_negative_cycle);
    (void)dist_johnson;

    EXPECT_TRUE(has_negative_cycle);
}

TEST(ShortestPathTest, MinimumMeanCycleSimplePositive) {
    int n = 3;
    ShortestPath sp(n);

    sp.add_edge(0, 1, 1);
    sp.add_edge(1, 2, 1);
    sp.add_edge(2, 0, 1);
    sp.add_edge(0, 2, 10);
    sp.add_edge(2, 0, 10);

    bool has_cycle = false;
    long double mean = sp.minimum_mean_cycle(has_cycle);

    EXPECT_TRUE(has_cycle);
    EXPECT_NEAR(static_cast<double>(mean), 1.0, 1e-9);
}

TEST(ShortestPathTest, MinimumMeanCycleNegative) {
    int n = 3;
    ShortestPath sp(n);

    sp.add_edge(0, 1, -2);
    sp.add_edge(1, 2, -2);
    sp.add_edge(2, 0, -2);

    bool has_cycle = false;
    long double mean = sp.minimum_mean_cycle(has_cycle);

    EXPECT_TRUE(has_cycle);
    EXPECT_LT(static_cast<double>(mean), 0.0);
}

TEST(ShortestPathTest, MinimumMeanCycleNoCycle) {
    int n = 3;
    ShortestPath sp(n);

    sp.add_edge(0, 1, 5);
    sp.add_edge(1, 2, 7);

    bool has_cycle = false;
    long double mean = sp.minimum_mean_cycle(has_cycle);
    (void)mean;

    EXPECT_FALSE(has_cycle);
}

TEST(GraphUtilsTest, ConnectedAndTreeDetection) {
    Graph tree(4, false);
    tree.add_edge(0, 1);
    tree.add_edge(1, 2);
    tree.add_edge(1, 3);

    EXPECT_TRUE(graphlib::is_connected(tree));
    EXPECT_TRUE(graphlib::is_tree(tree));

    Graph disconnected(4, false);
    disconnected.add_edge(0, 1);

    EXPECT_FALSE(graphlib::is_connected(disconnected));
    EXPECT_FALSE(graphlib::is_tree(disconnected));
}

TEST(GraphUtilsTest, BuildGraphFromEdgesAndBfsDistances) {
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(0, 1));
    edges.push_back(std::make_pair(1, 2));

    Graph g = graphlib::make_graph_from_edges(3, edges, false);
    std::vector<int> dist = graphlib::bfs_distances(g, 0);

    ASSERT_EQ(static_cast<int>(dist.size()), 3);
    EXPECT_EQ(dist[0], 0);
    EXPECT_EQ(dist[1], 1);
    EXPECT_EQ(dist[2], 2);
}

TEST(GraphUtilsTest, BuildGraphFromEdgesOneBased) {
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(2, 3));

    Graph g = graphlib::make_graph_from_edges_one_based(3, edges, false);
    std::vector<int> dist = graphlib::bfs_distances(g, 0);

    ASSERT_EQ(static_cast<int>(dist.size()), 3);
    EXPECT_EQ(dist[0], 0);
    EXPECT_EQ(dist[1], 1);
    EXPECT_EQ(dist[2], 2);
}

TEST(GraphUtilsTest, BuildWeightedGraphFromEdges) {
    std::vector<std::tuple<int, int, long long>> edges;
    edges.push_back(std::tuple<int, int, long long>(0, 1, 5));

    Graph g = graphlib::make_weighted_graph_from_edges(2, edges, true);
    graphlib::Edge* e = g.get_edges(0);
    ASSERT_NE(e, nullptr);
    EXPECT_EQ(e->to, 1);
    EXPECT_EQ(e->weight, 5);
}

TEST(GraphUtilsTest, BuildWeightedGraphFromEdgesOneBased) {
    std::vector<std::tuple<int, int, long long>> edges;
    edges.push_back(std::tuple<int, int, long long>(1, 2, 7));

    Graph g = graphlib::make_weighted_graph_from_edges_one_based(2, edges, true);
    graphlib::Edge* e = g.get_edges(0);
    ASSERT_NE(e, nullptr);
    EXPECT_EQ(e->to, 1);
    EXPECT_EQ(e->weight, 7);
}

TEST(GraphUtilsTest, BfsDistancesUnreachable) {
    Graph g(3, false);
    g.add_edge(0, 1);

    std::vector<int> dist = graphlib::bfs_distances(g, 0, -1);
    ASSERT_EQ(static_cast<int>(dist.size()), 3);
    EXPECT_EQ(dist[0], 0);
    EXPECT_EQ(dist[1], 1);
    EXPECT_EQ(dist[2], -1);
}

TEST(GraphUtilsTest, MultiSourceBfsDistances) {
    Graph g(5, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(3, 4);

    std::vector<int> sources;
    sources.push_back(0);
    sources.push_back(4);

    std::vector<int> dist = graphlib::bfs_multi_source(g, sources, -1);

    ASSERT_EQ(static_cast<int>(dist.size()), 5);
    EXPECT_EQ(dist[0], 0);
    EXPECT_EQ(dist[1], 1);
    EXPECT_EQ(dist[2], 2);
    EXPECT_EQ(dist[3], 1);
    EXPECT_EQ(dist[4], 0);
}

TEST(GraphUtilsTest, UndirectedEulerTrailAndCycle) {
    Graph path(3, false);
    path.add_edge(0, 1);
    path.add_edge(1, 2);

    EXPECT_TRUE(graphlib::has_eulerian_trail_undirected(path));
    EXPECT_FALSE(graphlib::has_eulerian_cycle_undirected(path));

    std::vector<int> trail = graphlib::eulerian_trail_undirected(path);
    ASSERT_EQ(static_cast<int>(trail.size()), 3);
    bool forward = (trail[0] == 0 && trail[1] == 1 && trail[2] == 2);
    bool backward = (trail[0] == 2 && trail[1] == 1 && trail[2] == 0);
    EXPECT_TRUE(forward || backward);

    Graph cycle(3, false);
    cycle.add_edge(0, 1);
    cycle.add_edge(1, 2);
    cycle.add_edge(2, 0);

    EXPECT_TRUE(graphlib::has_eulerian_trail_undirected(cycle));
    EXPECT_TRUE(graphlib::has_eulerian_cycle_undirected(cycle));

    std::vector<int> cycle_trail = graphlib::eulerian_trail_undirected(cycle);
    ASSERT_EQ(static_cast<int>(cycle_trail.size()), 4);
    EXPECT_EQ(cycle_trail.front(), cycle_trail.back());
}

TEST(GraphUtilsTest, DirectedEulerTrailAndCycle) {
    Graph g(3, true);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);

    EXPECT_TRUE(graphlib::has_eulerian_trail_directed(g));
    EXPECT_TRUE(graphlib::has_eulerian_cycle_directed(g));

    std::vector<int> trail = graphlib::eulerian_trail_directed(g);
    ASSERT_EQ(static_cast<int>(trail.size()), 4);
    EXPECT_EQ(trail.front(), trail.back());

    Graph g2(3, true);
    g2.add_edge(0, 1);
    g2.add_edge(1, 2);

    EXPECT_TRUE(graphlib::has_eulerian_trail_directed(g2));
    EXPECT_FALSE(graphlib::has_eulerian_cycle_directed(g2));

    std::vector<int> trail2 = graphlib::eulerian_trail_directed(g2);
    ASSERT_EQ(static_cast<int>(trail2.size()), 3);
}

TEST(GraphUtilsTest, GlobalMinCutUndirectedBasic) {
    Graph path(4, false);
    path.add_edge(0, 1);
    path.add_edge(1, 2);
    path.add_edge(2, 3);

    long long cut_path = graphlib::global_min_cut_undirected(path);
    EXPECT_EQ(cut_path, 1);

    Graph triangle(3, false);
    triangle.add_edge(0, 1);
    triangle.add_edge(1, 2);
    triangle.add_edge(2, 0);

    long long cut_triangle = graphlib::global_min_cut_undirected(triangle);
    EXPECT_EQ(cut_triangle, 2);

    Graph weighted(4, false);
    weighted.add_edge(0, 1, 3);
    weighted.add_edge(1, 2, 1);
    weighted.add_edge(2, 3, 4);
    weighted.add_edge(0, 3, 2);

    long long cut_weighted = graphlib::global_min_cut_undirected(weighted);
    EXPECT_EQ(cut_weighted, 3);
}

TEST(GraphUtilsTest, GlobalMinCutDirectedThrows) {
    Graph g(3, true);
    g.add_edge(0, 1);
    g.add_edge(1, 2);

    EXPECT_THROW(graphlib::global_min_cut_undirected(g), std::invalid_argument);
}

TEST(GraphUtilsTest, GomoryHuTreeMatchesPairwiseMinCutOnSmallGraph) {
    Graph g(4, false);
    g.add_edge(0, 1, 3);
    g.add_edge(1, 2, 1);
    g.add_edge(2, 3, 4);
    g.add_edge(0, 3, 2);

    std::vector<int> parent;
    std::vector<long long> min_cut;
    graphlib::gomory_hu_tree(g, parent, min_cut);

    ASSERT_EQ(static_cast<int>(parent.size()), 4);
    ASSERT_EQ(static_cast<int>(min_cut.size()), 4);

    std::vector<std::vector<std::pair<int, long long>>> tree_adj(4);
    for (int v = 0; v < 4; v++) {
        int p = parent[v];
        if (p == v) {
            continue;
        }
        tree_adj[v].push_back(std::pair<int, long long>(p, min_cut[v]));
        tree_adj[p].push_back(std::pair<int, long long>(v, min_cut[v]));
    }

    int edge_count = 0;
    for (int v = 0; v < 4; v++) {
        edge_count += static_cast<int>(tree_adj[v].size());
    }
    edge_count /= 2;
    EXPECT_EQ(edge_count, 3);

    auto min_edge_on_path = [&](int s, int t) -> long long {
        std::vector<int> par(4, -1);
        std::vector<int> stack;
        stack.push_back(s);
        par[s] = s;
        while (!stack.empty()) {
            int v = stack.back();
            stack.pop_back();
            if (v == t) {
                break;
            }
            for (std::size_t i = 0; i < tree_adj[v].size(); i++) {
                int to = tree_adj[v][i].first;
                if (par[to] == -1) {
                    par[to] = v;
                    stack.push_back(to);
                }
            }
        }

        long long best = std::numeric_limits<long long>::max();
        int cur = t;
        while (cur != s) {
            int p = par[cur];
            long long w = 0;
            for (std::size_t i = 0; i < tree_adj[cur].size(); i++) {
                if (tree_adj[cur][i].first == p) {
                    w = tree_adj[cur][i].second;
                    break;
                }
            }
            if (w < best) {
                best = w;
            }
            cur = p;
        }
        return best;
    };

    auto pairwise_min_cut = [&](int s, int t) -> long long {
        int n = g.vertex_count();
        std::vector<std::vector<long long>> w(n, std::vector<long long>(n, 0));
        for (int u = 0; u < n; u++) {
            graphlib::Edge* e = g.get_edges(u);
            while (e) {
                int v = e->to;
                if (u <= v && u != v) {
                    long long weight = e->weight;
                    w[u][v] += weight;
                    w[v][u] += weight;
                }
                e = e->next;
            }
        }

        MaxFlow mf(4);
        for (int u = 0; u < 4; u++) {
            for (int v = u + 1; v < 4; v++) {
                long long cap = w[u][v];
                if (cap > 0) {
                    mf.add_undirected_edge(u, v, cap);
                }
            }
        }

        return mf.dinic(s, t);
    };

    for (int s = 0; s < 4; s++) {
        for (int t = s + 1; t < 4; t++) {
            long long cut_tree = min_edge_on_path(s, t);
            long long cut_flow = pairwise_min_cut(s, t);
            EXPECT_EQ(cut_tree, cut_flow);
        }
    }
}

TEST(ShortestPathUtilsTest, ReconstructPathBasic) {
    std::vector<int> parent(3);
    parent[0] = -1;
    parent[1] = 0;
    parent[2] = 1;

    std::vector<int> path = graphlib::reconstruct_path(0, 2, parent);

    ASSERT_EQ(static_cast<int>(path.size()), 3);
    EXPECT_EQ(path[0], 0);
    EXPECT_EQ(path[1], 1);
    EXPECT_EQ(path[2], 2);
}

TEST(ShortestPathUtilsTest, ReconstructPathUnreachable) {
    std::vector<int> parent(3, -1);
    std::vector<int> path = graphlib::reconstruct_path(0, 2, parent);
    EXPECT_TRUE(path.empty());
}

TEST(ShortestPathTest, ZeroOneBfsMatchesDijkstraOnZeroOneGraph) {
    int n = 5;
    ShortestPath sp(n);

    sp.add_edge(0, 1, 1);
    sp.add_edge(1, 2, 0);
    sp.add_edge(0, 3, 1);
    sp.add_edge(3, 4, 1);
    sp.add_edge(2, 4, 0);

    long long inf = std::numeric_limits<long long>::max() / 4;

    std::vector<long long> dist_dij = sp.dijkstra(0, inf);
    std::vector<long long> dist_01 = sp.zero_one_bfs(0, inf);

    ASSERT_EQ(static_cast<int>(dist_dij.size()), n);
    ASSERT_EQ(static_cast<int>(dist_01.size()), n);

    for (int i = 0; i < n; i++) {
        EXPECT_EQ(dist_dij[i], dist_01[i]);
    }
}

TEST(ShortestPathTest, MultiSourceDijkstraNearestSource) {
    int n = 6;
    ShortestPath sp(n);

    sp.add_edge(0, 2, 5);
    sp.add_edge(1, 2, 1);
    sp.add_edge(2, 3, 2);
    sp.add_edge(3, 4, 3);
    sp.add_edge(4, 5, 4);

    std::vector<int> sources;
    sources.push_back(0);
    sources.push_back(1);

    long long inf = std::numeric_limits<long long>::max() / 4;
    std::vector<long long> dist_multi = sp.multi_source_dijkstra(sources, inf);

    ASSERT_EQ(static_cast<int>(dist_multi.size()), n);

    EXPECT_EQ(dist_multi[0], 0);
    EXPECT_EQ(dist_multi[1], 0);
    EXPECT_EQ(dist_multi[2], 1);
    EXPECT_EQ(dist_multi[3], 3);
    EXPECT_EQ(dist_multi[4], 6);
    EXPECT_EQ(dist_multi[5], 10);
}
