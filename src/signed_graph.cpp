#include "graphlib/signed_graph.h"
#include <queue>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_set>

namespace graphlib {

SignedGraph::SignedEdge::SignedEdge(int to, int sign, long long weight)
    : to(to), sign(sign), weight(weight), next(nullptr) {}

SignedGraph::SignedGraph(int n, bool directed) 
    : n_(n), directed_(directed) {
    adj_ = new SignedEdge*[n];
    for (int i = 0; i < n; ++i) {
        adj_[i] = nullptr;
    }
}

SignedGraph::~SignedGraph() {
    clear_adj();
    delete[] adj_;
}

void SignedGraph::clear_adj() {
    for (int i = 0; i < n_; ++i) {
        SignedEdge* e = adj_[i];
        while (e) {
            SignedEdge* next = e->next;
            delete e;
            e = next;
        }
        adj_[i] = nullptr;
    }
}

SignedGraph::SignedGraph(SignedGraph&& other) noexcept
    : n_(other.n_), adj_(other.adj_), directed_(other.directed_) {
    other.n_ = 0;
    other.adj_ = nullptr;
}

SignedGraph& SignedGraph::operator=(SignedGraph&& other) noexcept {
    if (this != &other) {
        clear_adj();
        delete[] adj_;
        
        n_ = other.n_;
        adj_ = other.adj_;
        directed_ = other.directed_;
        
        other.n_ = 0;
        other.adj_ = nullptr;
    }
    return *this;
}

void SignedGraph::add_edge(int from, int to, int sign, long long weight) {
    if (from < 0 || from >= n_ || to < 0 || to >= n_) return;
    
    SignedEdge* e = new SignedEdge(to, sign, weight);
    e->next = adj_[from];
    adj_[from] = e;
    
    if (!directed_ && from != to) {
        SignedEdge* e2 = new SignedEdge(from, sign, weight);
        e2->next = adj_[to];
        adj_[to] = e2;
    }
}

int SignedGraph::get_sign(int u, int v) const {
    if (u < 0 || u >= n_) return 0;
    
    for (SignedEdge* e = adj_[u]; e; e = e->next) {
        if (e->to == v) return e->sign;
    }
    return 0;
}

bool SignedGraph::is_balanced() const {
    // A signed graph is balanced iff it can be 2-colored such that
    // positive edges connect same colors, negative edges connect different colors
    
    std::vector<int> color(n_, -1);
    
    for (int start = 0; start < n_; ++start) {
        if (color[start] != -1) continue;
        
        std::queue<int> q;
        q.push(start);
        color[start] = 0;
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            
            for (SignedEdge* e = adj_[u]; e; e = e->next) {
                int expected_color = (e->sign == 1) ? color[u] : (1 - color[u]);
                
                if (color[e->to] == -1) {
                    color[e->to] = expected_color;
                    q.push(e->to);
                } else if (color[e->to] != expected_color) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

bool SignedGraph::is_weakly_balanced() const {
    // A signed graph is weakly balanced iff there is no triangle with
    // exactly 1 or 2 negative edges (only 0 or 3 negative edges are allowed)
    
    // Build adjacency for fast lookup
    std::vector<std::unordered_map<int, int>> adj_map(n_);
    for (int u = 0; u < n_; ++u) {
        for (SignedEdge* e = adj_[u]; e; e = e->next) {
            adj_map[u][e->to] = e->sign;
        }
    }
    
    // Check all triangles
    for (int u = 0; u < n_; ++u) {
        for (SignedEdge* e1 = adj_[u]; e1; e1 = e1->next) {
            int v = e1->to;
            if (v <= u) continue;
            
            for (SignedEdge* e2 = adj_[v]; e2; e2 = e2->next) {
                int w = e2->to;
                if (w <= v) continue;
                
                auto it = adj_map[u].find(w);
                if (it == adj_map[u].end()) continue;
                
                int neg_count = 0;
                if (e1->sign == -1) neg_count++;
                if (e2->sign == -1) neg_count++;
                if (it->second == -1) neg_count++;
                
                if (neg_count == 1 || neg_count == 2) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

bool SignedGraph::find_balance_partition(std::vector<int>& partition) const {
    partition.resize(n_, -1);
    
    for (int start = 0; start < n_; ++start) {
        if (partition[start] != -1) continue;
        
        std::queue<int> q;
        q.push(start);
        partition[start] = 0;
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            
            for (SignedEdge* e = adj_[u]; e; e = e->next) {
                int expected = (e->sign == 1) ? partition[u] : (1 - partition[u]);
                
                if (partition[e->to] == -1) {
                    partition[e->to] = expected;
                    q.push(e->to);
                } else if (partition[e->to] != expected) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

int SignedGraph::frustration_index() const {
    if (is_balanced()) return 0;
    
    // Use greedy approximation: count edges that would need to be removed
    // to make the graph balanced
    
    std::vector<int> partition;
    
    // Try to find best partition using BFS from each vertex
    int min_frustration = std::numeric_limits<int>::max();
    
    for (int start = 0; start < n_; ++start) {
        std::vector<int> color(n_, -1);
        color[start] = 0;
        
        std::queue<int> q;
        q.push(start);
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            
            for (SignedEdge* e = adj_[u]; e; e = e->next) {
                if (color[e->to] == -1) {
                    color[e->to] = (e->sign == 1) ? color[u] : (1 - color[u]);
                    q.push(e->to);
                }
            }
        }
        
        // Assign unvisited vertices to partition 0
        for (int i = 0; i < n_; ++i) {
            if (color[i] == -1) color[i] = 0;
        }
        
        // Count frustrated edges
        int frustration = 0;
        for (int u = 0; u < n_; ++u) {
            for (SignedEdge* e = adj_[u]; e; e = e->next) {
                if (u > e->to) continue;  // Count each edge once
                
                bool same_partition = (color[u] == color[e->to]);
                if ((e->sign == 1 && !same_partition) || (e->sign == -1 && same_partition)) {
                    frustration++;
                }
            }
        }
        
        min_frustration = std::min(min_frustration, frustration);
    }
    
    return min_frustration;
}

double SignedGraph::signed_clustering_coefficient(int v) const {
    if (v < 0 || v >= n_) return 0.0;
    
    std::vector<std::pair<int, int>> neighbors;  // (neighbor, sign)
    for (SignedEdge* e = adj_[v]; e; e = e->next) {
        neighbors.emplace_back(e->to, e->sign);
    }
    
    int k = static_cast<int>(neighbors.size());
    if (k < 2) return 0.0;
    
    // Build neighbor set for fast lookup
    std::unordered_map<int, int> neighbor_sign;
    for (const auto& [n, s] : neighbors) {
        neighbor_sign[n] = s;
    }
    
    int balanced_triangles = 0;
    int total_triangles = 0;
    
    for (size_t i = 0; i < neighbors.size(); ++i) {
        int u = neighbors[i].first;
        int sign_vu = neighbors[i].second;
        
        for (size_t j = i + 1; j < neighbors.size(); ++j) {
            int w = neighbors[j].first;
            int sign_vw = neighbors[j].second;
            
            // Check if u and w are connected
            int sign_uw = 0;
            for (SignedEdge* e = adj_[u]; e; e = e->next) {
                if (e->to == w) {
                    sign_uw = e->sign;
                    break;
                }
            }
            
            if (sign_uw != 0) {
                total_triangles++;
                
                // Triangle is balanced if product of signs is positive
                int product = sign_vu * sign_vw * sign_uw;
                if (product > 0) {
                    balanced_triangles++;
                }
            }
        }
    }
    
    if (total_triangles == 0) return 0.0;
    return static_cast<double>(balanced_triangles) / total_triangles;
}

std::vector<double> SignedGraph::status_scores() const {
    std::vector<double> status(n_, 0.0);
    
    // Status = sum of signs of incoming edges
    for (int u = 0; u < n_; ++u) {
        for (SignedEdge* e = adj_[u]; e; e = e->next) {
            status[e->to] += e->sign;
        }
    }
    
    return status;
}

std::pair<long long, long long> SignedGraph::count_signed_triangles() const {
    long long positive = 0;
    long long negative = 0;
    
    std::vector<std::unordered_map<int, int>> adj_map(n_);
    for (int u = 0; u < n_; ++u) {
        for (SignedEdge* e = adj_[u]; e; e = e->next) {
            adj_map[u][e->to] = e->sign;
        }
    }
    
    for (int u = 0; u < n_; ++u) {
        for (SignedEdge* e1 = adj_[u]; e1; e1 = e1->next) {
            int v = e1->to;
            if (v <= u) continue;
            
            for (SignedEdge* e2 = adj_[v]; e2; e2 = e2->next) {
                int w = e2->to;
                if (w <= v) continue;
                
                auto it = adj_map[u].find(w);
                if (it == adj_map[u].end()) continue;
                
                int product = e1->sign * e2->sign * it->second;
                if (product > 0) {
                    positive++;
                } else {
                    negative++;
                }
            }
        }
    }
    
    return {positive, negative};
}

std::vector<double> SignedGraph::signed_pagerank(double d, int iterations) const {
    std::vector<double> pr(n_, 1.0 / n_);
    std::vector<double> new_pr(n_);
    
    std::vector<int> out_degree(n_, 0);
    for (int u = 0; u < n_; ++u) {
        for (SignedEdge* e = adj_[u]; e; e = e->next) {
            out_degree[u]++;
        }
    }
    
    for (int iter = 0; iter < iterations; ++iter) {
        std::fill(new_pr.begin(), new_pr.end(), (1.0 - d) / n_);
        
        for (int u = 0; u < n_; ++u) {
            if (out_degree[u] == 0) {
                // Dangling node
                for (int v = 0; v < n_; ++v) {
                    new_pr[v] += d * pr[u] / n_;
                }
            } else {
                for (SignedEdge* e = adj_[u]; e; e = e->next) {
                    double contribution = d * pr[u] * e->sign / out_degree[u];
                    new_pr[e->to] += contribution;
                }
            }
        }
        
        // Normalize to handle negative values
        double sum = 0;
        for (int i = 0; i < n_; ++i) {
            new_pr[i] = std::max(0.0, new_pr[i]);
            sum += new_pr[i];
        }
        if (sum > 0) {
            for (int i = 0; i < n_; ++i) {
                new_pr[i] /= sum;
            }
        }
        
        std::swap(pr, new_pr);
    }
    
    return pr;
}

std::vector<int> SignedGraph::polarized_communities() const {
    std::vector<int> community(n_, -1);
    int num_communities = 0;
    
    // Use balance partition as initial communities
    find_balance_partition(community);
    
    // Reassign isolated vertices
    for (int i = 0; i < n_; ++i) {
        if (community[i] == -1) {
            community[i] = 0;
        }
    }
    
    // Count distinct communities
    std::unordered_set<int> seen(community.begin(), community.end());
    
    return community;
}

Graph SignedGraph::to_unsigned_graph() const {
    Graph g(n_, directed_);
    
    for (int u = 0; u < n_; ++u) {
        for (SignedEdge* e = adj_[u]; e; e = e->next) {
            if (directed_ || u <= e->to) {
                g.add_edge(u, e->to, e->weight);
            }
        }
    }
    
    return g;
}

Graph SignedGraph::positive_subgraph() const {
    Graph g(n_, directed_);
    
    for (int u = 0; u < n_; ++u) {
        for (SignedEdge* e = adj_[u]; e; e = e->next) {
            if (e->sign == 1 && (directed_ || u <= e->to)) {
                g.add_edge(u, e->to, e->weight);
            }
        }
    }
    
    return g;
}

Graph SignedGraph::negative_subgraph() const {
    Graph g(n_, directed_);
    
    for (int u = 0; u < n_; ++u) {
        for (SignedEdge* e = adj_[u]; e; e = e->next) {
            if (e->sign == -1 && (directed_ || u <= e->to)) {
                g.add_edge(u, e->to, e->weight);
            }
        }
    }
    
    return g;
}

int SignedGraph::predict_sign(int u, int v) const {
    if (u < 0 || u >= n_ || v < 0 || v >= n_) return 0;
    
    // Check if edge already exists
    for (SignedEdge* e = adj_[u]; e; e = e->next) {
        if (e->to == v) return e->sign;
    }
    
    // Use balance theory: predict based on common neighbors
    int positive_paths = 0;
    int negative_paths = 0;
    
    std::unordered_map<int, int> u_neighbors;
    for (SignedEdge* e = adj_[u]; e; e = e->next) {
        u_neighbors[e->to] = e->sign;
    }
    
    for (SignedEdge* e = adj_[v]; e; e = e->next) {
        auto it = u_neighbors.find(e->to);
        if (it != u_neighbors.end()) {
            // Common neighbor w: path u-w-v
            int path_sign = it->second * e->sign;
            if (path_sign > 0) {
                positive_paths++;
            } else {
                negative_paths++;
            }
        }
    }
    
    if (positive_paths > negative_paths) return 1;
    if (negative_paths > positive_paths) return -1;
    return 0;
}

void SignedGraph::triangle_analysis(long long& balanced_ppp, long long& balanced_pnn,
                                     long long& unbalanced_ppn, long long& unbalanced_nnn) const {
    balanced_ppp = balanced_pnn = unbalanced_ppn = unbalanced_nnn = 0;
    
    std::vector<std::unordered_map<int, int>> adj_map(n_);
    for (int u = 0; u < n_; ++u) {
        for (SignedEdge* e = adj_[u]; e; e = e->next) {
            adj_map[u][e->to] = e->sign;
        }
    }
    
    for (int u = 0; u < n_; ++u) {
        for (SignedEdge* e1 = adj_[u]; e1; e1 = e1->next) {
            int v = e1->to;
            if (v <= u) continue;
            
            for (SignedEdge* e2 = adj_[v]; e2; e2 = e2->next) {
                int w = e2->to;
                if (w <= v) continue;
                
                auto it = adj_map[u].find(w);
                if (it == adj_map[u].end()) continue;
                
                int neg_count = 0;
                if (e1->sign == -1) neg_count++;
                if (e2->sign == -1) neg_count++;
                if (it->second == -1) neg_count++;
                
                switch (neg_count) {
                    case 0: balanced_ppp++; break;
                    case 1: unbalanced_ppn++; break;
                    case 2: balanced_pnn++; break;
                    case 3: unbalanced_nnn++; break;
                }
            }
        }
    }
}

SignedGraph make_signed_graph(int n,
    const std::vector<std::pair<int, int>>& trust_edges,
    const std::vector<std::pair<int, int>>& distrust_edges) {
    
    SignedGraph g(n, false);
    
    for (const auto& [u, v] : trust_edges) {
        g.add_positive_edge(u, v);
    }
    
    for (const auto& [u, v] : distrust_edges) {
        g.add_negative_edge(u, v);
    }
    
    return g;
}

} // namespace graphlib
