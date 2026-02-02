#include "graphlib/tree_algo.h"
#include <algorithm>
#include <queue>
#include <functional>

namespace graphlib {

// Re-using the implementation from isomorphism.cpp if linker allows, 
// otherwise we can duplicate or move to graph_core. 
// Ideally get_tree_centers should be in a common place.
// Since it was static/internal in isomorphism, I'll implement it here or expose it.
// I exposed it in previous step in isomorphism.h? No, I implemented it inside isomorphism.cpp and exposed it via header?
// Let's check isomorphism.h from previous steps. 
// Ah, I see I modified isomorphism.cpp to include get_tree_centers but did I expose it in isomorphism.h?
// I only exposed is_tree_isomorphic.
// So I will implement it here fully.

GRAPHLIB_API std::vector<int> get_tree_centers(const Graph& tree) {
    int n = tree.vertex_count();
    if (n == 0) return {};
    if (n == 1) return {0};
    
    std::vector<int> degree(n, 0);
    std::vector<int> leaves;
    for(int i=0; i<n; ++i) {
        int d = 0;
        for(Edge* e = tree.get_edges(i); e; e = e->next) d++;
        degree[i] = d;
        if (d <= 1) leaves.push_back(i);
    }
    
    int remaining = n;
    while (remaining > 2) {
        remaining -= (int)leaves.size();
        std::vector<int> next_leaves;
        for(int leaf : leaves) {
            degree[leaf] = 0; 
            for(Edge* e = tree.get_edges(leaf); e; e = e->next) {
                int v = e->to;
                if (degree[v] > 0) {
                    degree[v]--;
                    if (degree[v] == 1) next_leaves.push_back(v);
                }
            }
        }
        leaves = next_leaves;
    }
    return leaves;
}


GRAPHLIB_API TreeDiameterResult get_tree_diameter(const Graph& tree) {
    int n = tree.vertex_count();
    if (n == 0) return {-1, -1, 0, {}};
    if (n == 1) return {0, 0, 0, {0}};

    // Double BFS
    auto bfs = [&](int start) {
        std::vector<int> dist(n, -1);
        std::vector<int> parent(n, -1);
        std::queue<int> q;
        q.push(start);
        dist[start] = 0;
        int farthest = start;
        
        while(!q.empty()) {
            int u = q.front();
            q.pop();
            if (dist[u] > dist[farthest]) farthest = u;
            
            for(Edge* e = tree.get_edges(u); e; e = e->next) {
                int v = e->to;
                if (dist[v] == -1) {
                    dist[v] = dist[u] + 1;
                    parent[v] = u;
                    q.push(v);
                }
            }
        }
        return std::make_pair(farthest, parent);
    };
    
    auto p1 = bfs(0);
    int u = p1.first;
    auto p2 = bfs(u);
    int v = p2.first;
    std::vector<int> parent = p2.second;
    
    std::vector<int> path;
    int curr = v;
    while(curr != -1) {
        path.push_back(curr);
        if (curr == u) break;
        curr = parent[curr];
    }
    
    return {u, v, (int)path.size() - 1, path};
}

GRAPHLIB_API long long max_weight_independent_set_tree(const Graph& tree, const std::vector<long long>& weights, std::vector<int>& selected_nodes) {
    int n = tree.vertex_count();
    if (n == 0) return 0;
    
    // DP:
    // dp[u][0]: max weight in subtree u if u is NOT picked.
    // dp[u][1]: max weight in subtree u if u IS picked.
    
    // dp[u][0] = sum(max(dp[v][0], dp[v][1])) for v in children
    // dp[u][1] = weight[u] + sum(dp[v][0]) for v in children
    
    std::vector<long long> dp0(n, 0);
    std::vector<long long> dp1(n, 0);
    std::vector<int> parent(n, -1);
    
    // DFS traversal order (post-order needed)
    std::vector<int> order;
    std::vector<bool> visited(n, false);
    
    std::function<void(int)> dfs = [&](int u) {
        visited[u] = true;
        for(Edge* e = tree.get_edges(u); e; e = e->next) {
            int v = e->to;
            if (!visited[v]) {
                parent[v] = u;
                dfs(v);
            }
        }
        order.push_back(u);
    };
    
    dfs(0); // Assuming connected
    
    for (int u : order) {
        dp1[u] = weights[u];
        dp0[u] = 0;
        
        for(Edge* e = tree.get_edges(u); e; e = e->next) {
            int v = e->to;
            if (parent[v] == u) { // v is child
                dp1[u] += dp0[v];
                dp0[u] += std::max(dp0[v], dp1[v]);
            }
        }
    }
    
    long long total = std::max(dp0[0], dp1[0]);
    
    // Reconstruct
    selected_nodes.clear();
    std::function<void(int, bool)> reconstruct = [&](int u, bool picked) {
         if (picked) {
             selected_nodes.push_back(u);
             for(Edge* e = tree.get_edges(u); e; e = e->next) {
                 int v = e->to;
                 if (parent[v] == u) {
                     reconstruct(v, false);
                 }
             }
         } else {
             for(Edge* e = tree.get_edges(u); e; e = e->next) {
                 int v = e->to;
                 if (parent[v] == u) {
                     // Pick v if dp1[v] > dp0[v]
                     if (dp1[v] > dp0[v]) reconstruct(v, true);
                     else reconstruct(v, false);
                 }
             }
         }
    };
    
    if (dp1[0] > dp0[0]) reconstruct(0, true);
    else reconstruct(0, false);
    
    return total;
}

GRAPHLIB_API int min_dominating_set_tree(const Graph& tree, std::vector<int>& selected_nodes) {
    int n = tree.vertex_count();
    if (n == 0) return 0;
    
    // Greedy strategy on tree:
    // Process nodes bottom-up (reverse topological / post-order).
    // If a node is not covered, pick its parent.
    // If root is not covered, pick root.
    
    std::vector<int> order;
    std::vector<int> parent(n, -1);
    std::vector<bool> visited(n, false);
    
    std::function<void(int)> dfs = [&](int u) {
        visited[u] = true;
        for(Edge* e = tree.get_edges(u); e; e = e->next) {
            int v = e->to;
            if (!visited[v]) {
                parent[v] = u;
                dfs(v);
            }
        }
        order.push_back(u);
    };
    
    dfs(0);
    
    selected_nodes.clear();
    std::vector<bool> covered(n, false);
    std::vector<bool> in_set(n, false); // Is node picked?
    
    for (int u : order) {
        if (covered[u]) continue;
        
        // If u is not covered, we must cover it.
        // Best way is to pick parent (covers parent, u, and siblings).
        int p = parent[u];
        if (p != -1) {
            if (!in_set[p]) {
                in_set[p] = true;
                selected_nodes.push_back(p);
                covered[p] = true;
                covered[u] = true; // u covered by p
                if (parent[p] != -1) covered[parent[p]] = true; // p covers its parent too
                // p covers all children of p
                for(Edge* e = tree.get_edges(p); e; e = e->next) {
                    covered[e->to] = true;
                }
            } else {
               // Parent already picked? Then u should have been covered?
               // Ah, covered array check at start handles it.
            }
        } else {
            // Root has no parent. Must pick root.
            if (!in_set[u]) {
                in_set[u] = true;
                selected_nodes.push_back(u);
                covered[u] = true;
                for(Edge* e = tree.get_edges(u); e; e = e->next) {
                    covered[e->to] = true;
                }
            }
        }
    }
    
    return (int)selected_nodes.size();
}

}
