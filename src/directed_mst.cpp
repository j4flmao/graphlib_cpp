#include "graphlib/directed_mst.h"
#include <algorithm>
#include <limits>
#include <vector>

namespace graphlib {

// Helper struct for internal processing with backtracking info
struct Edge {
    int u, v;
    long long w;
    int id; // Original edge ID
};

namespace {

// Recursive function to find DMST
// Returns cost, and fills selected_edge_ids with the IDs of edges in the MST
long long dmst_recursive(int n, int root, std::vector<Edge>& edges, std::vector<int>& selected_edge_ids) {
    long long res = 0;
    std::vector<int> min_edge(n, -1);
    
    // 1. Find best incoming edge for each node
    for (int i = 0; i < (int)edges.size(); ++i) {
        int v = edges[i].v;
        if (v == root) continue;
        if (min_edge[v] == -1 || edges[i].w < edges[min_edge[v]].w) {
            min_edge[v] = i;
        }
    }
    
    // Check connectivity
    for (int i = 0; i < n; ++i) {
        if (i != root && min_edge[i] == -1) return -1;
    }
    
    // 2. Detect cycles
    std::vector<int> group(n, -1);
    std::vector<int> visited(n, -1);
    std::vector<int> stack;
    int group_cnt = 0;
    
    for (int i = 0; i < n; ++i) {
        if (i == root) continue;
        int v = i;
        while (visited[v] == -1 && group[v] == -1 && v != root) {
            visited[v] = i;
            v = edges[min_edge[v]].u;
        }
        
        if (v != root && group[v] == -1) {
            // Cycle detected at v
            // Mark all nodes in cycle with new group ID
            int cur = v;
            while (group[cur] == -1) {
                group[cur] = group_cnt;
                cur = edges[min_edge[cur]].u;
            }
            group_cnt++;
        }
    }
    
    // 3. If no cycles, we are done
    if (group_cnt == 0) {
        for (int i = 0; i < n; ++i) {
            if (i != root) {
                int edge_idx = min_edge[i];
                res += edges[edge_idx].w;
                selected_edge_ids.push_back(edges[edge_idx].id);
            }
        }
        return res;
    }
    
    // 4. Contract cycles
    // Give remaining nodes group IDs
    for (int i = 0; i < n; ++i) {
        if (group[i] == -1) {
            group[i] = group_cnt++;
        }
    }
    
    // Calculate cost of edges in cycles (initially added)
    // We'll add these to result now but might remove/swap later
    // Actually standard Chu-Liu adds everything then subtracts.
    // For reconstruction, we pass reduced graph.
    
    // Let's identify cycle edges to potentially add
    // Edges forming the cycles:
    // For each node v in a cycle, min_edge[v] is in the cycle.
    
    std::vector<Edge> next_edges;
    // Map from new edge index to original edge index (for reconstruction)
    // We need to know which original edge corresponds to a selected edge in contracted graph.
    struct EdgeMapping {
        int original_edge_idx;
        int ring_node_to_remove; // If this edge is picked, which node in the cycle does it enter?
    };
    std::vector<EdgeMapping> mapping_info;
    
    // Cycle cost contribution
    // Cycle cost contribution
    // long long cycle_cost = 0;
    for(int i=0; i<n; ++i) {
        if (i != root && edges[min_edge[i]].v == i && group[edges[min_edge[i]].u] == group[i] && min_edge[i] != -1) {
             // This edge is part of a cycle (u and v in same group means cycle because we only grouped cycle nodes)
             // Wait, logic above: group[i] is assigned for ALL nodes. 
             // Cycle nodes have IDs 0..k-1. Others k..N-1.
             // If group size(group[i]) > 1 ? No, we can just check if cycle was detected.
             // But simpler: we know which nodes are in true cycles.
             // Let's just track cycle edges specially.
        }
    }
    
    // Build next level graph
    for (int i = 0; i < (int)edges.size(); ++i) {
        int u = edges[i].u;
        int v = edges[i].v;
        int gu = group[u];
        int gv = group[v];
        
        if (gu != gv) {
            long long new_w = edges[i].w;
            // If v is in a cycle (realy a cycle, not just a generic group), substract weight
            // Check if gv is a contracted cycle group.
            // But 'group' assigns unique IDs to EVERY component.
            // We need to know if 'v' was part of a cycle. 
            // We can check if min_edge[v] connects to something in same group.
            
            int v_in_cycle = 0;
            // Simple check: edge min_edge[v] is u'->v. Is gu' == gv?
            int u_prime = edges[min_edge[v]].u;
            if (group[u_prime] == group[v]) {
                new_w -= edges[min_edge[v]].w;
                v_in_cycle = v;
            } else {
                v_in_cycle = -1; // v is not in a cycle or min_edge[v] enters from outside (impossible if in cycle)
            }
            
            // Wait, if v is in a cycle (contracted), min_edge[v] MUST be the cycle edge.
            // Because min_edge logic picks best incoming. If cycle exists, they are best.
            
            Edge new_edge;
            new_edge.u = gu;
            new_edge.v = gv;
            new_edge.w = new_w;
            new_edge.id = edges[i].id; // Keep original ID for tracking? No, this ID is from input.
            // we need internal tracking.
            next_edges.push_back(new_edge);
            
            EdgeMapping info;
            info.original_edge_idx = i;
            info.ring_node_to_remove = v_in_cycle; // -1 if not removing anything
            mapping_info.push_back(info);
        }
    }
    
    int new_root = group[root];
    std::vector<int> recursive_selected_ids;
    // Recursive call
    // Note: next_edges does NOT have 'id' field carrying index into 'mapping_info', 
    // it carries original id. We need to map back.
    // Let's use 'id' field of next_edges to store index into 'mapping_info'.
    for(size_t k=0; k<next_edges.size(); ++k) {
        next_edges[k].id = (int)k;
    }
    
    long long rec_res = dmst_recursive(group_cnt, new_root, next_edges, recursive_selected_ids);
    if (rec_res == -1) return -1;
    
    res = rec_res;
    
    // Add weights of cycle edges
    for (int i = 0; i < n; ++i) {
        if (i != root) {
            int u_prime = edges[min_edge[i]].u;
            if (group[u_prime] == group[i]) {
                res += edges[min_edge[i]].w;
            }
        }
    }
    
    // Reconstruction
    // 1. Mark all cycle edges as candidates
    std::vector<bool> is_cycle_edge_selected(n, false);
    for (int i = 0; i < n; ++i) {
        if (i != root) {
            int u_prime = edges[min_edge[i]].u;
            if (group[u_prime] == group[i]) {
                is_cycle_edge_selected[i] = true;
            }
        }
    }
    
    // 2. Process selected edges from reduced graph
    for (int map_idx : recursive_selected_ids) {
        const auto& info = mapping_info[map_idx];
        int original_idx = info.original_edge_idx;
        int node_to_remove = info.ring_node_to_remove;
        
        // Add the bridge edge
        selected_edge_ids.push_back(edges[original_idx].id);
        
        // If it enters a circle, remove the internal edge at that entry point
        if (node_to_remove != -1) {
            is_cycle_edge_selected[node_to_remove] = false;
        }
    }
    
    // 3. Add remaining cycle edges
    for (int i = 0; i < n; ++i) {
        if (is_cycle_edge_selected[i]) {
            selected_edge_ids.push_back(edges[min_edge[i]].id);
        }
    }
    
    return res;
}

} // namespace

long long directed_mst(int n, int root, const std::vector<DirectedEdge>& input_edges, std::vector<int>& result_edges) {
    result_edges.clear();
    if (n == 0) return 0;
    
    std::vector<Edge> edges;
    for (const auto& e : input_edges) {
        edges.push_back({e.u, e.v, e.weight, e.id});
    }

    return dmst_recursive(n, root, edges, result_edges);
}

}
