#include "graphlib/tree_decomposition.h"
#include <algorithm>
#include <vector>
#include <set>
#include <numeric>
#include <limits>

namespace graphlib {

TreeDecompositionResult tree_decomposition_min_degree(const Graph& g) {
    int n = g.vertex_count();
    
    // Adjacency list using sets for efficient updates
    std::vector<std::set<int>> adj(n);
    for (int u = 0; u < n; ++u) {
        Edge* e = g.get_edges(u);
        while (e) {
            if (e->to != u) { // No self-loops
                adj[u].insert(e->to);
            }
            e = e->next;
        }
    }

    std::vector<bool> removed(n, false);
    std::vector<int> elimination_order;
    elimination_order.reserve(n);
    
    // For reconstructing the tree, we need to know the neighbors in the FILLED graph
    // that appear LATER in the elimination order.
    // We can simulate the process and record the fill-in.
    
    // Since we need "neighbors in filled graph", and we are adding edges,
    // the 'adj' will effectively become the filled graph adjacency (restricted to remaining nodes).
    
    // Store bags: bag[v] = {v} U {neighbors of v in current graph at time of elimination}
    std::vector<std::vector<int>> node_bags(n);
    
    for (int step = 0; step < n; ++step) {
        // Find node with min degree among non-removed
        int best_node = -1;
        size_t min_deg = std::numeric_limits<size_t>::max();
        
        for (int i = 0; i < n; ++i) {
            if (!removed[i]) {
                if (adj[i].size() < min_deg) {
                    min_deg = adj[i].size();
                    best_node = i;
                }
            }
        }
        
        int u = best_node;
        removed[u] = true;
        elimination_order.push_back(u);
        
        // Form the bag for u
        node_bags[u].push_back(u);
        for (int v : adj[u]) {
            node_bags[u].push_back(v);
        }
        
        // Add fill-in edges: make neighbors of u a clique
        // We only need to add edges between neighbors that are not removed
        // (adj already only contains non-removed neighbors because we remove u from them below? 
        // No, adj[u] contains neighbors. We need to update neighbors' adj lists.)
        
        std::vector<int> neighbors(adj[u].begin(), adj[u].end());
        
        for (size_t i = 0; i < neighbors.size(); ++i) {
            int v1 = neighbors[i];
            // Remove u from v1's adjacency
            adj[v1].erase(u);
            
            for (size_t j = i + 1; j < neighbors.size(); ++j) {
                int v2 = neighbors[j];
                // Add edge (v1, v2) if not exists
                if (adj[v1].find(v2) == adj[v1].end()) {
                    adj[v1].insert(v2);
                    adj[v2].insert(v1);
                }
            }
        }
    }
    
    // Build the tree decomposition
    // The nodes of the decomposition tree are the vertices of the original graph (mapped 1-to-1)
    // The bag for node u is node_bags[u].
    // The parent of u is the neighbor of u in node_bags[u] that appears earliest in elimination_order AFTER u.
    
    // Map vertex -> position in elimination order
    std::vector<int> pos(n);
    for (int i = 0; i < n; ++i) {
        pos[elimination_order[i]] = i;
    }
    
    Graph tree(n, false); // undirected tree
    int width = 0;
    
    for (int i = 0; i < n; ++i) {
        int u = elimination_order[i];
        
        // Update width
        if ((int)node_bags[u].size() - 1 > width) {
            width = (int)node_bags[u].size() - 1;
        }
        
        // Find parent
        int parent = -1;
        int min_pos = n + 1;
        
        for (int v : node_bags[u]) {
            if (v == u) continue;
            // v must be after u in elimination order
            if (pos[v] > pos[u]) {
                if (pos[v] < min_pos) {
                    min_pos = pos[v];
                    parent = v;
                }
            }
        }
        
        if (parent != -1) {
            tree.add_edge(u, parent);
        }
    }
    
    // The bags might be redundant (subset of other bags). 
    // Standard definition usually requires removing redundant bags or just keeping it as is.
    // The current construction gives a valid tree decomposition where each original vertex is associated with a bag.
    // The size of the tree is n nodes.
    
    TreeDecompositionResult result;
    result.tree = std::move(tree);
    result.bags = std::move(node_bags);
    result.width = width;
    
    return result;
}

}
