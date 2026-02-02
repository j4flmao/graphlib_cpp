#include "graphlib/clique.h"
#include <algorithm>
#include <set>
#include <vector>

namespace graphlib {

namespace {

// Helper to check if u and v are connected
bool is_connected(const Graph& g, int u, int v) {
    // Linear scan of adjacency list? Or Graph could have adjacency matrix?
    // For small graphs in Clique problems, matrix is faster.
    // If we assume sparse graph representation, Bron-Kerbosch usually uses neighbor lists.
    // P intersect N(u).
    Edge* e = g.get_edges(u);
    while (e) {
        if (e->to == v) return true;
        e = e->next;
    }
    return false;
}

// Optimized helper that uses adjacency list directly
void bron_kerbosch_pivot(const Graph& g, 
                         std::vector<int>& R, 
                         std::vector<int>& P, 
                         std::vector<int>& X, 
                         std::vector<int>& max_clique) {
    
    if (P.empty() && X.empty()) {
        if (R.size() > max_clique.size()) {
            max_clique = R;
        }
        return;
    }
    
    if (P.empty()) return;

    // Pivot selection: choose u in P U X that maximizes |P intersect N(u)|
    // This minimizes the number of recursive calls.
    int pivot = -1;
    int max_intersection = -1;
    
    // Candidates for pivot are P union X
    // We can just iterate P first.
    // Check pivot in P
    for (int u : P) {
         int c = 0;
         for (int v : P) {
             if (is_connected(g, u, v)) c++;
         }
         if (c > max_intersection) {
             max_intersection = c;
             pivot = u;
         }
    }
    // Check pivot in X?
    // Often picking pivot from P U X is better.
    // Let's stick to P for simplicity or check X too.
    for (int u : X) {
        int c = 0;
        for (int v : P) {
            if (is_connected(g, u, v)) c++;
        }
        if (c > max_intersection) {
            max_intersection = c;
            pivot = u;
        }
    }

    // P \ N(pivot)
    std::vector<int> P_copy = P; // iterate over copy because P changes? No, we iterate copy.
    
    for (int v : P_copy) {
        if (is_connected(g, pivot, v)) continue; // Skip neighbors of pivot
        
        // New R = R U {v}
        R.push_back(v);
        
        // New P = P intersect N(v)
        std::vector<int> new_P;
        for (int p_node : P) {
            if (is_connected(g, v, p_node)) {
                new_P.push_back(p_node);
            }
        }
        
        // New X = X intersect N(v)
        std::vector<int> new_X;
        for (int x_node : X) {
            if (is_connected(g, v, x_node)) {
                new_X.push_back(x_node);
            }
        }
        
        bron_kerbosch_pivot(g, R, new_P, new_X, max_clique);
        
        R.pop_back();
        
        // Move v from P to X
        // Remove v from P (slow in vector, acceptable for recursion depth)
        // P is passed by reference but we iterated P_copy.
        // P needs to effectively lose v for next iterations.
        // Erase-remove idiom
        P.erase(std::remove(P.begin(), P.end(), v), P.end());
        X.push_back(v);
    }
}

void bron_kerbosch_all(const Graph& g, 
                       std::vector<int>& R, 
                       std::vector<int>& P, 
                       std::vector<int>& X, 
                       std::vector<std::vector<int>>& cliques) {
    if (P.empty() && X.empty()) {
        cliques.push_back(R);
        return;
    }
    
    if (P.empty()) return;
    
    // Choose pivot to prune search space?
    // For *all* maximal cliques, pivoting is also used to avoid duplicates/subcliques?
    // Tomita's algorithm uses pivoting.
    
    int pivot = P[0]; // Simple pivot or max degree in P U X
    // Finding max degree pivot in P U X
    int max_deg = -1;
    for (int u : P) {
        // Compute intersection size P intersect N(u)
        int c = 0;
        for(int v : P) if(is_connected(g, u, v)) c++;
        if (c > max_deg) { max_deg = c; pivot = u; }
    }
    for (int u : X) {
        int c = 0;
        for(int v : P) if(is_connected(g, u, v)) c++;
        if (c > max_deg) { max_deg = c; pivot = u; }
    }
    
    std::vector<int> P_copy = P;
    for (int v : P_copy) {
        if (is_connected(g, pivot, v)) continue;
        
        R.push_back(v);
        
        std::vector<int> new_P;
        for(int p : P) if(is_connected(g, v, p)) new_P.push_back(p);
        
        std::vector<int> new_X;
        for(int x : X) if(is_connected(g, v, x)) new_X.push_back(x);
        
        bron_kerbosch_all(g, R, new_P, new_X, cliques);
        
        R.pop_back();
        P.erase(std::remove(P.begin(), P.end(), v), P.end());
        X.push_back(v);
    }
}

} // namespace

std::vector<int> maximum_clique(const Graph& g) {
    int n = g.vertex_count();
    std::vector<int> R;
    std::vector<int> P(n);
    std::vector<int> X;
    // Initial P contains all vertices
    for(int i=0; i<n; ++i) P[i] = i;
    
    std::vector<int> max_clique;
    bron_kerbosch_pivot(g, R, P, X, max_clique);
    return max_clique;
}

std::vector<std::vector<int>> find_all_maximal_cliques(const Graph& g) {
    int n = g.vertex_count();
    std::vector<int> R;
    std::vector<int> P(n);
    std::vector<int> X;
    for(int i=0; i<n; ++i) P[i] = i;
    
    std::vector<std::vector<int>> cliques;
    bron_kerbosch_all(g, R, P, X, cliques);
    return cliques;
}

}
