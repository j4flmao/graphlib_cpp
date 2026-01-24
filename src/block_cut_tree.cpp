#include "graphlib/block_cut_tree.h"
#include <algorithm>
#include <stack>
#include <map>
#include <iostream>

namespace graphlib {

namespace {
    struct TarjanState {
        int timer;
        std::vector<int> tin;
        std::vector<int> low;
        std::vector<int> parent; // Optional, can pass in DFS
        std::vector<bool> visited;
        std::stack<std::pair<int, int>> edge_stack; // For BCC
        std::vector<std::vector<int>> bccs; // Store BCCs as list of vertices
        std::vector<int> articulation_points;
        std::vector<std::pair<int, int>> bridges;
        
        TarjanState(int n) : timer(0), tin(n, -1), low(n, -1), visited(n, false) {}
    };

    void dfs_articulation(const Graph& g, int u, int p, TarjanState& state) {
        state.visited[u] = true;
        state.tin[u] = state.low[u] = state.timer++;
        int children = 0;

        Edge* e = g.get_edges(u);
        while (e) {
            int v = e->to;
            if (v == p) {
                e = e->next;
                continue;
            }

            if (state.visited[v]) {
                state.low[u] = std::min(state.low[u], state.tin[v]);
            } else {
                state.edge_stack.push({u, v});
                children++;
                dfs_articulation(g, v, u, state);
                
                state.low[u] = std::min(state.low[u], state.low[v]);
                
                if (state.low[v] >= state.tin[u] && p != -1) {
                    state.articulation_points.push_back(u);
                }
                
                if (state.low[v] >= state.tin[u]) {
                    // Found a BCC
                    std::vector<int> component;
                    std::set<int> unique_nodes;
                    while (true) {
                        if (state.edge_stack.empty()) break; // Safety check
                        std::pair<int, int> edge = state.edge_stack.top();
                        state.edge_stack.pop();
                        unique_nodes.insert(edge.first);
                        unique_nodes.insert(edge.second);
                        if (edge == std::make_pair(u, v)) break;
                    }
                    component.assign(unique_nodes.begin(), unique_nodes.end());
                    state.bccs.push_back(component);
                }

                if (state.low[v] > state.tin[u]) {
                    state.bridges.push_back({std::min(u, v), std::max(u, v)});
                }
            }
            e = e->next;
        }

        if (p == -1 && children > 1) {
            state.articulation_points.push_back(u);
        }
    }
}

std::vector<int> find_articulation_points(const Graph& g) {
    int n = g.vertex_count();
    TarjanState state(n);
    
    for (int i = 0; i < n; ++i) {
        if (!state.visited[i]) {
            dfs_articulation(g, i, -1, state);
            // Handle any remaining edges in stack for BCC if needed, 
            // but for articulation points, the loop logic covers it.
            // Wait, for isolated vertices or simple components, logic is fine.
            // For root, we check children > 1.
            // But if root has children but no back-edges higher up,
            // the bcc logic might leave edges on stack?
            // Yes, if the whole component is one BCC.
            if (!state.edge_stack.empty()) {
                // This block handles the root component case for BCCs
                // Not strictly needed for articulation points unless we want consistency.
                while (!state.edge_stack.empty()) state.edge_stack.pop();
            }
        }
    }
    
    // Remove duplicates
    std::sort(state.articulation_points.begin(), state.articulation_points.end());
    state.articulation_points.erase(std::unique(state.articulation_points.begin(), state.articulation_points.end()), state.articulation_points.end());
    
    return state.articulation_points;
}

std::vector<std::pair<int, int>> find_bridges(const Graph& g) {
    int n = g.vertex_count();
    TarjanState state(n);
    
    for (int i = 0; i < n; ++i) {
        if (!state.visited[i]) {
            dfs_articulation(g, i, -1, state);
        }
    }
    
    std::sort(state.bridges.begin(), state.bridges.end());
    return state.bridges;
}

std::vector<std::vector<int>> find_biconnected_components(const Graph& g) {
    int n = g.vertex_count();
    TarjanState state(n);
    
    for (int i = 0; i < n; ++i) {
        if (!state.visited[i]) {
            dfs_articulation(g, i, -1, state);
            
            // If stack is not empty (e.g. isolated vertex or single component),
            // pop everything as one BCC.
            if (!state.edge_stack.empty()) {
                std::vector<int> component;
                std::set<int> unique_nodes;
                while (!state.edge_stack.empty()) {
                    std::pair<int, int> edge = state.edge_stack.top();
                    state.edge_stack.pop();
                    unique_nodes.insert(edge.first);
                    unique_nodes.insert(edge.second);
                }
                if (!unique_nodes.empty()) {
                    component.assign(unique_nodes.begin(), unique_nodes.end());
                    state.bccs.push_back(component);
                }
            }
        }
    }
    
    return state.bccs;
}

BlockCutTreeResult build_block_cut_tree(const Graph& g) {
    int n = g.vertex_count();
    std::vector<int> aps = find_articulation_points(g);
    std::vector<std::vector<int>> bccs = find_biconnected_components(g);
    
    BlockCutTreeResult res;
    res.num_articulations = static_cast<int>(aps.size());
    res.num_blocks = static_cast<int>(bccs.size());
    
    int total_nodes = res.num_articulations + res.num_blocks;
    res.tree = Graph(total_nodes, false); // Undirected
    res.node_type.resize(total_nodes);
    res.original_id.resize(total_nodes, -1);
    res.block_nodes = bccs;
    
    // Map AP original ID to Tree ID
    std::vector<int> ap_to_tree_id(n, -1);
    for (int i = 0; i < res.num_articulations; ++i) {
        res.node_type[i] = 1; // AP
        res.original_id[i] = aps[i];
        ap_to_tree_id[aps[i]] = i;
    }
    
    for (int i = 0; i < res.num_blocks; ++i) {
        int block_node_idx = res.num_articulations + i;
        res.node_type[block_node_idx] = 0; // Block
        
        for (int u : bccs[i]) {
            if (ap_to_tree_id[u] != -1) {
                // u is an AP, add edge between AP node and Block node
                res.tree.add_edge(ap_to_tree_id[u], block_node_idx);
            }
        }
    }
    
    return res;
}

} // namespace graphlib
