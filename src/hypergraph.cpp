#include "graphlib/hypergraph.h"
#include <algorithm>
#include <stdexcept>

namespace graphlib {

Hypergraph::Hypergraph(int n) : n_(n), incident_edges_(n) {}

int Hypergraph::add_hyperedge(const std::vector<int>& nodes, double weight) {
    int id = static_cast<int>(edges_.size());
    HyperEdge edge;
    edge.id = id;
    edge.weight = weight;
    edge.nodes = nodes;
    
    // Sort and remove duplicates for consistency? 
    // Usually hypergraphs are sets, so ordered might not matter, but typically we keep them distinct.
    // For now, we trust the input but ensure validity.
    
    for(int v : nodes) {
        if (v < 0 || v >= n_) {
            throw std::out_of_range("Vertex index out of bounds");
        }
        incident_edges_[v].push_back(id);
    }
    
    edges_.push_back(edge);
    return id;
}

int Hypergraph::vertex_count() const {
    return n_;
}

int Hypergraph::edge_count() const {
    return static_cast<int>(edges_.size());
}

int Hypergraph::vertex_degree(int v) const {
    if (v < 0 || v >= n_) throw std::out_of_range("Vertex index out of bounds");
    return static_cast<int>(incident_edges_[v].size());
}

int Hypergraph::edge_size(int e_id) const {
    if (e_id < 0 || e_id >= static_cast<int>(edges_.size())) throw std::out_of_range("Edge ID out of bounds");
    return static_cast<int>(edges_[e_id].nodes.size());
}

const std::vector<int>& Hypergraph::get_incident_edges(int v) const {
    if (v < 0 || v >= n_) throw std::out_of_range("Vertex index out of bounds");
    return incident_edges_[v];
}

const std::vector<int>& Hypergraph::get_edge_nodes(int e_id) const {
    if (e_id < 0 || e_id >= static_cast<int>(edges_.size())) throw std::out_of_range("Edge ID out of bounds");
    return edges_[e_id].nodes;
}

double Hypergraph::get_edge_weight(int e_id) const {
    if (e_id < 0 || e_id >= static_cast<int>(edges_.size())) throw std::out_of_range("Edge ID out of bounds");
    return edges_[e_id].weight;
}

Hypergraph Hypergraph::get_dual() const {
    // In dual:
    // New Vertices = Old Edge IDs (0 to m-1)
    // New Edges = Old Vertices (0 to n-1)
    // New Edge i connects all Old Edges that contained Old Vertex i.
    
    int m = edge_count();
    Hypergraph dual_h(m);
    
    for (int v = 0; v < n_; ++v) {
        const auto& inc = incident_edges_[v];
        if (!inc.empty()) {
            dual_h.add_hyperedge(inc);
        }
    }
    
    return dual_h;
}

int Hypergraph::max_rank() const {
    int mr = 0;
    for (const auto& e : edges_) {
        mr = std::max(mr, static_cast<int>(e.nodes.size()));
    }
    return mr;
}

Graph Hypergraph::to_clique_expansion() const {
    Graph g(n_, false); // Undirected
    
    for (const auto& e : edges_) {
        const auto& nodes = e.nodes;
        for (size_t i = 0; i < nodes.size(); ++i) {
            for (size_t j = i + 1; j < nodes.size(); ++j) {
                g.add_edge(nodes[i], nodes[j], static_cast<long long>(e.weight));
            }
        }
    }
    return g;
}

Graph Hypergraph::to_bipartite_expansion() const {
    int m = edge_count();
    Graph g(n_ + m, false); // Undirected user-edge graph
    
    // Original vertices: 0 to n-1
    // Edge vertices: n to n+m-1
    
    for (int i = 0; i < m; ++i) {
        int e_node = n_ + i;
        for (int v : edges_[i].nodes) {
            g.add_edge(v, e_node, static_cast<long long>(edges_[i].weight));
        }
    }
    return g;
}

std::vector<int> Hypergraph::greedy_transversal() const {
    // Greedy heuristic: repeatedly pick the vertex that covers the most uncovered edges.
    int m = static_cast<int>(edges_.size());
    std::vector<bool> edge_covered(m, false);
    int covered_count = 0;
    std::vector<int> transversal;
    
    // Optimizing this would require a priority queue, but for now O(V * E) is acceptable.
    while (covered_count < m) {
        int best_v = -1;
        int max_new_covered = -1;
        
        for (int v = 0; v < n_; ++v) {
            bool already_in = false; 
            for(int x : transversal) if(x == v) { already_in = true; break; }
            if (already_in) continue;
            
            int current_new = 0;
            for (int e_id : incident_edges_[v]) {
                if (!edge_covered[e_id]) {
                    current_new++;
                }
            }
            
            if (current_new > max_new_covered) {
                max_new_covered = current_new;
                best_v = v;
            }
        }
        
        if (best_v == -1 || max_new_covered == 0) {
            // Should not happen if all edges have at least one vertex and vertices exist
            break; 
        }
        
        transversal.push_back(best_v);
        for (int e_id : incident_edges_[best_v]) {
            if (!edge_covered[e_id]) {
                edge_covered[e_id] = true;
                covered_count++;
            }
        }
    }
    
    return transversal;
}

std::vector<int> Hypergraph::greedy_matching() const {
    // Greedy heuristic: Pick disjoint edges.
    // Heuristic: Prefer edges with smaller degree (size) first? Or simply first available?
    // Let's sort by size (smallest first).
    
    std::vector<int> p(edges_.size());
    for(size_t i=0; i<edges_.size(); ++i) p[i] = static_cast<int>(i);
    
    std::sort(p.begin(), p.end(), [&](int a, int b) {
        return edges_[a].nodes.size() < edges_[b].nodes.size();
    });
    
    std::vector<int> result;
    std::vector<bool> vertex_used(n_, false);
    
    for (int e_id : p) {
        bool disjoint = true;
        for (int v : edges_[e_id].nodes) {
            if (vertex_used[v]) {
                disjoint = false;
                break;
            }
        }
        
        if (disjoint) {
             result.push_back(e_id);
             for (int v : edges_[e_id].nodes) {
                 vertex_used[v] = true;
             }
        }
    }
    
    return result;
}

static bool solve_2_coloring(int v, int n, const std::vector<HyperEdge>& edges, 
                             const std::vector<std::vector<int>>& incident, 
                             std::vector<int>& colors) {
    if (v == n) return true;
    
    // Try color 0
    colors[v] = 0;
    bool ok0 = true;
    for (int e_id : incident[v]) {
        const auto& nodes = edges[e_id].nodes;
        // Check if edge is now fully colored and monochromatic
        bool all_same = true; 
        bool all_colored = true;
        
        for (int u : nodes) {
            if (colors[u] == -1) {
                all_colored = false; 
                break;
            }
            if (colors[u] != 0) {
                all_same = false; 
            }
        }
        
        if (all_colored && all_same) {
            ok0 = false;
            break;
        }
    }
    
    if (ok0) {
        if (solve_2_coloring(v + 1, n, edges, incident, colors)) return true;
    }
    
    // Try color 1
    colors[v] = 1;
    bool ok1 = true;
    for (int e_id : incident[v]) {
        const auto& nodes = edges[e_id].nodes;
        bool all_same = true; 
        bool all_colored = true;
        
        for (int u : nodes) {
            if (colors[u] == -1) {
                all_colored = false; 
                break;
            }
            if (colors[u] != 1) {
                all_same = false; 
            }
        }
        
        if (all_colored && all_same) {
            ok1 = false;
            break;
        }
    }
    
    if (ok1) {
        if (solve_2_coloring(v + 1, n, edges, incident, colors)) return true;
    }
    
    // Backtrack
    colors[v] = -1;
    return false;
}

bool Hypergraph::is_2_colorable(std::vector<int>& colors) const {
    colors.assign(n_, -1);
    // Simple backtracking
    // Not optimized for disconnected components, but valid.
    return solve_2_coloring(0, n_, edges_, incident_edges_, colors);
}

} // namespace graphlib
