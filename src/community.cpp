#include "graphlib/community.h"
#include <vector>
#include <map>
#include <algorithm>
#include <random>
#include <numeric>

namespace graphlib {

namespace {
    struct NeighborInfo {
        int target;
        long long weight;
    };

    class LouvainGraph {
    public:
        int n;
        std::vector<std::vector<NeighborInfo>> adj;
        std::vector<long long> node_weight; // sum of edge weights incident to node
        long long total_weight; // 2 * m

        LouvainGraph(int n_) : n(n_), adj(n_), node_weight(n_, 0), total_weight(0) {}

        void add_edge(int u, int v, long long w) {
            adj[u].push_back({v, w});
            adj[v].push_back({u, w}); // Assuming undirected
            node_weight[u] += w;
            node_weight[v] += w;
            total_weight += 2 * w;
        }
    };
}

GRAPHLIB_API std::vector<int> louvain_communities(const Graph& g) {
    int n = g.vertex_count();
    if (n == 0) return {};

    // 1. Initial Graph Construction
    LouvainGraph current_g(n);
    for (int u = 0; u < n; ++u) {
        Edge* e = g.get_edges(u);
        while(e) {
            if (u <= e->to) { // Add edge once (handle self-loops if any)
                current_g.add_edge(u, e->to, e->weight);
            }
            e = e->next;
        }
    }

    std::vector<int> final_community(n);
    std::iota(final_community.begin(), final_community.end(), 0);

    // Mappings from original nodes to current level nodes
    std::vector<int> node_to_comm(n);
    std::iota(node_to_comm.begin(), node_to_comm.end(), 0);

    bool improvement = true;
    while (improvement) {
        improvement = false;
        
        // Phase 1: Local Modularity Optimization
        std::vector<int> local_community(current_g.n); // comm of node i in current_g
        std::iota(local_community.begin(), local_community.end(), 0);
        
        std::vector<long long> comm_tot(current_g.n); // Sum of degrees in comm
        std::vector<long long> comm_in(current_g.n, 0); // Internal weight (optional for delta Q but good for tracking)
        
        for(int i=0; i<current_g.n; ++i) {
            comm_tot[i] = current_g.node_weight[i];
        }

        // Randomize order
        std::vector<int> order(current_g.n);
        std::iota(order.begin(), order.end(), 0);
        std::mt19937 rng(42);
        std::shuffle(order.begin(), order.end(), rng);

        bool local_change = true;
        int pass = 0;
        // Limit passes to avoid infinite loops in some edge cases
        while (local_change && pass++ < 100) {
            local_change = false;
            
            for (int i : order) {
                int old_comm = local_community[i];
                long long ki = current_g.node_weight[i];
                
                // Remove i from old_comm calculations
                comm_tot[old_comm] -= ki;
                
                // Find neighbor communities and weights
                std::map<int, long long> neighbor_comm_weights;
                for (const auto& edge : current_g.adj[i]) {
                    int neighbor = edge.target;
                    if (neighbor == i) continue; // Ignore self-loop for neighbor check (it moves with i)
                    int neighbor_c = local_community[neighbor];
                    neighbor_comm_weights[neighbor_c] += edge.weight;
                }
                
                int best_comm = old_comm;
                double max_gain = 0.0;
                double m2 = (double)current_g.total_weight; 
                // Using 2m for normalization. total_weight is 2m.

                // Calculate gain for old_comm (staying) vs neighbors
                // Gain formula (simplified):
                // Delta Q = (ki_in - ki * Sigma_tot / m)
                // We compare increase from isolating i to putting i in C.
                
                // Weight from i to old_comm
                long long ki_in_old = neighbor_comm_weights[old_comm];
                double current_gain = (double)ki_in_old - (double)ki * (double)comm_tot[old_comm] / m2;
                
                max_gain = current_gain;

                for (auto const& [comm, w_in] : neighbor_comm_weights) {
                    if (comm == old_comm) continue;
                    
                    double gain = (double)w_in - (double)ki * (double)comm_tot[comm] / m2;
                    
                    if (gain > max_gain + 1e-10) { // Stability tolerance
                        max_gain = gain;
                        best_comm = comm;
                    }
                }
                
                comm_tot[best_comm] += ki;
                if (best_comm != old_comm) {
                    local_community[i] = best_comm;
                    local_change = true;
                }
            }
        }
        
        // Re-number communities
        std::map<int, int> renumber;
        int new_n = 0;
        for(int i=0; i<current_g.n; ++i) {
            if (renumber.find(local_community[i]) == renumber.end()) {
                renumber[local_community[i]] = new_n++;
            }
        }
        
        if (new_n < current_g.n) {
            // Update final community mapping
            // node_to_comm: orig_node -> current_g_node
            // We need new map: orig_node -> new_g_node
            // local_community: current_g_node -> comm_id (raw)
            // renumber: comm_id -> new_g_node
            
            for(int i=0; i<n; ++i) {
                final_community[i] = renumber[local_community[node_to_comm[i]]];
                node_to_comm[i] = final_community[i]; // Update for next pass
            }
            
            // Phase 2: Aggregation
            LouvainGraph next_g(new_n);
             
            // Build new weighted graph
             std::map<std::pair<int, int>, long long> edge_weights;
            
            for(int i=0; i<current_g.n; ++i) {
                int c_i = renumber[local_community[i]];
                
                // Internal edges (including self-loops of i)
                // External edges
                for(const auto& edge : current_g.adj[i]) {
                    int j = edge.target;
                    int c_j = renumber[local_community[j]];
                    
                    if (i <= j) { // Process each undirected edge once
                        if (c_i > c_j) std::swap(c_i, c_j);
                        edge_weights[{c_i, c_j}] += edge.weight;
                    }
                }
            }
            
            for(const auto& kv : edge_weights) {
                next_g.add_edge(kv.first.first, kv.first.second, kv.second);
            }
            
            current_g = next_g;
            improvement = true;
        } else {
            improvement = false;
        }
    }

    return final_community;
}

GRAPHLIB_API std::map<std::pair<int, int>, int> k_truss_decomposition(const Graph& g) {
    int n = g.vertex_count();
    std::map<std::pair<int, int>, int> trussness;
    if (n == 0) return trussness;
    
    // 1. Identify all edges and compute initial support (triangle count)
    // Edge structure: u < v
    struct EdgeData {
        int u, v;
        int support;
        bool removed;
    };
    
    std::vector<EdgeData> edges;
    std::map<std::pair<int, int>, int> edge_idx; // Map (u,v) to index in edges
    
    // Adjacency for fast intersection. Using sorted vector.
    std::vector<std::vector<int>> adj(n);
    
    for (int u = 0; u < n; ++u) {
        Edge* e = g.get_edges(u);
        while(e) {
            if (u < e->to) { // Undirected, unique
                adj[u].push_back(e->to);
                adj[e->to].push_back(u);
                
                EdgeData ed;
                ed.u = u;
                ed.v = e->to;
                ed.support = 0;
                ed.removed = false;
                
                edge_idx[{u, e->to}] = (int)edges.size();
                edges.push_back(ed);
            }
            e = e->next;
        }
    }
    
    // Sort adj for intersection
    for(int i=0; i<n; ++i) std::sort(adj[i].begin(), adj[i].end());
    
    // Compute initial support
    for(auto& ed : edges) {
        int u = ed.u;
        int v = ed.v;
        
        // Count common neighbors
        std::vector<int> common;
        std::set_intersection(adj[u].begin(), adj[u].end(), 
                              adj[v].begin(), adj[v].end(),
                              std::back_inserter(common));
        ed.support = (int)common.size();
    }
    
    // Peeling process (similar to K-Core)
    // Sort edges by support (using bucket sort or re-sorting)
    // K-Truss usually processes k=3, 4, ...
    // Or we can use a min-priority queue or just iterative peeling.
    // Iterative peeling O(m^1.5) is standard.
    
    int m = (int)edges.size();
    // int k = 2; // k-truss starts at k=3 (triangle support >= 1)
               // Edges with support=0 belong to 2-truss (just edges).
               // We will peal edges with support <= k-2.
    
    int remaining_edges = m;
    
    while (remaining_edges > 0) {
        bool changed = true;
        while(changed) {
            changed = false;
            // Scan and remove
            // To do this efficiently, we really need the bucket queue structure like core decomposition.
            // Let's implement the O(m^1.5) approach using re-checking.
            // Wait, standard algo uses bin-sort property.
            
            // Let's use a simpler approach: 
            // While there exists edge with support < k-2:
            //   Remove it, decrement support of neighbors.
        }
        
        // Optimized approach with buckets
        // 1. Put edges into bins by support.
        // 2. Iterate from s = 0 up to max_support.
        // 3. For each edge in bin[s]: 
        //      trussness[e] = max(s + 2, k_current) ? No, trussness is s+2.
        //      Remove e. For each triangle involving e (u,v,w):
        //         Decrement support of (u,w) and (v,w).
        //         If support drops, move to lower bin. (If new_sup > s, just move. If <= s, it will be processed later or handled carefully).
        //         Actually since we process in increasing order of s, dropping support means it might fall into current bin or lower (already processed?). 
        //         Actually, if we process s, any neighbor with support > s might drop to s or >s. If it drops to <= s, we need to process it now.
        break; // Switching to implementation below
    }
    
    // Re-implementation with buckets
    int max_sup = 0;
    for(const auto& ed : edges) max_sup = std::max(max_sup, ed.support);
    
    std::vector<std::vector<int>> bins(max_sup + 2);
    // Helper to track position if we want O(1) removal, but lazy removal is easier if we just decrement and check.
    // However, we need to iterate in order.
    // Let's rely on an array `removed` and `current_support`.
    
    std::vector<int> current_support(m);
    for(int i=0; i<m; ++i) current_support[i] = edges[i].support;
    
    // Fill bins
    for(int i=0; i<m; ++i) {
        bins[current_support[i]].push_back(i);
    }
    
    // We need to move items between bins. Fixed bins vector is hard.
    // Use an array `pos` and `vert` like in core_number.
    std::vector<int> sorted_edges(m);
    std::vector<int> pos(m);
    std::vector<int> bin_start(max_sup + 2, 0);
    
    for(int s : current_support) bin_start[s]++;
    
    int start = 0;
    for(int s=0; s<=max_sup; ++s) {
        int count = bin_start[s];
        bin_start[s] = start;
        start += count;
    }
    
    for(int i=0; i<m; ++i) {
        int s = current_support[i];
        pos[i] = bin_start[s];
        sorted_edges[pos[i]] = i;
        bin_start[s]++;
    }
    
    for(int s=max_sup; s>=1; --s) bin_start[s] = bin_start[s-1];
    bin_start[0] = 0;
    
    
    // Process
    for(int i=0; i<m; ++i) {
        int idx = sorted_edges[i]; // edge index
        int sup = current_support[idx];
        
        trussness[{edges[idx].u, edges[idx].v}] = sup + 2;
        
        int u = edges[idx].u;
        int v = edges[idx].v;
        
        // Find triangles (u, v, w)
        // Intersection of adj[u] and adj[v]
        // Since list is sorted, linear scan.
        
        auto it1 = adj[u].begin();
        auto it2 = adj[v].begin();
        
        while(it1 != adj[u].end() && it2 != adj[v].end()) {
            if (*it1 == *it2) {
                int w = *it1;
                // Triangle (u,v,w). Update (u,w) and (v,w)
                // Need their edge indices
                int k1_idx = -1, k2_idx = -1;
                
                int eu = std::min(u, w), ev = std::max(u, w);
                if(edge_idx.count({eu, ev})) k1_idx = edge_idx[{eu, ev}];
                
                eu = std::min(v, w), ev = std::max(v, w);
                if(edge_idx.count({eu, ev})) k2_idx = edge_idx[{eu, ev}];
                
                // Processing k1_idx
                if (k1_idx != -1 && current_support[k1_idx] > sup) {
                    // Reduce support
                    int old_sup = current_support[k1_idx];
                    int p = pos[k1_idx];
                    int p_start = bin_start[old_sup];
                    
                    if (p != p_start) {
                         int swap_idx = sorted_edges[p_start];
                         sorted_edges[p] = swap_idx;
                         sorted_edges[p_start] = k1_idx;
                         pos[swap_idx] = p;
                         pos[k1_idx] = p_start;
                    }
                    
                    bin_start[old_sup]++;
                    current_support[k1_idx]--;
                }
                
                // Processing k2_idx
                if (k2_idx != -1 && current_support[k2_idx] > sup) {
                     int old_sup = current_support[k2_idx];
                     int p = pos[k2_idx];
                     int p_start = bin_start[old_sup];
                     
                     if (p != p_start) {
                          int swap_idx = sorted_edges[p_start];
                          sorted_edges[p] = swap_idx;
                          sorted_edges[p_start] = k2_idx;
                          pos[swap_idx] = p;
                          pos[k2_idx] = p_start;
                     }
                     
                     bin_start[old_sup]++;
                     current_support[k2_idx]--;
                }
                
                ++it1; ++it2;
            } else if (*it1 < *it2) {
                ++it1;
            } else {
                ++it2;
            }
        }
    }
    
    return trussness;
}

} // namespace graphlib
