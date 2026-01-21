#include "graphlib/graph_core.h"
#include "graphlib/shortest_path.h"
#include "graphlib/general_matching.h"
#include <gtest/gtest.h>
#include <vector>
#include <numeric>
#include <limits>
#include <iostream>
#include <stdexcept>

using namespace graphlib;

// Helper to calculate Chinese Postman Problem cost
// Returns -1 if graph is not connected
long long solve_chinese_postman(const Graph& g) {
    int n = g.vertex_count();
    
    // 1. Calculate sum of all edge weights
    // Since edges are undirected, get_edges returns each edge twice.
    // We sum all and divide by 2.
    long long total_weight = 0;
    std::vector<int> degree(n, 0);
    
    for (int u = 0; u < n; u++) {
        Edge* e = g.get_edges(u);
        while (e) {
            total_weight += e->weight;
            degree[u]++;
            e = e->next;
        }
    }
    total_weight /= 2;
    
    // 2. Identify odd-degree vertices
    std::vector<int> odd_vertices;
    for (int i = 0; i < n; i++) {
        if (degree[i] % 2 != 0) {
            odd_vertices.push_back(i);
        }
    }
    
    // If no odd vertices, it's Eulerian
    if (odd_vertices.empty()) {
        return total_weight;
    }
    
    // 3. Compute all-pairs shortest paths between odd-degree vertices
    // We can use repeated Dijkstra or Floyd-Warshall.
    // Since we need distances between specific nodes, we can just run Dijkstra from each odd node.
    // However, GraphLib's ShortestPath class might be useful.
    
    int k = odd_vertices.size();
    // k must be even
    if (k % 2 != 0) return -1; // Should not happen in connected undirected graph
    
    // Create a complete graph for matching
    // We want Minimum Weight Perfect Matching.
    // GeneralMatching finds Maximum Weight Matching.
    // Transformation: w'(u,v) = M - w(u,v) where M is large.
    
    GeneralMatching gm(k);
    long long max_dist = 0;
    
    std::vector<std::vector<long long>> dist_matrix(k, std::vector<long long>(k));
    
    ShortestPath sp(n);
    for (int u = 0; u < n; u++) {
        Edge* e = g.get_edges(u);
        while (e) {
            sp.add_edge(u, e->to, e->weight);
            e = e->next;
        }
    }
    
    long long INF = std::numeric_limits<long long>::max();
    
    for (int i = 0; i < k; i++) {
        int u = odd_vertices[i];
        std::vector<long long> dists = sp.dijkstra(u, INF);
        for (int j = 0; j < k; j++) {
            int v = odd_vertices[j];
            dist_matrix[i][j] = dists[v];
            if (dists[v] != INF) {
                max_dist = std::max(max_dist, dists[v]);
            }
        }
    }
    
    long long M = max_dist + 1;
    
    for (int i = 0; i < k; i++) {
        for (int j = i + 1; j < k; j++) {
            long long d = dist_matrix[i][j];
            if (d == std::numeric_limits<long long>::max()) {
                // Graph not connected?
                return -1;
            }
            // Add edge with transformed weight
            gm.add_edge(i, j, M - d);
        }
    }
    
    long long max_matching_weight = gm.maximum_weight_matching();
    
    // Total weight of matching in terms of original distances:
    // We have k/2 edges in perfect matching.
    // Sum(M - d) = (k/2)*M - Sum(d)
    // Sum(d) = (k/2)*M - Sum(M - d)
    
    long long matching_cost = (k / 2) * M - max_matching_weight;
    
    return total_weight + matching_cost;
}

TEST(ChinesePostmanTest, EulerianGraph) {
    Graph g(3, false);
    g.add_edge(0, 1, 5);
    g.add_edge(1, 2, 5);
    g.add_edge(2, 0, 5);
    // All degrees 2. Eulerian. Cost = 15.
    EXPECT_EQ(solve_chinese_postman(g), 15);
}

TEST(ChinesePostmanTest, TwoOddVertices) {
    // 0-1-2-3
    // Edges: (0,1,2), (1,2,3), (2,3,4)
    // Odd: 0, 3.
    // Path 0-3 needs to be doubled.
    // Shortest path 0-3 is 2+3+4=9.
    // Total cost = (2+3+4) + 9 = 18.
    Graph g(4, false);
    g.add_edge(0, 1, 2);
    g.add_edge(1, 2, 3);
    g.add_edge(2, 3, 4);
    EXPECT_EQ(solve_chinese_postman(g), 18);
}

TEST(ChinesePostmanTest, KonigsbergBridges) {
    // 4 nodes, 7 edges.
    // 0 has 3 edges to 1, 2 edges to 2?
    // Let's make a simplified version.
    // 0 connected to 1 (w=1), 1 connected to 2 (w=1), 2 connected to 0 (w=1).
    // Plus 0-3 (w=10).
    // Odd degrees:
    // 0: (0,1), (0,2), (0,3) -> deg 3
    // 1: (1,0), (1,2) -> deg 2
    // 2: (2,0), (2,1) -> deg 2
    // 3: (3,0) -> deg 1
    // Odd vertices: 0, 3.
    // Shortest path 0-3 is 10.
    // Total weight = 1+1+1+10 = 13.
    // Add path 0-3 (cost 10).
    // Total = 13 + 10 = 23.
    Graph g(4, false);
    g.add_edge(0, 1, 1);
    g.add_edge(1, 2, 1);
    g.add_edge(2, 0, 1);
    g.add_edge(0, 3, 10);
    EXPECT_EQ(solve_chinese_postman(g), 23);
}

TEST(ChinesePostmanTest, FourOddVertices) {
    // Square with diagonals
    // 0-1 (1), 1-2 (1), 2-3 (1), 3-0 (1)
    // 0-2 (10), 1-3 (10)
    // Degrees:
    // 0: 3 (1, 3, 2)
    // 1: 3 (0, 2, 3)
    // 2: 3 (1, 3, 0)
    // 3: 3 (2, 0, 1)
    // All 4 are odd.
    // Need to match pairs to minimize cost.
    // Distances:
    // 0-1: 1
    // 0-2: 2 (0-1-2 or 0-3-2), direct is 10.
    // 0-3: 1
    // 1-2: 1
    // 1-3: 2 (1-0-3 or 1-2-3), direct is 10.
    // 2-3: 1
    
    // Possible matchings of {0, 1, 2, 3}:
    // 1. (0,1) and (2,3). Cost 1 + 1 = 2.
    // 2. (0,2) and (1,3). Cost 2 + 2 = 4.
    // 3. (0,3) and (1,2). Cost 1 + 1 = 2.
    // Min added cost is 2.
    // Total original weight: 4*1 (cycle) + 2*10 (diagonals) = 24.
    // Total CPP cost = 24 + 2 = 26.
    
    Graph g(4, false);
    g.add_edge(0, 1, 1);
    g.add_edge(1, 2, 1);
    g.add_edge(2, 3, 1);
    g.add_edge(3, 0, 1);
    g.add_edge(0, 2, 10);
    g.add_edge(1, 3, 10);
    
    EXPECT_EQ(solve_chinese_postman(g), 26);
}

// -----------------------------------------------------------------------------
// Traveling Salesperson Problem (TSP) using Bitmask DP
// -----------------------------------------------------------------------------

long long solve_tsp(const Graph& g) {
    int n = g.vertex_count();
    if (n == 0) return 0;
    if (n == 1) return 0;
    if (n > 20) return -1; // Too large for bitmask DP
    
    // Build adjacency matrix
    long long INF = 1e15; // Sufficiently large
    std::vector<std::vector<long long>> dist(n, std::vector<long long>(n, INF));
    
    for(int u=0; u<n; ++u) {
        Edge* e = g.get_edges(u);
        while(e) {
            // Keep minimum weight if multi-edges
            if (e->weight < dist[u][e->to]) {
                dist[u][e->to] = e->weight;
            }
            e = e->next;
        }
    }
    
    // dp[mask][last_visited]
    std::vector<std::vector<long long>> dp(1 << n, std::vector<long long>(n, INF));
    
    // Start at vertex 0
    dp[1][0] = 0;
    
    for(int mask=1; mask < (1<<n); ++mask) {
        for(int u=0; u<n; ++u) {
            if((mask >> u) & 1) {
                if(dp[mask][u] == INF) continue;
                
                for(int v=0; v<n; ++v) {
                    if(!((mask >> v) & 1)) {
                        if(dist[u][v] != INF) {
                            int next_mask = mask | (1 << v);
                            dp[next_mask][v] = std::min(dp[next_mask][v], dp[mask][u] + dist[u][v]);
                        }
                    }
                }
            }
        }
    }
    
    long long ans = INF;
    int full_mask = (1 << n) - 1;
    // Try to return to 0 from any node
    for(int u=1; u<n; ++u) {
        if(dp[full_mask][u] != INF && dist[u][0] != INF) {
            ans = std::min(ans, dp[full_mask][u] + dist[u][0]);
        }
    }
    
    return ans == INF ? -1 : ans;
}

TEST(TSPTest, SmallCompleteGraph) {
    Graph g(4);
    // 0--10--1
    // |      |
    // 20     20
    // |      |
    // 3--10--2
    // diagonals: sqrt(500) approx 22 -> let's say 25
    
    g.add_edge(0, 1, 10); g.add_edge(1, 0, 10);
    g.add_edge(1, 2, 20); g.add_edge(2, 1, 20);
    g.add_edge(2, 3, 10); g.add_edge(3, 2, 10);
    g.add_edge(3, 0, 20); g.add_edge(0, 3, 20);
    
    // Add expensive diagonals
    g.add_edge(0, 2, 25); g.add_edge(2, 0, 25);
    g.add_edge(1, 3, 25); g.add_edge(3, 1, 25);
    
    // Optimal: 0-1-2-3-0 cost 10+20+10+20 = 60
    EXPECT_EQ(solve_tsp(g), 60);
}

TEST(TSPTest, DisconnectedGraph) {
    Graph g(4);
    g.add_edge(0, 1, 1); g.add_edge(1, 0, 1);
    g.add_edge(2, 3, 1); g.add_edge(3, 2, 1);
    // 0-1 and 2-3 are disconnected
    EXPECT_EQ(solve_tsp(g), -1);
}

TEST(TSPTest, SingleVertex) {
    Graph g(1);
    EXPECT_EQ(solve_tsp(g), 0);
}

TEST(TSPTest, Triangle) {
    Graph g(3);
    g.add_edge(0, 1, 10); g.add_edge(1, 0, 10);
    g.add_edge(1, 2, 15); g.add_edge(2, 1, 15);
    g.add_edge(2, 0, 20); g.add_edge(0, 2, 20);
    EXPECT_EQ(solve_tsp(g), 45);
}

// -----------------------------------------------------------------------------
// Maximum Clique (Bron-Kerbosch Algorithm)
// -----------------------------------------------------------------------------

void bron_kerbosch(const std::vector<std::vector<bool>>& adj,
                   std::vector<int>& R, std::vector<int>& P, std::vector<int>& X,
                   int& max_size) {
    if (P.empty() && X.empty()) {
        max_size = std::max(max_size, (int)R.size());
        return;
    }

    // Pivot selection to minimize branching
    int pivot = -1;
    // Choose pivot from P U X with max neighbors in P
    // ... Simplified: just take first in P or X
    // Optimization: pivot u in P U X
    // Recurse for v in P \ N(u)
    
    if (!P.empty()) pivot = P[0];
    else if (!X.empty()) pivot = X[0];
    
    std::vector<int> P_copy = P;
    
    for (int v : P_copy) {
        if (pivot != -1 && adj[pivot][v]) continue; // Skip neighbors of pivot
        
        std::vector<int> newR = R;
        newR.push_back(v);
        
        std::vector<int> newP, newX;
        
        for (int p : P) if (adj[v][p]) newP.push_back(p);
        for (int x : X) if (adj[v][x]) newX.push_back(x);
        
        bron_kerbosch(adj, newR, newP, newX, max_size);
        
        // Move v from P to X
        // Remove v from P (linear scan ok for small graphs)
        for (auto it = P.begin(); it != P.end(); ++it) {
            if (*it == v) {
                P.erase(it);
                break;
            }
        }
        X.push_back(v);
    }
}

int solve_max_clique(const Graph& g) {
    int n = g.vertex_count();
    if (n == 0) return 0;
    
    std::vector<std::vector<bool>> adj(n, std::vector<bool>(n, false));
    for (int u = 0; u < n; u++) {
        Edge* e = g.get_edges(u);
        while (e) {
            adj[u][e->to] = true;
            // Assuming undirected graph for clique
            e = e->next;
        }
    }
    
    std::vector<int> R, P, X;
    for (int i = 0; i < n; i++) P.push_back(i);
    
    int max_size = 0;
    bron_kerbosch(adj, R, P, X, max_size);
    return max_size;
}

TEST(MaxCliqueTest, K4) {
    Graph g(4, false);
    // Complete graph K4
    for(int i=0; i<4; ++i)
        for(int j=i+1; j<4; ++j)
            g.add_edge(i, j, 1);
            
    EXPECT_EQ(solve_max_clique(g), 4);
}

TEST(MaxCliqueTest, K3_plus_isolated) {
    Graph g(5, false);
    // 0-1-2 triangle
    g.add_edge(0, 1, 1);
    g.add_edge(1, 2, 1);
    g.add_edge(2, 0, 1);
    // 3-4 edge
    g.add_edge(3, 4, 1);
    
    EXPECT_EQ(solve_max_clique(g), 3);
}

TEST(MaxCliqueTest, EmptyGraph) {
    Graph g(5, false);
    EXPECT_EQ(solve_max_clique(g), 1); // Single node is a clique of size 1
}

TEST(MaxCliqueTest, ZeroNodes) {
    EXPECT_THROW({
        Graph g(0, false);
    }, std::invalid_argument);
}
