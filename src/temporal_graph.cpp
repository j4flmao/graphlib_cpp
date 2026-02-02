#include "graphlib/temporal_graph.h"
#include <algorithm>
#include <queue>
#include <limits>
#include <cmath>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <map>

namespace graphlib {

TemporalGraph::TemporalGraph(int n, bool directed) 
    : n_(n), directed_(directed), min_time_(std::numeric_limits<long long>::max()), 
      max_time_(std::numeric_limits<long long>::min()), sorted_(false) {}

void TemporalGraph::add_edge(int from, int to, long long timestamp, long long weight, long long duration) {
    edges_.push_back({from, to, weight, timestamp, duration});
    min_time_ = std::min(min_time_, timestamp);
    max_time_ = std::max(max_time_, timestamp + duration);
    sorted_ = false;
}

void TemporalGraph::add_edges(const std::vector<TemporalEdge>& edges) {
    for (const auto& e : edges) {
        add_edge(e.from, e.to, e.timestamp, e.weight, e.duration);
    }
}

void TemporalGraph::sort_by_time() {
    if (sorted_) return;
    std::sort(edges_.begin(), edges_.end());
    sorted_ = true;
}

Graph TemporalGraph::snapshot_at(long long t) const {
    Graph g(n_, directed_);
    
    for (const auto& e : edges_) {
        if (e.timestamp <= t && t <= e.timestamp + e.duration) {
            g.add_edge(e.from, e.to, e.weight);
        }
    }
    
    return g;
}

Graph TemporalGraph::snapshot_window(long long t1, long long t2) const {
    Graph g(n_, directed_);
    
    for (const auto& e : edges_) {
        // Edge is active if its interval overlaps with [t1, t2]
        if (e.timestamp <= t2 && e.timestamp + e.duration >= t1) {
            g.add_edge(e.from, e.to, e.weight);
        }
    }
    
    return g;
}

std::vector<TemporalGraph::TemporalEdge> TemporalGraph::edges_in_window(long long t1, long long t2) const {
    std::vector<TemporalEdge> result;
    
    for (const auto& e : edges_) {
        if (e.timestamp >= t1 && e.timestamp <= t2) {
            result.push_back(e);
        }
    }
    
    return result;
}

Graph TemporalGraph::aggregate(const std::string& aggregation) const {
    Graph g(n_, directed_);
    
    // Use map to aggregate
    std::map<std::pair<int, int>, std::vector<long long>> edge_weights;
    
    for (const auto& e : edges_) {
        auto key = std::make_pair(e.from, e.to);
        edge_weights[key].push_back(e.weight);
    }
    
    for (const auto& [key, weights] : edge_weights) {
        long long agg_weight = 0;
        
        if (aggregation == "sum") {
            agg_weight = std::accumulate(weights.begin(), weights.end(), 0LL);
        } else if (aggregation == "count") {
            agg_weight = static_cast<long long>(weights.size());
        } else if (aggregation == "mean") {
            agg_weight = std::accumulate(weights.begin(), weights.end(), 0LL) / static_cast<long long>(weights.size());
        } else if (aggregation == "max") {
            agg_weight = *std::max_element(weights.begin(), weights.end());
        } else if (aggregation == "min") {
            agg_weight = *std::min_element(weights.begin(), weights.end());
        }
        
        g.add_edge(key.first, key.second, agg_weight);
    }
    
    return g;
}

std::vector<long long> TemporalGraph::earliest_arrival(int source, long long start_time) const {
    std::vector<long long> arrival(n_, std::numeric_limits<long long>::max());
    arrival[source] = start_time;
    
    // Sort edges by timestamp
    std::vector<TemporalEdge> sorted_edges = edges_;
    std::sort(sorted_edges.begin(), sorted_edges.end());
    
    // Process edges in temporal order
    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& e : sorted_edges) {
            if (arrival[e.from] <= e.timestamp) {
                long long new_arrival = e.timestamp + 1;  // Assume unit travel time
                if (new_arrival < arrival[e.to]) {
                    arrival[e.to] = new_arrival;
                    changed = true;
                }
            }
        }
    }
    
    // Convert max to -1
    for (int i = 0; i < n_; ++i) {
        if (arrival[i] == std::numeric_limits<long long>::max()) {
            arrival[i] = -1;
        }
    }
    
    return arrival;
}

std::vector<long long> TemporalGraph::latest_departure(int target, long long arrive_by) const {
    std::vector<long long> departure(n_, std::numeric_limits<long long>::min());
    departure[target] = arrive_by;
    
    // Sort edges by timestamp descending
    std::vector<TemporalEdge> sorted_edges = edges_;
    std::sort(sorted_edges.begin(), sorted_edges.end(), [](const TemporalEdge& a, const TemporalEdge& b) {
        return a.timestamp > b.timestamp;
    });
    
    // Process edges in reverse temporal order
    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& e : sorted_edges) {
            if (e.timestamp + 1 <= departure[e.to]) {  // Can arrive in time
                long long new_dep = e.timestamp;
                if (new_dep > departure[e.from]) {
                    departure[e.from] = new_dep;
                    changed = true;
                }
            }
        }
    }
    
    // Convert min to -1
    for (int i = 0; i < n_; ++i) {
        if (departure[i] == std::numeric_limits<long long>::min()) {
            departure[i] = -1;
        }
    }
    
    return departure;
}

std::pair<long long, long long> TemporalGraph::fastest_path(int source, int target, long long start_time) const {
    if (source == target) return {0, start_time};
    
    // Priority queue: (arrival_time, departure_time, vertex)
    using State = std::tuple<long long, long long, int>;
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
    
    std::vector<long long> best_arrival(n_, std::numeric_limits<long long>::max());
    best_arrival[source] = start_time;
    
    // Sort edges for efficient lookup
    std::vector<std::vector<TemporalEdge>> adj(n_);
    for (const auto& e : edges_) {
        adj[e.from].push_back(e);
    }
    for (int i = 0; i < n_; ++i) {
        std::sort(adj[i].begin(), adj[i].end());
    }
    
    pq.push({start_time, start_time, source});
    
    while (!pq.empty()) {
        auto [arr, dep, u] = pq.top();
        pq.pop();
        
        if (u == target) {
            return {arr - start_time, arr};
        }
        
        if (arr > best_arrival[u]) continue;
        
        for (const auto& e : adj[u]) {
            if (e.timestamp >= arr) {
                long long new_arr = e.timestamp + 1;
                if (new_arr < best_arrival[e.to]) {
                    best_arrival[e.to] = new_arr;
                    pq.push({new_arr, e.timestamp, e.to});
                }
            }
        }
    }
    
    return {-1, -1};
}

int TemporalGraph::shortest_temporal_path(int source, int target, long long start_time) const {
    if (source == target) return 0;
    
    // BFS with time constraints
    std::vector<std::pair<int, long long>> queue;  // (vertex, arrival_time)
    std::vector<int> dist(n_, -1);
    dist[source] = 0;
    queue.push_back({source, start_time});
    
    std::vector<std::vector<TemporalEdge>> adj(n_);
    for (const auto& e : edges_) {
        adj[e.from].push_back(e);
    }
    
    size_t head = 0;
    while (head < queue.size()) {
        auto [u, arrival] = queue[head++];
        
        for (const auto& e : adj[u]) {
            if (e.timestamp >= arrival && dist[e.to] == -1) {
                dist[e.to] = dist[u] + 1;
                if (e.to == target) return dist[e.to];
                queue.push_back({e.to, e.timestamp + 1});
            }
        }
    }
    
    return -1;
}

std::vector<bool> TemporalGraph::temporal_reachability(int source, long long start_time) const {
    std::vector<bool> reachable(n_, false);
    reachable[source] = true;
    
    auto arrival = earliest_arrival(source, start_time);
    for (int i = 0; i < n_; ++i) {
        reachable[i] = (arrival[i] >= 0);
    }
    
    return reachable;
}

std::vector<double> TemporalGraph::temporal_betweenness() const {
    std::vector<double> bc(n_, 0.0);
    
    // Simplified: use static betweenness on aggregated graph
    Graph agg = aggregate("count");
    
    // Run Brandes on aggregated graph (simplified version)
    for (int s = 0; s < n_; ++s) {
        std::vector<std::vector<int>> pred(n_);
        std::vector<int> dist(n_, -1);
        std::vector<int> sigma(n_, 0);
        std::vector<int> order;
        
        dist[s] = 0;
        sigma[s] = 1;
        std::queue<int> q;
        q.push(s);
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            order.push_back(u);
            
            for (Edge* e = agg.get_edges(u); e; e = e->next) {
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
        
        std::vector<double> delta(n_, 0.0);
        for (auto it = order.rbegin(); it != order.rend(); ++it) {
            int w = *it;
            for (int v : pred[w]) {
                delta[v] += (static_cast<double>(sigma[v]) / sigma[w]) * (1 + delta[w]);
            }
            if (w != s) {
                bc[w] += delta[w];
            }
        }
    }
    
    return bc;
}

std::vector<long long> TemporalGraph::vertex_activity(int v) const {
    std::vector<long long> times;
    
    for (const auto& e : edges_) {
        if (e.from == v || e.to == v) {
            times.push_back(e.timestamp);
        }
    }
    
    std::sort(times.begin(), times.end());
    return times;
}

double TemporalGraph::vertex_burstiness(int v) const {
    auto times = vertex_activity(v);
    if (times.size() < 2) return 0.0;
    
    // Compute inter-event times
    std::vector<double> intervals;
    for (size_t i = 1; i < times.size(); ++i) {
        intervals.push_back(static_cast<double>(times[i] - times[i - 1]));
    }
    
    double mean = std::accumulate(intervals.begin(), intervals.end(), 0.0) / intervals.size();
    
    double sq_sum = 0.0;
    for (double interval : intervals) {
        sq_sum += (interval - mean) * (interval - mean);
    }
    double sigma = std::sqrt(sq_sum / intervals.size());
    
    if (sigma + mean == 0) return 0.0;
    return (sigma - mean) / (sigma + mean);
}

std::vector<Graph> TemporalGraph::sliding_windows(long long window_size) const {
    std::vector<Graph> windows;
    
    for (long long t = min_time_; t <= max_time_; t += window_size) {
        windows.push_back(snapshot_window(t, t + window_size - 1));
    }
    
    return windows;
}

// ============== Free Functions ==============

int temporal_scc(const TemporalGraph& g, std::vector<int>& component) {
    int n = g.vertex_count();
    component.resize(n, -1);
    
    // Use reachability to determine temporal SCCs
    // Two vertices are temporally strongly connected if there exist temporal paths
    // in both directions
    
    std::vector<std::vector<bool>> can_reach(n, std::vector<bool>(n, false));
    
    for (int s = 0; s < n; ++s) {
        auto reach = g.temporal_reachability(s, g.min_timestamp());
        for (int t = 0; t < n; ++t) {
            can_reach[s][t] = reach[t];
        }
    }
    
    int num_components = 0;
    for (int i = 0; i < n; ++i) {
        if (component[i] != -1) continue;
        
        component[i] = num_components;
        for (int j = i + 1; j < n; ++j) {
            if (component[j] == -1 && can_reach[i][j] && can_reach[j][i]) {
                component[j] = num_components;
            }
        }
        num_components++;
    }
    
    return num_components;
}

std::vector<std::vector<TemporalGraph::TemporalEdge>> temporal_motifs(
    const TemporalGraph& g, long long delta) {
    
    std::vector<std::vector<TemporalGraph::TemporalEdge>> motifs;
    
    const auto& edges = g.edges();
    int m = static_cast<int>(edges.size());
    
    // Find 2-edge temporal motifs (edge sequences within delta time)
    for (int i = 0; i < m; ++i) {
        for (int j = i + 1; j < m; ++j) {
            if (std::abs(edges[j].timestamp - edges[i].timestamp) <= delta) {
                // Check if edges share a vertex (forming a path or star)
                if (edges[i].to == edges[j].from || 
                    edges[i].from == edges[j].to ||
                    edges[i].from == edges[j].from ||
                    edges[i].to == edges[j].to) {
                    motifs.push_back({edges[i], edges[j]});
                }
            }
        }
    }
    
    return motifs;
}

} // namespace graphlib
