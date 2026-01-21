#include <gtest/gtest.h>
#include "graphlib/graph_core.h"
#include "graphlib/tree.h"
#include <numeric>

using namespace graphlib;

TEST(Invariants, HandshakingLemma) {
    // In any graph, sum of degrees = 2 * |E|
    // For directed, sum(out_degree) = |E|, sum(in_degree) = |E|
    
    // Undirected case
    int n = 20;
    Graph g(n, false);
    // Create random edges
    int m = 0;
    for(int i=0; i<n; ++i) {
        for(int j=i+1; j<n; ++j) {
            if ((i+j) % 3 == 0) {
                g.add_edge(i, j);
                m++;
            }
        }
    }
    
    long long sum_deg = 0;
    for(int i=0; i<n; ++i) {
        Edge* e = g.get_edges(i);
        while(e) {
            sum_deg++;
            e = e->next;
        }
    }
    // Since it's undirected, each edge (u,v) adds 1 to deg(u) and 1 to deg(v) in the adjacency list representation
    // If implementation stores undirected as two directed edges, then sum_deg should be 2*m.
    EXPECT_EQ(sum_deg, 2 * m);
}

TEST(Invariants, DirectedDegreeSum) {
    // For directed graph: sum(out_degree) = |E|
    int n = 20;
    Graph g(n, true);
    int m = 0;
    for(int i=0; i<n; ++i) {
        for(int j=0; j<n; ++j) {
            if (i != j && (i*j) % 7 == 0) {
                g.add_edge(i, j);
                m++;
            }
        }
    }
    
    long long sum_out = 0;
    for(int i=0; i<n; ++i) {
        Edge* e = g.get_edges(i);
        while(e) {
            sum_out++;
            e = e->next;
        }
    }
    EXPECT_EQ(sum_out, m);
}

TEST(Invariants, TreeProperties) {
    // A tree with N vertices has exactly N-1 edges and is connected.
    int n = 50;
    TreeLCA tree(n); // TreeLCA inherits Graph? No, it takes adj list. 
    // Wait, let's use Graph to build a tree first.
    Graph g(n, false);
    for(int i=1; i<n; ++i) {
        g.add_edge(0, i); // Star graph
    }
    
    // Check edge count
    long long edges = 0;
    for(int i=0; i<n; ++i) {
        Edge* e = g.get_edges(i);
        while(e) { edges++; e = e->next; }
    }
    EXPECT_EQ(edges, 2 * (n - 1)); // Undirected doubles
    
    // Check connectivity
    auto dist = bfs_distances(g, 0);
    for(int d : dist) {
        EXPECT_NE(d, -1);
    }
}

TEST(Invariants, CompleteGraphEdges) {
    // Kn has n*(n-1)/2 edges
    int n = 10;
    Graph g(n, false);
    for(int i=0; i<n; ++i) {
        for(int j=i+1; j<n; ++j) {
            g.add_edge(i, j);
        }
    }
    
    long long edge_entries = 0;
    for(int i=0; i<n; ++i) {
        Edge* e = g.get_edges(i);
        while(e) { edge_entries++; e = e->next; }
    }
    EXPECT_EQ(edge_entries, n * (n - 1));
}
