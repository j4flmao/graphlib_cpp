#include "graphlib/graph_generator.h"
#include "graphlib/graph_measures.h"
#include "graphlib/connectivity.h"
#include "graphlib/planarity.h"
#include <gtest/gtest.h>
#include <cmath>
#include <queue>

using namespace graphlib;

class GraphGeneratorTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(GraphGeneratorTest, RandomGraphSize) {
    int n = 50;
    double p = 0.1;
    Graph g = generator::random_graph(n, p, false, 42);
    EXPECT_EQ(g.vertex_count(), n);
}

TEST_F(GraphGeneratorTest, CompleteGraph) {
    int n = 10;
    Graph g = generator::complete_graph(n, false);
    EXPECT_EQ(g.vertex_count(), n);
    // Edges = n * (n-1) / 2
    // We can't easily count edges without iterating, or adding edge count to Graph class.
    // Let's verify degrees.
    for (int i = 0; i < n; ++i) {
        int deg = 0;
        auto* e = g.get_edges(i);
        while(e) {
            deg++;
            e = e->next;
        }
        EXPECT_EQ(deg, n - 1);
    }
}

TEST_F(GraphGeneratorTest, CycleGraph) {
    int n = 10;
    Graph g = generator::cycle_graph(n, false);
    EXPECT_EQ(g.vertex_count(), n);
    for (int i = 0; i < n; ++i) {
        int deg = 0;
        auto* e = g.get_edges(i);
        while(e) {
            deg++;
            e = e->next;
        }
        EXPECT_EQ(deg, 2);
    }
}

TEST_F(GraphGeneratorTest, PathGraph) {
    int n = 10;
    Graph g = generator::path_graph(n, false);
    EXPECT_EQ(g.vertex_count(), n);
    // Endpoints degree 1, others 2
    int deg0 = 0;
    auto* e0 = g.get_edges(0);
    while(e0) { deg0++; e0 = e0->next; }
    EXPECT_EQ(deg0, 1);
    
    int deg1 = 0;
    auto* e1 = g.get_edges(1);
    while(e1) { deg1++; e1 = e1->next; }
    EXPECT_EQ(deg1, 2);
}

TEST_F(GraphGeneratorTest, RandomTree) {
    int n = 20;
    Graph g = generator::random_tree(n, 123);
    EXPECT_EQ(g.vertex_count(), n);
    // Tree with n vertices has n-1 edges.
    // Sum of degrees = 2(n-1).
    int sum_deg = 0;
    for (int i = 0; i < n; ++i) {
        auto* e = g.get_edges(i);
        while(e) {
            sum_deg++;
            e = e->next;
        }
    }
    EXPECT_EQ(sum_deg, 2 * (n - 1));
    // Should be connected (implied by construction + edge count, but good to check)
    // We don't have is_connected exposed easily in core, but BFS works.
    std::vector<int> dist = graphlib::eccentricity(g);
    // If connected, no distance is -1 (assuming eccentricity handles disconnect)
    // Or just check min_cut or something.
    // Actually, graph_measures::eccentricity returns -1 for disconnected.
    for(int d : dist) {
        EXPECT_NE(d, -1);
    }
}

TEST_F(GraphGeneratorTest, Grid2D) {
    int rows = 3, cols = 3;
    Graph g = generator::grid_2d_graph(rows, cols);
    EXPECT_EQ(g.vertex_count(), 9);
    // Center node (1,1) -> index 4 should have degree 4
    int deg = 0;
    auto* e = g.get_edges(4);
    while(e) { deg++; e = e->next; }
    EXPECT_EQ(deg, 4);
    
    // Corner (0,0) -> index 0 should have degree 2
    deg = 0;
    e = g.get_edges(0);
    while(e) { deg++; e = e->next; }
    EXPECT_EQ(deg, 2);
}

TEST_F(GraphGeneratorTest, WattsStrogatz) {
    int n = 20;
    int k = 4;
    double p = 0.2;
    Graph g = generator::watts_strogatz(n, k, p, 999);
    EXPECT_EQ(g.vertex_count(), n);
    // Average degree should be k
    int sum_deg = 0;
    for (int i = 0; i < n; ++i) {
        auto* e = g.get_edges(i);
        while(e) {
            sum_deg++;
            e = e->next;
        }
    }
    EXPECT_EQ(sum_deg, n * k);
}

TEST_F(GraphGeneratorTest, BarabasiAlbert) {
    int n = 30;
    int m = 3;
    Graph g = generator::barabasi_albert(n, m, 555);
    EXPECT_EQ(g.vertex_count(), n);
    // Edges added:
    // First m nodes: complete graph K_m -> m(m-1)/2 edges
    // Next n-m nodes: each adds m edges -> (n-m)m edges
    // Total edges E = m(m-1)/2 + m(n-m) = m(m-1 + 2n - 2m)/2 = m(2n - m - 1)/2
    // Sum of degrees = 2E = m(2n - m - 1)
    
    int sum_deg = 0;
    for (int i = 0; i < n; ++i) {
        auto* e = g.get_edges(i);
        while(e) {
            sum_deg++;
            e = e->next;
        }
    }
    EXPECT_EQ(sum_deg, m * (2 * n - m - 1));
}

TEST_F(GraphGeneratorTest, RandomTreePrufer) {
    int n = 20;
    Graph g = generator::random_tree_prufer(n, 123);
    EXPECT_EQ(g.vertex_count(), n);
    
    int edge_count = 0;
    for (int i = 0; i < n; ++i) {
        auto* e = g.get_edges(i);
        while(e) {
            edge_count++;
            e = e->next;
        }
    }
    // Undirected graph, each edge counted twice in adjacency list
    EXPECT_EQ(edge_count, 2 * (n - 1));
    
    // Check connectivity using BFS from node 0
    std::vector<bool> visited(n, false);
    std::queue<int> q;
    q.push(0);
    visited[0] = true;
    int visited_count = 0;
    while(!q.empty()) {
        int u = q.front();
        q.pop();
        visited_count++;
        auto* e = g.get_edges(u);
        while(e) {
            if (!visited[e->to]) {
                visited[e->to] = true;
                q.push(e->to);
            }
            e = e->next;
        }
    }
    EXPECT_EQ(visited_count, n);
}

TEST_F(GraphGeneratorTest, RMATGraph) {
    int n = 32; // Power of 2
    int m = 100;
    // Standard R-MAT parameters
    double a = 0.57, b = 0.19, c = 0.19, d = 0.05;
    Graph g = generator::rmat_graph(n, m, a, b, c, d, 12345);
    
    EXPECT_GE(g.vertex_count(), n); // Might be slightly larger due to padding to power of 2
    
    // Count edges (might be less than m if duplicate edges were generated and removed, 
    // but rmat_graph implementation usually keeps duplicates or removes them? 
    // My implementation removes duplicates using std::set.)
    // So edge count <= 2*m (undirected)
    
    long long actual_edges = 0;
    for (int i = 0; i < g.vertex_count(); ++i) {
        auto* e = g.get_edges(i);
        while(e) {
            actual_edges++;
            e = e->next;
        }
    }
    // It's undirected, so sum of degrees is 2 * |E|
    // Implementation generates m edges. If unique, then 2*m.
    // It's likely slightly less.
    EXPECT_LE(actual_edges, 2 * m);
    EXPECT_GT(actual_edges, 0); 
}

TEST_F(GraphGeneratorTest, RandomGeometricGraph) {
    int n = 50;
    double radius = 0.2;
    Graph g = generator::random_geometric_graph(n, radius, 2, 9876);
    EXPECT_EQ(g.vertex_count(), n);
    
    // Just verify graph structure validity
    for (int i = 0; i < n; ++i) {
        auto* e = g.get_edges(i);
        while(e) {
            EXPECT_GE(e->to, 0);
            EXPECT_LT(e->to, n);
            e = e->next;
        }
    }
}
