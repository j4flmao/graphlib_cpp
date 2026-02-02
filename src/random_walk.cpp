#include "graphlib/random_walk.h"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_set>
#include <queue>

namespace graphlib {

namespace {
    std::mt19937& get_rng(int seed) {
        static thread_local std::mt19937 rng;
        if (seed >= 0) {
            rng.seed(static_cast<unsigned int>(seed));
        } else {
            std::random_device rd;
            rng.seed(rd());
        }
        return rng;
    }
    
    std::vector<std::pair<int, long long>> get_neighbors(const Graph& g, int v) {
        std::vector<std::pair<int, long long>> neighbors;
        for (Edge* e = g.get_edges(v); e; e = e->next) {
            neighbors.emplace_back(e->to, e->weight);
        }
        return neighbors;
    }
    
    int random_neighbor(const Graph& g, int v, std::mt19937& rng) {
        auto neighbors = get_neighbors(g, v);
        if (neighbors.empty()) return -1;
        
        std::uniform_int_distribution<int> dist(0, static_cast<int>(neighbors.size()) - 1);
        return neighbors[dist(rng)].first;
    }
    
    int weighted_random_neighbor(const Graph& g, int v, std::mt19937& rng) {
        auto neighbors = get_neighbors(g, v);
        if (neighbors.empty()) return -1;
        
        long long total_weight = 0;
        for (const auto& [n, w] : neighbors) {
            total_weight += w;
        }
        
        std::uniform_int_distribution<long long> dist(0, total_weight - 1);
        long long r = dist(rng);
        
        long long cumulative = 0;
        for (const auto& [n, w] : neighbors) {
            cumulative += w;
            if (r < cumulative) return n;
        }
        
        return neighbors.back().first;
    }
}

std::vector<std::vector<int>> random_walks(const Graph& g, RandomWalkConfig config) {
    int n = g.vertex_count();
    std::vector<int> all_nodes(n);
    std::iota(all_nodes.begin(), all_nodes.end(), 0);
    return random_walks_from(g, all_nodes, config);
}

std::vector<std::vector<int>> random_walks_from(const Graph& g, 
                                                  const std::vector<int>& start_nodes,
                                                  RandomWalkConfig config) {
    std::vector<std::vector<int>> walks;
    walks.reserve(start_nodes.size() * config.num_walks);
    
    auto& rng = get_rng(config.seed);
    std::uniform_real_distribution<double> restart_dist(0.0, 1.0);
    
    for (int start : start_nodes) {
        for (int w = 0; w < config.num_walks; ++w) {
            std::vector<int> walk;
            walk.reserve(config.walk_length);
            walk.push_back(start);
            
            int curr = start;
            for (int step = 1; step < config.walk_length; ++step) {
                // Random restart
                if (config.restart_prob > 0 && restart_dist(rng) < config.restart_prob) {
                    curr = start;
                    walk.push_back(curr);
                    continue;
                }
                
                int next = random_neighbor(g, curr, rng);
                if (next == -1) break;  // Dead end
                
                walk.push_back(next);
                curr = next;
            }
            
            walks.push_back(std::move(walk));
        }
    }
    
    return walks;
}

std::vector<std::vector<int>> node2vec_walks(const Graph& g, Node2VecConfig config) {
    int n = g.vertex_count();
    std::vector<std::vector<int>> walks;
    walks.reserve(n * config.num_walks);
    
    auto& rng = get_rng(config.seed);
    
    // Precompute neighbors
    std::vector<std::vector<int>> neighbors(n);
    std::vector<std::unordered_set<int>> neighbor_sets(n);
    
    for (int u = 0; u < n; ++u) {
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            neighbors[u].push_back(e->to);
            neighbor_sets[u].insert(e->to);
        }
    }
    
    for (int start = 0; start < n; ++start) {
        for (int w = 0; w < config.num_walks; ++w) {
            std::vector<int> walk;
            walk.reserve(config.walk_length);
            walk.push_back(start);
            
            if (neighbors[start].empty()) {
                walks.push_back(walk);
                continue;
            }
            
            // First step is uniform random
            std::uniform_int_distribution<int> first_dist(0, static_cast<int>(neighbors[start].size()) - 1);
            int curr = neighbors[start][first_dist(rng)];
            walk.push_back(curr);
            int prev = start;
            
            for (int step = 2; step < config.walk_length; ++step) {
                if (neighbors[curr].empty()) break;
                
                // Compute transition probabilities based on p and q
                std::vector<double> probs;
                probs.reserve(neighbors[curr].size());
                
                for (int next : neighbors[curr]) {
                    double weight;
                    if (next == prev) {
                        // Return to previous node
                        weight = 1.0 / config.p;
                    } else if (neighbor_sets[prev].count(next)) {
                        // Distance 1 from prev (common neighbor)
                        weight = 1.0;
                    } else {
                        // Distance 2 from prev
                        weight = 1.0 / config.q;
                    }
                    probs.push_back(weight);
                }
                
                // Normalize and sample
                double total = std::accumulate(probs.begin(), probs.end(), 0.0);
                std::uniform_real_distribution<double> dist(0.0, total);
                double r = dist(rng);
                
                double cumulative = 0;
                int next = neighbors[curr][0];
                for (size_t i = 0; i < probs.size(); ++i) {
                    cumulative += probs[i];
                    if (r < cumulative) {
                        next = neighbors[curr][i];
                        break;
                    }
                }
                
                walk.push_back(next);
                prev = curr;
                curr = next;
            }
            
            walks.push_back(std::move(walk));
        }
    }
    
    return walks;
}

double hitting_time(const Graph& g, int source, int target,
                    int num_simulations, int max_steps, int seed) {
    if (source == target) return 0.0;
    
    auto& rng = get_rng(seed);
    
    long long total_steps = 0;
    int successful = 0;
    
    for (int sim = 0; sim < num_simulations; ++sim) {
        int curr = source;
        for (int step = 0; step < max_steps; ++step) {
            int next = random_neighbor(g, curr, rng);
            if (next == -1) break;  // Dead end
            
            curr = next;
            if (curr == target) {
                total_steps += step + 1;
                successful++;
                break;
            }
        }
    }
    
    if (successful == 0) return -1.0;
    return static_cast<double>(total_steps) / successful;
}

double commute_time(const Graph& g, int u, int v,
                    int num_simulations, int seed) {
    double h_uv = hitting_time(g, u, v, num_simulations, 10000, seed);
    double h_vu = hitting_time(g, v, u, num_simulations, 10000, seed);
    
    if (h_uv < 0 || h_vu < 0) return -1.0;
    return h_uv + h_vu;
}

double cover_time(const Graph& g, int start_vertex,
                  int num_simulations, int seed) {
    int n = g.vertex_count();
    if (n == 0) return 0.0;
    
    // Check connectivity
    std::vector<bool> reachable(n, false);
    std::queue<int> q;
    q.push(start_vertex >= 0 ? start_vertex : 0);
    reachable[q.front()] = true;
    int count = 1;
    
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            if (!reachable[e->to]) {
                reachable[e->to] = true;
                count++;
                q.push(e->to);
            }
        }
    }
    
    if (count < n) return -1.0;  // Disconnected
    
    auto& rng = get_rng(seed);
    
    long long total_steps = 0;
    
    for (int sim = 0; sim < num_simulations; ++sim) {
        std::vector<bool> visited(n, false);
        int start = start_vertex >= 0 ? start_vertex : std::uniform_int_distribution<int>(0, n - 1)(rng);
        
        visited[start] = true;
        int visited_count = 1;
        int curr = start;
        int steps = 0;
        
        while (visited_count < n && steps < n * n * 10) {
            int next = random_neighbor(g, curr, rng);
            if (next == -1) break;
            
            steps++;
            curr = next;
            
            if (!visited[curr]) {
                visited[curr] = true;
                visited_count++;
            }
        }
        
        if (visited_count == n) {
            total_steps += steps;
        }
    }
    
    return static_cast<double>(total_steps) / num_simulations;
}

std::vector<double> stationary_distribution(const Graph& g, int iterations) {
    int n = g.vertex_count();
    if (n == 0) return {};
    
    if (!g.is_directed()) {
        // For undirected graphs: pi_v = degree(v) / (2 * m)
        std::vector<double> pi(n);
        long long total_degree = 0;
        
        for (int u = 0; u < n; ++u) {
            int deg = 0;
            for (Edge* e = g.get_edges(u); e; e = e->next) {
                deg++;
            }
            pi[u] = deg;
            total_degree += deg;
        }
        
        if (total_degree == 0) {
            return std::vector<double>(n, 1.0 / n);
        }
        
        for (int u = 0; u < n; ++u) {
            pi[u] /= total_degree;
        }
        
        return pi;
    }
    
    // For directed graphs: use power iteration
    std::vector<double> pi(n, 1.0 / n);
    std::vector<double> new_pi(n);
    
    // Compute out-degrees
    std::vector<int> out_degree(n, 0);
    for (int u = 0; u < n; ++u) {
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            out_degree[u]++;
        }
    }
    
    for (int iter = 0; iter < iterations; ++iter) {
        std::fill(new_pi.begin(), new_pi.end(), 0.0);
        
        // Handle dangling nodes
        double dangling = 0.0;
        for (int u = 0; u < n; ++u) {
            if (out_degree[u] == 0) {
                dangling += pi[u] / n;
            }
        }
        
        for (int u = 0; u < n; ++u) {
            new_pi[u] = dangling;
            
            // Incoming edges
            for (int v = 0; v < n; ++v) {
                for (Edge* e = g.get_edges(v); e; e = e->next) {
                    if (e->to == u && out_degree[v] > 0) {
                        new_pi[u] += pi[v] / out_degree[v];
                    }
                }
            }
        }
        
        std::swap(pi, new_pi);
    }
    
    return pi;
}

std::vector<double> random_walk_with_restart(const Graph& g, int source,
                                               double restart_prob, 
                                               int iterations) {
    int n = g.vertex_count();
    if (n == 0) return {};
    
    std::vector<double> ppr(n, 0.0);
    ppr[source] = 1.0;
    std::vector<double> new_ppr(n);
    
    // Compute out-degrees
    std::vector<int> out_degree(n, 0);
    for (int u = 0; u < n; ++u) {
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            out_degree[u]++;
        }
    }
    
    for (int iter = 0; iter < iterations; ++iter) {
        std::fill(new_ppr.begin(), new_ppr.end(), 0.0);
        
        for (int u = 0; u < n; ++u) {
            if (out_degree[u] == 0) continue;
            
            for (Edge* e = g.get_edges(u); e; e = e->next) {
                new_ppr[e->to] += (1.0 - restart_prob) * ppr[u] / out_degree[u];
            }
        }
        
        // Add restart probability
        new_ppr[source] += restart_prob;
        
        // Handle dangling nodes
        double dangling = 0.0;
        for (int u = 0; u < n; ++u) {
            if (out_degree[u] == 0) {
                dangling += (1.0 - restart_prob) * ppr[u];
            }
        }
        new_ppr[source] += dangling;
        
        std::swap(ppr, new_ppr);
    }
    
    return ppr;
}

std::vector<int> random_walk_sampling(const Graph& g, int sample_size, int seed) {
    int n = g.vertex_count();
    if (n == 0 || sample_size <= 0) return {};
    
    sample_size = std::min(sample_size, n);
    
    auto& rng = get_rng(seed);
    std::unordered_set<int> sampled;
    
    // Start from random vertex
    std::uniform_int_distribution<int> start_dist(0, n - 1);
    int curr = start_dist(rng);
    sampled.insert(curr);
    
    int stuck_count = 0;
    while (static_cast<int>(sampled.size()) < sample_size && stuck_count < 1000) {
        int next = random_neighbor(g, curr, rng);
        if (next == -1) {
            // Dead end, restart
            curr = start_dist(rng);
            stuck_count++;
        } else {
            curr = next;
            sampled.insert(curr);
            stuck_count = 0;
        }
    }
    
    return std::vector<int>(sampled.begin(), sampled.end());
}

} // namespace graphlib
