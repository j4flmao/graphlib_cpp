#include "graphlib/multigraph.h"
#include "graphlib/graph_core.h"
#include <algorithm>
#include <stack>
#include <unordered_set>
#include <unordered_map>

namespace graphlib {

// ============== Multigraph Implementation ==============

Multigraph::MultiEdge::MultiEdge(int id, int from, int to, long long weight)
    : id(id), from(from), to(to), weight(weight) {}

Multigraph::Multigraph(int n, bool directed) 
    : n_(n), next_edge_id_(0), directed_(directed), adj_(n) {}

int Multigraph::add_edge(int from, int to, long long weight) {
    int id = next_edge_id_++;
    
    MultiEdge edge(id, from, to, weight);
    adj_[from].push_back(edge);
    edges_by_id_[id] = edge;
    
    if (from == to) {
        self_loops_.push_back(id);
    } else if (!directed_) {
        MultiEdge reverse_edge(id, to, from, weight);
        adj_[to].push_back(reverse_edge);
    }
    
    return id;
}

bool Multigraph::remove_edge(int edge_id) {
    auto it = edges_by_id_.find(edge_id);
    if (it == edges_by_id_.end()) return false;
    
    MultiEdge edge = it->second;
    edges_by_id_.erase(it);
    
    // Remove from adjacency list
    auto& list = adj_[edge.from];
    list.erase(std::remove_if(list.begin(), list.end(),
        [edge_id](const MultiEdge& e) { return e.id == edge_id; }), list.end());
    
    if (!directed_ && edge.from != edge.to) {
        auto& list2 = adj_[edge.to];
        list2.erase(std::remove_if(list2.begin(), list2.end(),
            [edge_id](const MultiEdge& e) { return e.id == edge_id; }), list2.end());
    }
    
    // Remove from self-loops if applicable
    self_loops_.erase(std::remove(self_loops_.begin(), self_loops_.end(), edge_id),
                      self_loops_.end());
    
    return true;
}

int Multigraph::remove_all_edges(int u, int v) {
    int count = 0;
    
    std::vector<int> to_remove;
    for (const auto& e : adj_[u]) {
        if (e.to == v) {
            to_remove.push_back(e.id);
        }
    }
    
    for (int id : to_remove) {
        remove_edge(id);
        count++;
    }
    
    return count;
}

const Multigraph::MultiEdge* Multigraph::get_edge(int edge_id) const {
    auto it = edges_by_id_.find(edge_id);
    if (it == edges_by_id_.end()) return nullptr;
    return &(it->second);
}

int Multigraph::edge_multiplicity(int u, int v) const {
    int count = 0;
    for (const auto& e : adj_[u]) {
        if (e.to == v) count++;
    }
    return count;
}

std::vector<Multigraph::MultiEdge> Multigraph::edges_between(int u, int v) const {
    std::vector<MultiEdge> result;
    for (const auto& e : adj_[u]) {
        if (e.to == v) result.push_back(e);
    }
    return result;
}

int Multigraph::degree(int v) const {
    return static_cast<int>(adj_[v].size());
}

int Multigraph::simple_degree(int v) const {
    std::unordered_set<int> neighbors;
    for (const auto& e : adj_[v]) {
        neighbors.insert(e.to);
    }
    return static_cast<int>(neighbors.size());
}

bool Multigraph::has_parallel_edges() const {
    for (int u = 0; u < n_; ++u) {
        std::unordered_map<int, int> neighbor_count;
        for (const auto& e : adj_[u]) {
            if (++neighbor_count[e.to] > 1) return true;
        }
    }
    return false;
}

int Multigraph::remove_self_loops() {
    std::vector<int> loops_to_remove = self_loops_;
    for (int id : loops_to_remove) {
        remove_edge(id);
    }
    return static_cast<int>(loops_to_remove.size());
}

int Multigraph::simplify(const std::string& keep) {
    int removed = 0;
    
    for (int u = 0; u < n_; ++u) {
        std::unordered_map<int, std::vector<int>> neighbor_edges;
        
        for (const auto& e : adj_[u]) {
            neighbor_edges[e.to].push_back(e.id);
        }
        
        for (auto& [v, edge_ids] : neighbor_edges) {
            if (edge_ids.size() <= 1) continue;
            
            // Find the edge to keep
            int keep_id = edge_ids[0];
            long long keep_weight = edges_by_id_[keep_id].weight;
            
            if (keep == "sum") {
                long long sum = 0;
                for (int id : edge_ids) {
                    sum += edges_by_id_[id].weight;
                }
                keep_weight = sum;
            }
            
            for (int id : edge_ids) {
                long long w = edges_by_id_[id].weight;
                if (keep == "max" && w > keep_weight) {
                    keep_id = id;
                    keep_weight = w;
                } else if (keep == "min" && w < keep_weight) {
                    keep_id = id;
                    keep_weight = w;
                }
            }
            
            // Remove all but the keeper
            for (int id : edge_ids) {
                if (id != keep_id) {
                    remove_edge(id);
                    removed++;
                }
            }
            
            // Update keeper weight if sum
            if (keep == "sum") {
                edges_by_id_[keep_id].weight = keep_weight;
                for (auto& e : adj_[u]) {
                    if (e.id == keep_id) e.weight = keep_weight;
                }
                if (!directed_) {
                    for (auto& e : adj_[v]) {
                        if (e.id == keep_id) e.weight = keep_weight;
                    }
                }
            }
        }
    }
    
    // Remove self-loops
    removed += remove_self_loops();
    
    return removed;
}

Graph Multigraph::to_simple_graph() const {
    Graph g(n_, directed_);
    
    for (int u = 0; u < n_; ++u) {
        std::unordered_map<int, long long> max_weights;
        
        for (const auto& e : adj_[u]) {
            if (e.from == e.to) continue;  // Skip self-loops
            if (directed_ || u <= e.to) {
                auto it = max_weights.find(e.to);
                if (it == max_weights.end() || e.weight > it->second) {
                    max_weights[e.to] = e.weight;
                }
            }
        }
        
        for (const auto& [v, w] : max_weights) {
            g.add_edge(u, v, w);
        }
    }
    
    return g;
}

bool Multigraph::is_eulerian() const {
    if (!directed_) {
        for (int v = 0; v < n_; ++v) {
            if (degree(v) % 2 != 0) return false;
        }
    } else {
        std::vector<int> in_degree(n_, 0);
        for (int u = 0; u < n_; ++u) {
            for (const auto& e : adj_[u]) {
                in_degree[e.to]++;
            }
        }
        for (int v = 0; v < n_; ++v) {
            if (degree(v) != in_degree[v]) return false;
        }
    }
    return true;
}

std::vector<int> Multigraph::eulerian_path() const {
    if (!is_eulerian()) return {};
    
    // Find a vertex with edges
    int start = -1;
    for (int v = 0; v < n_; ++v) {
        if (!adj_[v].empty()) {
            start = v;
            break;
        }
    }
    if (start == -1) return {};
    
    // Hierholzer's algorithm
    std::vector<int> path;
    std::stack<int> stack;
    std::unordered_set<int> used_edges;
    
    std::vector<std::vector<MultiEdge>> adj_copy = adj_;
    
    stack.push(start);
    while (!stack.empty()) {
        int u = stack.top();
        
        bool found = false;
        while (!adj_copy[u].empty()) {
            MultiEdge e = adj_copy[u].back();
            adj_copy[u].pop_back();
            
            if (used_edges.count(e.id)) continue;
            
            used_edges.insert(e.id);
            stack.push(e.to);
            found = true;
            break;
        }
        
        if (!found) {
            path.push_back(u);
            stack.pop();
        }
    }
    
    std::reverse(path.begin(), path.end());
    return path;
}

long long Multigraph::total_weight() const {
    long long sum = 0;
    for (const auto& [id, edge] : edges_by_id_) {
        sum += edge.weight;
    }
    return sum;
}

std::vector<Multigraph::MultiEdge> Multigraph::all_edges() const {
    std::vector<MultiEdge> result;
    for (const auto& [id, edge] : edges_by_id_) {
        result.push_back(edge);
    }
    return result;
}



} // namespace graphlib
