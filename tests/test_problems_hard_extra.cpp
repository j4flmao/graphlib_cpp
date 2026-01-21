#include <gtest/gtest.h>
#include <graphlib/graphlib.h>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace graphlib;

// --- Helper for Max Independent Set ---
// Max Independent Set in G is Max Clique in Complement(G)

void bron_kerbosch_is(const std::vector<std::vector<bool>>& adj,
                   std::vector<int>& R, std::vector<int>& P, std::vector<int>& X,
                   int& max_size) {
    if (P.empty() && X.empty()) {
        max_size = std::max(max_size, (int)R.size());
        return;
    }
    
    int pivot = -1;
    if (!P.empty()) pivot = P[0];
    else if (!X.empty()) pivot = X[0];
    
    std::vector<int> P_copy = P;
    for (int v : P_copy) {
        if (pivot != -1 && adj[pivot][v]) continue;
        
        std::vector<int> newR = R; newR.push_back(v);
        std::vector<int> newP, newX;
        
        for (int p : P) if (adj[v][p]) newP.push_back(p);
        for (int x : X) if (adj[v][x]) newX.push_back(x);
        
        bron_kerbosch_is(adj, newR, newP, newX, max_size);
        
        for (auto it = P.begin(); it != P.end(); ++it) {
            if (*it == v) { P.erase(it); break; }
        }
        X.push_back(v);
    }
}

int solve_max_independent_set(const Graph& g) {
    int n = g.vertex_count();
    if (n == 0) return 0;
    
    // Build adjacency matrix of COMPLEMENT graph
    // adj[u][v] = true if NO edge in G (and u != v)
    std::vector<std::vector<bool>> adj(n, std::vector<bool>(n, true));
    for(int i=0; i<n; ++i) adj[i][i] = false;
    
    for (int u = 0; u < n; u++) {
        Edge* e = g.get_edges(u);
        while (e) {
            adj[u][e->to] = false; // Edge exists, so remove from complement
            adj[e->to][u] = false;
            e = e->next;
        }
    }
    
    std::vector<int> R, P, X;
    for (int i = 0; i < n; i++) P.push_back(i);
    
    int max_size = 0;
    bron_kerbosch_is(adj, R, P, X, max_size);
    return max_size;
}

// --- Helper for Graph Coloring ---
// Backtracking for small graphs

bool is_safe(int v, const std::vector<std::vector<int>>& adj, const std::vector<int>& color, int c) {
    for (int u : adj[v]) {
        if (color[u] == c) return false;
    }
    return true;
}

bool graph_coloring_util(int v, int n, int m, const std::vector<std::vector<int>>& adj, std::vector<int>& color) {
    if (v == n) return true;
    
    for (int c = 1; c <= m; c++) {
        if (is_safe(v, adj, color, c)) {
            color[v] = c;
            if (graph_coloring_util(v + 1, n, m, adj, color)) return true;
            color[v] = 0;
        }
    }
    return false;
}

int solve_chromatic_number(const Graph& g) {
    int n = g.vertex_count();
    if (n == 0) return 0;
    
    std::vector<std::vector<int>> adj(n);
    for (int u = 0; u < n; u++) {
        Edge* e = g.get_edges(u);
        while (e) {
            adj[u].push_back(e->to);
            // undirected implied for coloring usually, ensure symmetry if needed
            e = e->next;
        }
    }
    
    // Try m = 1, 2, ... n
    std::vector<int> color(n, 0);
    for (int m = 1; m <= n; m++) {
        if (graph_coloring_util(0, n, m, adj, color)) return m;
    }
    return n;
}

TEST(HardExtraTest, MaxIndependentSet_Cycle5) {
    // Cycle 5: 0-1-2-3-4-0
    // Max IS is 2 (e.g., {0, 2}, {0, 3})
    Graph g(5, false);
    g.add_edge(0, 1); g.add_edge(1, 2);
    g.add_edge(2, 3); g.add_edge(3, 4);
    g.add_edge(4, 0);
    
    EXPECT_EQ(solve_max_independent_set(g), 2);
}

TEST(HardExtraTest, MaxIndependentSet_Cycle6) {
    // Cycle 6: 0-1-2-3-4-5-0
    // Max IS is 3 ({0, 2, 4})
    Graph g(6, false);
    g.add_edge(0, 1); g.add_edge(1, 2);
    g.add_edge(2, 3); g.add_edge(3, 4);
    g.add_edge(4, 5); g.add_edge(5, 0);
    
    EXPECT_EQ(solve_max_independent_set(g), 3);
}

TEST(HardExtraTest, VertexCover_K4) {
    // K4
    // Max IS is 1. Min VC = 4 - 1 = 3.
    Graph g(4, false);
    for(int i=0; i<4; ++i)
        for(int j=i+1; j<4; ++j)
            g.add_edge(i, j);
            
    int mis = solve_max_independent_set(g);
    EXPECT_EQ(mis, 1);
    int min_vc = 4 - mis;
    EXPECT_EQ(min_vc, 3);
}

TEST(HardExtraTest, ChromaticNumber_K4) {
    Graph g(4, false);
    for(int i=0; i<4; ++i)
        for(int j=i+1; j<4; ++j)
            g.add_edge(i, j);
            
    EXPECT_EQ(solve_chromatic_number(g), 4);
}

TEST(HardExtraTest, ChromaticNumber_Bipartite) {
    // 0-1, 1-2, 2-3. Bipartite. Chi = 2.
    Graph g(4, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    
    EXPECT_EQ(solve_chromatic_number(g), 2);
}

TEST(HardExtraTest, ChromaticNumber_Cycle5) {
    // Odd cycle. Chi = 3.
    Graph g(5, false);
    g.add_edge(0, 1); g.add_edge(1, 2);
    g.add_edge(2, 3); g.add_edge(3, 4);
    g.add_edge(4, 0);
    
    EXPECT_EQ(solve_chromatic_number(g), 3);
}

TEST(HardExtraTest, ChromaticNumber_Wheel) {
    // Wheel graph W5 (center + 4 rim)
    // Rim is C4 (even cycle, 2 colors). Center connected to all (needs new color).
    // Chi = 3.
    Graph g(5, false);
    // Rim 1-2-3-4-1
    g.add_edge(1, 2); g.add_edge(2, 3);
    g.add_edge(3, 4); g.add_edge(4, 1);
    // Center 0 connected to 1,2,3,4
    g.add_edge(0, 1); g.add_edge(0, 2);
    g.add_edge(0, 3); g.add_edge(0, 4);
    
    EXPECT_EQ(solve_chromatic_number(g), 3);
}

TEST(HardExtraTest, ChromaticNumber_Wheel_OddRim) {
    // Wheel W6 (center + 5 rim)
    // Rim C5 needs 3 colors. Center needs 4th color.
    // Chi = 4.
    Graph g(6, false);
    // Rim 1-2-3-4-5-1
    g.add_edge(1, 2); g.add_edge(2, 3);
    g.add_edge(3, 4); g.add_edge(4, 5);
    g.add_edge(5, 1);
    // Center 0
    for(int i=1; i<=5; ++i) g.add_edge(0, i);
    
    EXPECT_EQ(solve_chromatic_number(g), 4);
}
