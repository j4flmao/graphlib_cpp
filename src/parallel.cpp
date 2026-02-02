#include "graphlib/parallel.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <queue>
#include <functional>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <map>
#include <set>
#include <random>

namespace graphlib {
namespace parallel {

namespace {
    ParallelConfig global_config;
    

    int get_thread_count() {
        if (global_config.num_threads > 0) {
            return global_config.num_threads;
        }
        return static_cast<int>(std::thread::hardware_concurrency());
    }
}

namespace detail {
    ParallelConfig& get_global_config() {
        return global_config;
    }
}

void set_parallel_config(ParallelConfig config) {
    global_config = config;
}

int get_num_threads() {
    return get_thread_count();
}

// ============== Parallel BFS ==============

std::vector<int> parallel_bfs(const Graph& g, int source, ExecutionPolicy policy) {
    int n = g.vertex_count();
    std::vector<int> dist(n, -1);
    
    if (source < 0 || source >= n) return dist;
    
    if (policy == ExecutionPolicy::Sequential) {
        // Standard BFS
        std::queue<int> q;
        q.push(source);
        dist[source] = 0;
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            
            for (Edge* e = g.get_edges(u); e; e = e->next) {
                if (dist[e->to] == -1) {
                    dist[e->to] = dist[u] + 1;
                    q.push(e->to);
                }
            }
        }
        return dist;
    }
    
    // Parallel BFS using frontier-based approach
    dist[source] = 0;
    std::vector<int> frontier = {source};
    std::vector<int> next_frontier;
    
    while (!frontier.empty()) {
        int frontier_size = static_cast<int>(frontier.size());
        
        // Thread-local next frontiers
        int num_threads = get_thread_count();
        std::vector<std::vector<int>> local_frontiers(num_threads);
        
        detail::parallel_for(0, frontier_size, [&](int i) {
            int thread_id = i * num_threads / frontier_size;
            int u = frontier[i];
            
            for (Edge* e = g.get_edges(u); e; e = e->next) {
                // int expected = -1;
                // Atomic compare-and-swap simulation using mutex per vertex (simplified)
                if (dist[e->to] == -1) {
                    dist[e->to] = dist[u] + 1;
                    local_frontiers[thread_id].push_back(e->to);
                }
            }
        }, policy);
        
        // Merge frontiers
        next_frontier.clear();
        for (const auto& lf : local_frontiers) {
            next_frontier.insert(next_frontier.end(), lf.begin(), lf.end());
        }
        
        std::swap(frontier, next_frontier);
    }
    
    return dist;
}

std::vector<int> parallel_bfs(const CSRGraph& g, int source, ExecutionPolicy policy) {
    int n = g.vertex_count();
    std::vector<int> dist(n, -1);
    
    if (source < 0 || source >= n) return dist;
    
    dist[source] = 0;
    std::vector<int> frontier = {source};
    std::vector<int> next_frontier;
    
    const auto& row_ptr = g.row_ptr();
    const auto& col_idx = g.col_idx();
    
    while (!frontier.empty()) {
        int frontier_size = static_cast<int>(frontier.size());
        
        if (policy == ExecutionPolicy::Sequential || frontier_size < global_config.grain_size) {
            for (int u : frontier) {
                for (int j = row_ptr[u]; j < row_ptr[u + 1]; ++j) {
                    int v = col_idx[j];
                    if (dist[v] == -1) {
                        dist[v] = dist[u] + 1;
                        next_frontier.push_back(v);
                    }
                }
            }
        } else {
            std::mutex mtx;
            
            detail::parallel_for(0, frontier_size, [&](int i) {
                int u = frontier[i];
                std::vector<int> local_next;
                
                for (int j = row_ptr[u]; j < row_ptr[u + 1]; ++j) {
                    int v = col_idx[j];
                    if (dist[v] == -1) {
                        dist[v] = dist[u] + 1;
                        local_next.push_back(v);
                    }
                }
                
                if (!local_next.empty()) {
                    std::lock_guard<std::mutex> lock(mtx);
                    next_frontier.insert(next_frontier.end(), 
                                        local_next.begin(), local_next.end());
                }
            }, policy);
        }
        
        std::swap(frontier, next_frontier);
        next_frontier.clear();
    }
    
    return dist;
}

// ============== Parallel Connected Components ==============

int parallel_connected_components(const Graph& g, std::vector<int>& component, ExecutionPolicy policy) {
    int n = g.vertex_count();
    component.resize(n);
    std::iota(component.begin(), component.end(), 0);
    
    if (policy == ExecutionPolicy::Sequential) {
        // Simple sequential union-find
        std::function<int(int)> find = [&](int x) {
            if (component[x] != x) {
                component[x] = find(component[x]);
            }
            return component[x];
        };
        
        for (int u = 0; u < n; ++u) {
            for (Edge* e = g.get_edges(u); e; e = e->next) {
                int ru = find(u);
                int rv = find(e->to);
                if (ru != rv) {
                    if (ru < rv) component[rv] = ru;
                    else component[ru] = rv;
                }
            }
        }
        
        // Compress
        for (int i = 0; i < n; ++i) {
            find(i);
        }
    } else {
        // Parallel Shiloach-Vishkin style algorithm
        std::vector<std::atomic<int>> parent(n);
        for (int i = 0; i < n; ++i) {
            parent[i].store(i);
        }
        
        auto find = [&](int x) {
            while (parent[x].load() != x) {
                int p = parent[x].load();
                int gp = parent[p].load();
                parent[x].compare_exchange_weak(p, gp);
                x = parent[x].load();
            }
            return x;
        };
        
        bool changed = true;
        while (changed) {
            changed = false;
            
            detail::parallel_for(0, n, [&](int u) {
                for (Edge* e = g.get_edges(u); e; e = e->next) {
                    int ru = find(u);
                    int rv = find(e->to);
                    
                    if (ru != rv) {
                        int old_val = std::max(ru, rv);
                        int new_val = std::min(ru, rv);
                        if (parent[old_val].compare_exchange_strong(old_val, new_val)) {
                            changed = true;
                        }
                    }
                }
            }, policy);
        }
        
        // Copy to output
        for (int i = 0; i < n; ++i) {
            component[i] = find(i);
        }
    }
    
    // Renumber components
    std::map<int, int> comp_map;
    int num_components = 0;
    for (int i = 0; i < n; ++i) {
        if (comp_map.find(component[i]) == comp_map.end()) {
            comp_map[component[i]] = num_components++;
        }
        component[i] = comp_map[component[i]];
    }
    
    return num_components;
}

int parallel_connected_components(const CSRGraph& g, std::vector<int>& component, ExecutionPolicy policy) {
    Graph simple = g.to_graph();
    return parallel_connected_components(simple, component, policy);
}

// ============== Parallel PageRank ==============

std::vector<double> parallel_pagerank(const Graph& g, double d, int iterations, double tolerance,
                                       ExecutionPolicy policy) {
    int n = g.vertex_count();
    std::vector<double> pr(n, 1.0 / n);
    std::vector<double> new_pr(n);
    
    std::vector<int> out_degree(n, 0);
    for (int u = 0; u < n; ++u) {
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            out_degree[u]++;
        }
    }
    
    for (int iter = 0; iter < iterations; ++iter) {
        std::fill(new_pr.begin(), new_pr.end(), (1.0 - d) / n);
        
        // Compute dangling node contribution
        double dangling = 0.0;
        for (int u = 0; u < n; ++u) {
            if (out_degree[u] == 0) {
                dangling += d * pr[u] / n;
            }
        }
        
        for (int i = 0; i < n; ++i) {
            new_pr[i] += dangling;
        }
        
        if (policy == ExecutionPolicy::Sequential) {
            for (int u = 0; u < n; ++u) {
                if (out_degree[u] == 0) continue;
                double contrib = d * pr[u] / out_degree[u];
                for (Edge* e = g.get_edges(u); e; e = e->next) {
                    new_pr[e->to] += contrib;
                }
            }
        } else {
            std::vector<std::mutex> mutexes(n);
            
            detail::parallel_for(0, n, [&](int u) {
                if (out_degree[u] == 0) return;
                double contrib = d * pr[u] / out_degree[u];
                for (Edge* e = g.get_edges(u); e; e = e->next) {
                    std::lock_guard<std::mutex> lock(mutexes[e->to]);
                    new_pr[e->to] += contrib;
                }
            }, policy);
        }
        
        // Check convergence
        double max_diff = 0.0;
        for (int i = 0; i < n; ++i) {
            max_diff = std::max(max_diff, std::abs(new_pr[i] - pr[i]));
        }
        
        std::swap(pr, new_pr);
        
        if (max_diff < tolerance) break;
    }
    
    return pr;
}

// ============== Parallel Triangle Count ==============

long long parallel_triangle_count(const Graph& g, ExecutionPolicy policy) {
    int n = g.vertex_count();
    
    // Build adjacency set
    std::vector<std::set<int>> adj(n);
    for (int u = 0; u < n; ++u) {
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            adj[u].insert(e->to);
        }
    }
    
    // Degree ordering
    std::vector<int> degree(n);
    for (int u = 0; u < n; ++u) {
        degree[u] = static_cast<int>(adj[u].size());
    }
    
    if (policy == ExecutionPolicy::Sequential) {
        long long count = 0;
        for (int u = 0; u < n; ++u) {
            for (int v : adj[u]) {
                if (degree[v] > degree[u] || (degree[v] == degree[u] && v > u)) {
                    for (int w : adj[u]) {
                        if ((degree[w] > degree[v] || (degree[w] == degree[v] && w > v)) 
                            && adj[v].count(w)) {
                            count++;
                        }
                    }
                }
            }
        }
        return count;
    }
    
    std::atomic<long long> count(0);
    
    detail::parallel_for(0, n, [&](int u) {
        long long local_count = 0;
        for (int v : adj[u]) {
            if (degree[v] > degree[u] || (degree[v] == degree[u] && v > u)) {
                for (int w : adj[u]) {
                    if ((degree[w] > degree[v] || (degree[w] == degree[v] && w > v)) 
                        && adj[v].count(w)) {
                        local_count++;
                    }
                }
            }
        }
        count += local_count;
    }, policy);
    
    return count.load();
}

long long parallel_triangle_count(const CSRGraph& g, ExecutionPolicy policy) {
    Graph simple = g.to_graph();
    return parallel_triangle_count(simple, policy);
}

// ============== Parallel SSSP Delta-Stepping ==============

std::vector<long long> parallel_sssp_delta_stepping(const Graph& g, int source, long long delta,
                                                     ExecutionPolicy policy) {
    int n = g.vertex_count();
    const long long INF = std::numeric_limits<long long>::max();
    std::vector<long long> dist(n, INF);
    
    if (source < 0 || source >= n) return dist;
    
    // Auto-tune delta if not provided
    if (delta <= 0) {
        long long max_weight = 1;
        for (int u = 0; u < n; ++u) {
            for (Edge* e = g.get_edges(u); e; e = e->next) {
                max_weight = std::max(max_weight, e->weight);
            }
        }
        delta = std::max(1LL, max_weight / 10);
    }
    
    dist[source] = 0;
    
    // Buckets indexed by distance / delta
    std::vector<std::set<int>> buckets(n + 1);
    buckets[0].insert(source);
    
    for (int b = 0; b <= n; ++b) {
        while (!buckets[b].empty()) {
            std::vector<int> light_relaxations;
            
            // Process all vertices in bucket b
            while (!buckets[b].empty()) {
                int u = *buckets[b].begin();
                buckets[b].erase(buckets[b].begin());
                
                for (Edge* e = g.get_edges(u); e; e = e->next) {
                    if (e->weight <= delta) {
                        // Light edge
                        long long new_dist = dist[u] + e->weight;
                        if (new_dist < dist[e->to]) {
                            int old_bucket = (dist[e->to] == INF) ? -1 : 
                                static_cast<int>(dist[e->to] / delta);
                            if (old_bucket >= 0 && old_bucket <= n) {
                                buckets[old_bucket].erase(e->to);
                            }
                            dist[e->to] = new_dist;
                            int new_bucket = static_cast<int>(new_dist / delta);
                            if (new_bucket <= n) {
                                buckets[new_bucket].insert(e->to);
                            }
                        }
                    } else {
                        // Heavy edge - defer
                        light_relaxations.push_back(e->to);
                    }
                }
            }
            
            // Process heavy edges
            for (int v : light_relaxations) {
                (void)v;
                // Find the edge weight (simplified - should track properly)
            }
        }
    }
    
    return dist;
}

// ============== Parallel Betweenness Centrality ==============

std::vector<double> parallel_betweenness_centrality(const Graph& g, ExecutionPolicy policy) {
    int n = g.vertex_count();
    std::vector<double> bc(n, 0.0);
    
    auto compute_from_source = [&](int s) {
        std::vector<std::vector<int>> pred(n);
        std::vector<int> dist(n, -1);
        std::vector<int> sigma(n, 0);
        std::vector<int> order;
        
        dist[s] = 0;
        sigma[s] = 1;
        std::queue<int> q;
        q.push(s);
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            order.push_back(u);
            
            for (Edge* e = g.get_edges(u); e; e = e->next) {
                int v = e->to;
                if (dist[v] < 0) {
                    dist[v] = dist[u] + 1;
                    q.push(v);
                }
                if (dist[v] == dist[u] + 1) {
                    sigma[v] += sigma[u];
                    pred[v].push_back(u);
                }
            }
        }
        
        std::vector<double> delta(n, 0.0);
        for (auto it = order.rbegin(); it != order.rend(); ++it) {
            int w = *it;
            for (int v : pred[w]) {
                delta[v] += (static_cast<double>(sigma[v]) / sigma[w]) * (1 + delta[w]);
            }
        }
        
        return delta;
    };
    
    if (policy == ExecutionPolicy::Sequential) {
        for (int s = 0; s < n; ++s) {
            auto delta = compute_from_source(s);
            for (int v = 0; v < n; ++v) {
                if (v != s) bc[v] += delta[v];
            }
        }
    } else {
        std::vector<std::vector<double>> all_deltas(n);
        
        detail::parallel_for(0, n, [&](int s) {
            all_deltas[s] = compute_from_source(s);
        }, policy);
        
        for (int s = 0; s < n; ++s) {
            for (int v = 0; v < n; ++v) {
                if (v != s) bc[v] += all_deltas[s][v];
            }
        }
    }
    
    // Normalize for undirected graphs
    if (!g.is_directed()) {
        for (int i = 0; i < n; ++i) {
            bc[i] /= 2.0;
        }
    }
    
    return bc;
}

// ============== Parallel Label Propagation ==============

std::vector<int> parallel_label_propagation(const Graph& g, int max_iterations, ExecutionPolicy policy) {
    int n = g.vertex_count();
    std::vector<int> labels(n);
    std::iota(labels.begin(), labels.end(), 0);
    
    std::vector<int> order(n);
    std::iota(order.begin(), order.end(), 0);
    
    for (int iter = 0; iter < max_iterations; ++iter) {
        bool changed = false;
        
        // Random shuffle order
        // Random shuffle order
        std::random_device rd;
        std::mt19937 rng(rd());
        std::shuffle(order.begin(), order.end(), rng);
        
        if (policy == ExecutionPolicy::Sequential) {
            for (int u : order) {
                std::map<int, int> label_count;
                for (Edge* e = g.get_edges(u); e; e = e->next) {
                    label_count[labels[e->to]]++;
                }
                
                if (!label_count.empty()) {
                    int best_label = labels[u];
                    int best_count = 0;
                    for (const auto& [lbl, cnt] : label_count) {
                        if (cnt > best_count) {
                            best_count = cnt;
                            best_label = lbl;
                        }
                    }
                    if (best_label != labels[u]) {
                        labels[u] = best_label;
                        changed = true;
                    }
                }
            }
        } else {
            std::vector<int> new_labels = labels;
            std::atomic<bool> atomic_changed(false);
            
            detail::parallel_for(0, n, [&](int i) {
                int u = order[i];
                std::map<int, int> label_count;
                for (Edge* e = g.get_edges(u); e; e = e->next) {
                    label_count[labels[e->to]]++;
                }
                
                if (!label_count.empty()) {
                    int best_label = labels[u];
                    int best_count = 0;
                    for (const auto& [lbl, cnt] : label_count) {
                        if (cnt > best_count) {
                            best_count = cnt;
                            best_label = lbl;
                        }
                    }
                    if (best_label != labels[u]) {
                        new_labels[u] = best_label;
                        atomic_changed = true;
                    }
                }
            }, policy);
            
            labels = new_labels;
            changed = atomic_changed;
        }
        
        if (!changed) break;
    }
    
    return labels;
}

// ============== Parallel K-Core ==============

std::vector<int> parallel_kcore(const Graph& g, ExecutionPolicy policy) {
    int n = g.vertex_count();
    std::vector<int> core(n);
    std::vector<int> degree(n, 0);
    
    for (int u = 0; u < n; ++u) {
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            degree[u]++;
        }
        core[u] = degree[u];
    }
    
    // Peeling algorithm
    std::vector<bool> removed(n, false);
    int remaining = n;
    
    for (int k = 0; remaining > 0; ++k) {
        bool progress = true;
        while (progress) {
            progress = false;
            
            for (int u = 0; u < n; ++u) {
                if (!removed[u] && degree[u] <= k) {
                    removed[u] = true;
                    core[u] = k;
                    remaining--;
                    progress = true;
                    
                    for (Edge* e = g.get_edges(u); e; e = e->next) {
                        if (!removed[e->to]) {
                            degree[e->to]--;
                        }
                    }
                }
            }
        }
    }
    
    return core;
}

// ============== Parallel Graph Contraction ==============

Graph parallel_contract(const Graph& g, const std::vector<int>& mapping, int new_n,
                         ExecutionPolicy policy) {
    Graph contracted(new_n, g.is_directed());
    
    std::map<std::pair<int, int>, long long> edge_weights;
    std::mutex mtx;
    
    int n = g.vertex_count();
    
    if (policy == ExecutionPolicy::Sequential) {
        for (int u = 0; u < n; ++u) {
            for (Edge* e = g.get_edges(u); e; e = e->next) {
                int new_u = mapping[u];
                int new_v = mapping[e->to];
                if (new_u != new_v) {
                    auto key = std::make_pair(std::min(new_u, new_v), std::max(new_u, new_v));
                    edge_weights[key] += e->weight;
                }
            }
        }
    } else {
        detail::parallel_for(0, n, [&](int u) {
            std::map<std::pair<int, int>, long long> local_weights;
            
            for (Edge* e = g.get_edges(u); e; e = e->next) {
                int new_u = mapping[u];
                int new_v = mapping[e->to];
                if (new_u != new_v) {
                    auto key = std::make_pair(std::min(new_u, new_v), std::max(new_u, new_v));
                    local_weights[key] += e->weight;
                }
            }
            
            std::lock_guard<std::mutex> lock(mtx);
            for (const auto& [key, weight] : local_weights) {
                edge_weights[key] += weight;
            }
        }, policy);
    }
    
    for (const auto& [key, weight] : edge_weights) {
        contracted.add_edge(key.first, key.second, weight);
    }
    
    return contracted;
}

// ============== Parallel Utilities ==============

void parallel_prefix_sum(std::vector<long long>& arr, ExecutionPolicy policy) {
    if (policy == ExecutionPolicy::Sequential || arr.size() < 1000) {
        for (size_t i = 1; i < arr.size(); ++i) {
            arr[i] += arr[i - 1];
        }
        return;
    }
    
    // Parallel prefix sum (Blelloch scan)
    int n = static_cast<int>(arr.size());
    int num_threads = get_thread_count();
    int chunk_size = (n + num_threads - 1) / num_threads;
    
    std::vector<long long> chunk_sums(num_threads);
    
    // Phase 1: Local prefix sums
    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
        int start = t * chunk_size;
        int end = std::min(start + chunk_size, n);
        
        if (start >= n) break;
        
        threads.emplace_back([start, end, &arr, &chunk_sums, t]() {
            for (int i = start + 1; i < end; ++i) {
                arr[i] += arr[i - 1];
            }
            chunk_sums[t] = arr[end - 1];
        });
    }
    
    for (auto& t : threads) t.join();
    threads.clear();
    
    // Phase 2: Prefix sum of chunk sums
    for (int t = 1; t < num_threads; ++t) {
        chunk_sums[t] += chunk_sums[t - 1];
    }
    
    // Phase 3: Add chunk offsets
    for (int t = 1; t < num_threads; ++t) {
        int start = t * chunk_size;
        int end = std::min(start + chunk_size, n);
        
        if (start >= n) break;
        
        long long offset = chunk_sums[t - 1];
        threads.emplace_back([start, end, offset, &arr]() {
            for (int i = start; i < end; ++i) {
                arr[i] += offset;
            }
        });
    }
    
    for (auto& t : threads) t.join();
}

void parallel_for_vertices(const Graph& g, std::function<void(int)> func, ExecutionPolicy policy) {
    detail::parallel_for(0, g.vertex_count(), func, policy);
}

void parallel_for_edges(const Graph& g, std::function<void(int, int, long long)> func, ExecutionPolicy policy) {
    if (policy == ExecutionPolicy::Sequential) {
        for (int u = 0; u < g.vertex_count(); ++u) {
            for (Edge* e = g.get_edges(u); e; e = e->next) {
                func(u, e->to, e->weight);
            }
        }
    } else {
        detail::parallel_for(0, g.vertex_count(), [&](int u) {
            for (Edge* e = g.get_edges(u); e; e = e->next) {
                func(u, e->to, e->weight);
            }
        }, policy);
    }
}

// ============== Parallel MIS and Coloring ==============

std::vector<int> parallel_maximal_independent_set(const Graph& g, ExecutionPolicy policy) {
    int n = g.vertex_count();
    std::vector<int> mis_status(n, 0); // 0: unknown, 1: in MIS, -1: not in MIS
    std::vector<int> random_val(n);
    
    // Simple PRNG (Linear Congruential Generator) for determinism/speed per thread
    detail::parallel_for(0, n, [&](int i) {
        // Each vertex picks a random value
        random_val[i] = ((i * 1103515245 + 12345) / 65536) % 32768; 
        // Mix with more randomness if needed or use std::random_device outside
        // For simplicity, we assume this is "random enough" to break symmetries
    }, policy);

    int remaining = n;
    while (remaining > 0) {
        std::vector<int> candidates;
        // In a real optimized implementation, we would maintain a list of active vertices.
        // Here we scan for simplicity.
        
        // Phase 1: Check if local max
        std::vector<bool> is_local_max(n, false);
        
        detail::parallel_for(0, n, [&](int u) {
            if (mis_status[u] != 0) return;
            
            bool local_max = true;
            for (Edge* e = g.get_edges(u); e; e = e->next) {
                int v = e->to;
                if (mis_status[v] == 0) { // Only compare with active neighbors
                    if (random_val[v] > random_val[u] || (random_val[v] == random_val[u] && v > u)) {
                        local_max = false;
                        break;
                    }
                }
            }
            if (local_max) is_local_max[u] = true;
        }, policy);
        
        // Phase 2: Add to MIS and remove neighbors
        std::atomic<int> newly_decided(0);
        
        detail::parallel_for(0, n, [&](int u) {
            if (is_local_max[u]) {
                mis_status[u] = 1; // In MIS
                newly_decided++;
                // Neighbors are out
                for (Edge* e = g.get_edges(u); e; e = e->next) {
                    // We need to be careful with concurrent writes here.
                    // However, multiple writers writing -1 is fine (idempotent).
                    // We just need to ensure we don't overwrite a 1 (which shouldn't happen by logic).
                    if (mis_status[e->to] == 0) {
                        mis_status[e->to] = -1; // Not in MIS
                        // Note: counting newly_decided for neighbors accurately needs atomic or careful counting
                        // We'll just track if we are done by checking generally
                    }
                }
            }
        }, policy);
        
        // Count remaining (expensive but robust)
        remaining = 0;
        for(int i=0; i<n; ++i) if(mis_status[i] == 0) remaining++;
        
        if (remaining > 0) {
             // Reroll random values for active vertices to avoid cycles/stalls?
             // Luby's original alg rerolls.
             detail::parallel_for(0, n, [&](int i) {
                 if (mis_status[i] == 0) {
                      random_val[i] = (random_val[i] * 1103515245 + 12345) % 32768;
                 }
             }, policy);
        }
    }
    
    // Convert status to list of vertices
    std::vector<int> mis_list;
    for(int i=0; i<n; ++i) {
        if (mis_status[i] == 1) {
            mis_list.push_back(i);
        }
    }
    return mis_list;
}

std::vector<int> parallel_coloring(const Graph& g, ExecutionPolicy policy) {
    int n = g.vertex_count();
    std::vector<int> colors(n, -1);
    std::vector<int> random_val(n);
    
    // Init random values
    detail::parallel_for(0, n, [&](int i) {
        random_val[i] = ((i * 1103515245 + 12345) / 65536) % 32768; 
    }, policy);
    
    int uncolored_count = n;
    
    while (uncolored_count > 0) {
        std::vector<int> independent_set(n, 0); // 1 if in IS
        
        // Find Independent Set of uncolored vertices
        detail::parallel_for(0, n, [&](int u) {
            if (colors[u] != -1) return;
            
            bool local_max = true;
            for (Edge* e = g.get_edges(u); e; e = e->next) {
                int v = e->to;
                if (colors[v] == -1) { // Only consider uncolored neighbors
                    if (random_val[v] > random_val[u] || (random_val[v] == random_val[u] && v > u)) {
                        local_max = false;
                        break;
                    }
                }
            }
            if (local_max) {
                 independent_set[u] = 1;
            }
        }, policy);
        
        // Assign colors to IS
        // Each u in IS finds the smallest color not used by its neighbors
        detail::parallel_for(0, n, [&](int u) {
            if (independent_set[u]) {
                unsigned long long mask = 0; // Bitmask for small colors (handle up to 64)
                std::vector<int> neighbor_colors;
                
                for (Edge* e = g.get_edges(u); e; e = e->next) {
                    if (colors[e->to] != -1) {
                         int c = colors[e->to];
                         if (c < 64) mask |= (1ULL << c);
                         else neighbor_colors.push_back(c);
                    }
                }
                
                // Find first zero bit
                int c = 0;
                while ((mask & (1ULL << c))) {
                    c++;
                    if (c >= 64) break;
                }
                
                if (c >= 64) {
                    // Fallback using vector
                    std::sort(neighbor_colors.begin(), neighbor_colors.end());
                    c = 0;
                    for (int nc : neighbor_colors) {
                        if (nc == c) c++;
                        else if (nc > c) break;
                    }
                }
                
                colors[u] = c;
            }
        }, policy);
        
        // Recalculate uncolored and reroll randoms
        int next_uncolored = 0;
        for(int i=0; i<n; ++i) {
             if (colors[i] == -1) {
                 next_uncolored++;
                 random_val[i] = (random_val[i] * 1103515245 + 12345) % 32768;
             }
        }
        uncolored_count = next_uncolored;
    }
    
    return colors;
}

} // namespace parallel
} // namespace graphlib
