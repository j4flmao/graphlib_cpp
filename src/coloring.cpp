#include "graphlib/coloring.h"
#include <algorithm>
#include <set>
#include <map>
#include <iostream>

namespace graphlib {

// Helper to check if vertex 'u' can be colored with 'c'
static bool can_color(const Graph& g, int u, int c, const std::vector<int>& colors) {
    Edge* e = g.get_edges(u);
    while (e) {
        if (colors[e->to] == c) return false;
        e = e->next;
    }
    return true;
}

std::vector<int> greedy_coloring(const Graph& g) {
    // DSATUR (Degree of Saturation) Heuristic
    int n = g.vertex_count();
    std::vector<int> colors(n, -1);
    std::vector<int> saturation(n, 0); // Number of distinct colors in neighborhood
    std::vector<int> degree(n, 0);
    
    // Calculate degrees
    for(int i=0; i<n; ++i) {
        int d = 0;
        Edge* e = g.get_edges(i);
        while(e) { d++; e = e->next; }
        degree[i] = d;
    }
    
    std::set<int> uncolored;
    for(int i=0; i<n; ++i) uncolored.insert(i);
    
    while (!uncolored.empty()) {
        // Pick vertex with max saturation, break ties with max degree
        int best_u = -1;
        int max_sat = -1;
        int max_deg = -1;
        
        for (int u : uncolored) {
            if (saturation[u] > max_sat) {
                max_sat = saturation[u];
                max_deg = degree[u];
                best_u = u;
            } else if (saturation[u] == max_sat) {
                if (degree[u] > max_deg) {
                    max_deg = degree[u];
                    best_u = u;
                }
            }
        }
        
        if (best_u == -1) best_u = *uncolored.begin(); // Should not happen
        
        // Assign smallest available color
        int c = 0;
        while (true) {
            if (can_color(g, best_u, c, colors)) {
                colors[best_u] = c;
                break;
            }
            c++;
        }
        
        uncolored.erase(best_u);
        
        // Update neighbors' saturation
        Edge* e = g.get_edges(best_u);
        while (e) {
            int v = e->to;
            if (colors[v] == -1) {
                // Check if color c is already known to v
                bool known = false;
                // We'd strictly need to store sets of neighbor colors.
                // Recomputing saturation is O(deg).
                // Or simplified DSATUR: Update neighbor sets.
                // Let's recompute saturation for v, or maintain neighbor sets.
                // Maintaining sets is better.
                // But simplified: Just check neighbors of v.
            }
            e = e->next;
        }
        
        // Exact DSATUR update is slightly complex.
        // Recomputing sat for ALL uncolored neighbors is simplest for implementation.
        // Optimization: Maintain `std::vector<std::set<int>> adj_colors(n)`.
        
    }
    
    // Re-implementation with adjacency color sets for O(N^2) or O(M log N)
    std::fill(colors.begin(), colors.end(), -1);
    std::vector<std::set<int>> adj_colors(n);
    // Uncolored set
    uncolored.clear();
    for(int i=0; i<n; ++i) uncolored.insert(i);
    
    while(!uncolored.empty()) {
        int best_u = -1;
        int max_sat = -1;
        int max_deg = -1;
        
        for (int u : uncolored) {
            int sat = (int)adj_colors[u].size();
            if (sat > max_sat) {
                max_sat = sat;
                max_deg = degree[u];
                best_u = u;
            } else if (sat == max_sat) {
                 if (degree[u] > max_deg) {
                     max_deg = degree[u];
                     best_u = u;
                 }
            }
        }
        
        int c = 0;
        while (true) {
             // Check if c is in adj_colors[best_u]
             if (adj_colors[best_u].find(c) == adj_colors[best_u].end()) {
                 colors[best_u] = c;
                 break;
             }
             c++;
        }
        
        uncolored.erase(best_u);
        
        Edge* e = g.get_edges(best_u);
        while(e) {
            int v = e->to;
            if (colors[v] == -1) {
                adj_colors[v].insert(c);
            }
            e = e->next;
        }
    }
    
    return colors;
}

// Backtracking for Exact Chromatic Number
static void color_backtrack(const Graph& g, int idx, int n, int num_colors_used, std::vector<int>& colors, int& min_colors) {
    if (num_colors_used >= min_colors) return; // Pruning
    
    if (idx == n) {
        min_colors = std::min(min_colors, num_colors_used);
        return;
    }
    
    // Try to color vertex idx with colors 0 to num_colors_used-1
    for (int c = 0; c < num_colors_used; ++c) {
        if (can_color(g, idx, c, colors)) {
            colors[idx] = c;
            color_backtrack(g, idx + 1, n, num_colors_used, colors, min_colors);
            colors[idx] = -1;
        }
    }
    
    // Try new color
    if (num_colors_used < min_colors - 1) {
        colors[idx] = num_colors_used;
        color_backtrack(g, idx + 1, n, num_colors_used + 1, colors, min_colors);
        colors[idx] = -1;
    }
}

GRAPHLIB_API int chromatic_number(const Graph& g) {
    int n = g.vertex_count();
    if (n == 0) return 0;
    if (n == 1) return 1;
    
    // Check if graph has any edges
    bool has_edges = false;
    for (int i = 0; i < n && !has_edges; ++i) {
        if (g.get_edges(i) != nullptr) has_edges = true;
    }
    if (!has_edges) return 1;
    
    // Initial bound from greedy
    std::vector<int> greedy_colors = greedy_coloring(g);
    int max_c = 0;
    for(int c : greedy_colors) max_c = std::max(max_c, c);
    
    int min_colors = max_c + 1; // 1-based count (greedy upper bound)
    
    std::vector<int> colors(n, -1);
    
    colors[0] = 0;
    color_backtrack(g, 1, n, 1, colors, min_colors);
    
    return min_colors;
}

}
