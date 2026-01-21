#include <gtest/gtest.h>
#include <graphlib/graphlib.h>
#include <vector>
#include <algorithm>
#include <numeric>
#include <set>

using namespace graphlib;

// -----------------------------------------------------------------------------
// Graph Coloring (Greedy with Welsh-Powell Heuristic)
// -----------------------------------------------------------------------------

// Helper to calculate greedy coloring
// Returns number of colors used and fills colors vector (colors[i] = color of vertex i)
int greedy_coloring(const Graph& g, std::vector<int>& colors) {
    int n = g.vertex_count();
    colors.assign(n, -1);
    
    // Sort vertices by degree (descending) - Welsh-Powell heuristic
    std::vector<int> degree(n, 0);
    std::vector<int> nodes(n);
    for(int i=0; i<n; ++i) {
        nodes[i] = i;
        Edge* e = g.get_edges(i);
        while(e) {
            degree[i]++;
            e = e->next;
        }
    }
    
    std::sort(nodes.begin(), nodes.end(), [&](int a, int b) {
        return degree[a] > degree[b];
    });
    
    int max_color = 0;
    
    for(int u : nodes) {
        // Find first available color
        std::set<int> neighbor_colors;
        Edge* e = g.get_edges(u);
        while(e) {
            if (colors[e->to] != -1) {
                neighbor_colors.insert(colors[e->to]);
            }
            e = e->next;
        }
        
        int color = 0;
        while(neighbor_colors.count(color)) {
            color++;
        }
        
        colors[u] = color;
        max_color = std::max(max_color, color + 1);
    }
    
    return max_color;
}

TEST(ColoringTest, BipartiteIs2Colorable) {
    Graph g(4, false); // Square 0-1-2-3-0
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(3, 0);
    
    std::vector<int> colors;
    int k = greedy_coloring(g, colors);
    
    EXPECT_LE(k, 2); // Should be exactly 2
    for(int i=0; i<4; ++i) EXPECT_GE(colors[i], 0);
    EXPECT_NE(colors[0], colors[1]);
    EXPECT_NE(colors[1], colors[2]);
    EXPECT_NE(colors[2], colors[3]);
    EXPECT_NE(colors[3], colors[0]);
}

TEST(ColoringTest, K4Is4Colorable) {
    Graph g(4, false);
    for(int i=0; i<4; ++i)
        for(int j=i+1; j<4; ++j)
            g.add_edge(i, j);
            
    std::vector<int> colors;
    int k = greedy_coloring(g, colors);
    
    EXPECT_EQ(k, 4);
    std::set<int> distinct_colors(colors.begin(), colors.end());
    EXPECT_EQ(distinct_colors.size(), 4);
}

TEST(ColoringTest, OddCycleIs3Colorable) {
    Graph g(5, false); // C5
    g.add_edge(0, 1); g.add_edge(1, 2);
    g.add_edge(2, 3); g.add_edge(3, 4);
    g.add_edge(4, 0);
    
    std::vector<int> colors;
    int k = greedy_coloring(g, colors);
    
    EXPECT_EQ(k, 3);
}

// -----------------------------------------------------------------------------
// Disjoint Paths (Menger's Theorem Validation)
// -----------------------------------------------------------------------------

TEST(DisjointPathsTest, EdgeDisjointPaths) {
    // Graph with bottleneck
    // S -> A, S -> B
    // A -> T, B -> T
    // Plus A -> B
    // Max flow S->T should be 2.
    
    MaxFlow mf(4);
    mf.add_edge(0, 1, 1); // S->A
    mf.add_edge(0, 2, 1); // S->B
    mf.add_edge(1, 3, 1); // A->T
    mf.add_edge(2, 3, 1); // B->T
    mf.add_edge(1, 2, 1); // A->B
    
    long long flow = mf.dinic(0, 3);
    EXPECT_EQ(flow, 2);
    
    // Check path reconstruction (simulation)
    // If we remove one path, flow should drop by 1
}

TEST(DisjointPathsTest, VertexDisjointPaths) {
    // S -> A, S -> B
    // A -> C, B -> C
    // C -> T
    // Vertex C is bottleneck. Max vertex disjoint paths should be 1.
    // To model vertex capacities, split node C into C_in -> C_out with cap 1.
    
    // Nodes: S=0, T=5
    // A=1, B=2, C_in=3, C_out=4
    
    MaxFlow mf(6);
    
    // Edges with infinite capacity (or large enough)
    long long INF = 100;
    
    // S -> A, S -> B
    mf.add_edge(0, 1, INF);
    mf.add_edge(0, 2, INF);
    
    // A -> C_in, B -> C_in
    mf.add_edge(1, 3, INF);
    mf.add_edge(2, 3, INF);
    
    // C_in -> C_out (Vertex Capacity = 1)
    mf.add_edge(3, 4, 1);
    
    // C_out -> T
    mf.add_edge(4, 5, INF);
    
    long long flow = mf.dinic(0, 5);
    EXPECT_EQ(flow, 1);
}

// -----------------------------------------------------------------------------
// Independent Set (using Max Clique on Complement)
// -----------------------------------------------------------------------------

// Re-implement Max Clique helper (or assume it's available/copy logic)
// Since we are in a separate file, we need to re-implement or include.
// Let's implement a simple backtracking for Independent Set directly.

void max_independent_set_recursive(const Graph& g, int idx, std::vector<int>& current_set, std::vector<int>& max_set) {
    int n = g.vertex_count();
    if (idx == n) {
        if (current_set.size() > max_set.size()) {
            max_set = current_set;
        }
        return;
    }
    
    // Pruning: if current + remaining < max, stop
    if (current_set.size() + (n - idx) <= max_set.size()) {
        return;
    }
    
    // Try including idx
    bool can_include = true;
    for (int u : current_set) {
        // Check if u and idx are connected
        Edge* e = g.get_edges(u);
        while(e) {
            if (e->to == idx) {
                can_include = false;
                break;
            }
            e = e->next;
        }
        if (!can_include) break;
    }
    
    if (can_include) {
        current_set.push_back(idx);
        max_independent_set_recursive(g, idx + 1, current_set, max_set);
        current_set.pop_back();
    }
    
    // Try excluding idx
    max_independent_set_recursive(g, idx + 1, current_set, max_set);
}

TEST(IndependentSetTest, SimpleCycle) {
    Graph g(5, false); // C5: 0-1-2-3-4-0
    g.add_edge(0, 1); g.add_edge(1, 2);
    g.add_edge(2, 3); g.add_edge(3, 4);
    g.add_edge(4, 0);
    
    std::vector<int> current, max_set;
    max_independent_set_recursive(g, 0, current, max_set);
    
    EXPECT_EQ(max_set.size(), 2); // Max IS for C5 is 2
}

TEST(IndependentSetTest, StarGraph) {
    Graph g(6, false); // Center 0, leaves 1-5
    for(int i=1; i<=5; ++i) g.add_edge(0, i);
    
    std::vector<int> current, max_set;
    max_independent_set_recursive(g, 0, current, max_set);
    
    EXPECT_EQ(max_set.size(), 5); // All leaves
}
