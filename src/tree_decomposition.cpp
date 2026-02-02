#include "graphlib/tree_decomposition.h"
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <list>

namespace graphlib {

namespace {

struct NodeInfo {
    int id;
    int degree;
    bool eliminated;
    std::set<int> neighbors;
};

}

TreeDecomposition compute_tree_decomposition(const Graph& g) {
    int n = g.vertex_count();
    if (n == 0) return {{}, {}, 0};

    // 1. Elimination Ordering (Min-Degree Heuristic)
    std::vector<NodeInfo> nodes(n);
    for (int i = 0; i < n; ++i) {
        nodes[i].id = i;
        nodes[i].eliminated = false;
        Edge* e = g.get_edges(i);
        while (e) {
            // Treat as undirected
            nodes[i].neighbors.insert(e->to);
            e = e->next;
        }
    }
    for (int i = 0; i < n; ++i) nodes[i].degree = (int)nodes[i].neighbors.size();

    // To efficiently pick min degree, we can use a set of (degree, id) or linear scan for small graphs/dense.
    // For "perfect" implementation, let's use a set for O(log n) updates.
    std::set<std::pair<int, int>> min_degree_queue;
    for (int i = 0; i < n; ++i) {
         min_degree_queue.insert({nodes[i].degree, i});
    }

    std::vector<int> elimination_order;
    elimination_order.reserve(n);
    
    // Track neighbors at time of elimination for bag construction
    std::vector<std::vector<int>> lower_neighbors(n);

    // Copy of graph for fill-in simulation
    // We update neighbors in 'nodes'.
    
    for (int step = 0; step < n; ++step) {
        if (min_degree_queue.empty()) break;
        
        std::pair<int, int> top = *min_degree_queue.begin();
        min_degree_queue.erase(min_degree_queue.begin());
        
        int u = top.second;
        nodes[u].eliminated = true;
        elimination_order.push_back(u);
        
        // Collect current neighbors (that are not eliminated)
        std::vector<int> current_neighbors;
        for (int v : nodes[u].neighbors) {
            if (!nodes[v].eliminated) {
                current_neighbors.push_back(v);
            }
        }
        
        lower_neighbors[u] = current_neighbors;

        // Add fill-in edges: clique among neighbors
        // This is the most expensive part O(degree^2).
        for (size_t i = 0; i < current_neighbors.size(); ++i) {
            int v = current_neighbors[i];
            
            for (size_t j = i + 1; j < current_neighbors.size(); ++j) {
                int w = current_neighbors[j];
                
                // Add edge (v, w) if not exists
                if (nodes[v].neighbors.find(w) == nodes[v].neighbors.end()) {
                    // Update v
                    min_degree_queue.erase({nodes[v].degree, v});
                    nodes[v].neighbors.insert(w);
                    nodes[v].degree++;
                    min_degree_queue.insert({nodes[v].degree, v});
                    
                    // Update w
                    min_degree_queue.erase({nodes[w].degree, w});
                    nodes[w].neighbors.insert(v);
                    nodes[w].degree++;
                    min_degree_queue.insert({nodes[w].degree, w});
                }
            }
        }
    }

    // 2. Construct Tree Decomposition
    // Bags[u] = {u} U lower_neighbors[u]
    // Parent of Bags[u] is Bags[p] where p = first node in lower_neighbors[u] (w.r.t elimination order)
    
    // We need map from node to its rank in elimination order to sort/find "first"
    std::vector<int> rank(n);
    for (int i = 0; i < n; ++i) rank[elimination_order[i]] = i;
    
    int num_bags = n;
    TreeDecomposition td;
    td.bags.resize(num_bags);
    td.adj.resize(num_bags);
    
    int max_bag_size = 0;

    for (int u = 0; u < n; ++u) {
        // Construct bag for node u (which is index u in bags array? No, let's match)
        // Let's use u as bag ID corresponding to node u being eliminated.
        Bag& b = td.bags[u];
        b.id = u;
        b.vertices.push_back(u);
        for (int v : lower_neighbors[u]) {
            b.vertices.push_back(v);
        }
        
        if ((int)b.vertices.size() > max_bag_size) {
            max_bag_size = (int)b.vertices.size();
        }
        
        // Find parent
        if (!lower_neighbors[u].empty()) {
            // The parent is the neighbor with Minimum Rank (appears earliest in remaining list)
            // Wait, rank is position in elimination order.
            // lower_neighbors are nodes that apppear LATER in elimination order (not eliminated yet).
            // So we want the one with MINIMUM Rank among them (the one eliminated soonest after u).
            
            int parent = -1;
            int min_rank = n + 1;
            
            for (int v : lower_neighbors[u]) {
                if (rank[v] < min_rank) {
                    min_rank = rank[v];
                    parent = v;
                }
            }
            
            if (parent != -1) {
                td.adj[u].push_back(parent);
                td.adj[parent].push_back(u);
            }
        }
    }
    
    td.width = max_bag_size - 1;
    
    // Optional: Prune non-maximal bags or smooth the decomposition.
    // However, the requested definition is valid even with redundant bags.
    // The main use is usually Dynamic Programming, which works on this structure.
    
    return td;
}

}
