#include "graphlib/shortest_path.h"
#include "graphlib/graph_core.h"
#include <queue>
#include <stdexcept>
#include <limits>
#include <algorithm>
#include <deque>
#include <set>
#include <map>
#include <functional>

namespace graphlib {

ShortestPath::Edge::Edge(int to, long long weight)
    : to(to), weight(weight), enabled(true), next(nullptr) {
}

ShortestPath::ShortestPath(int n)
    : n_(n), adj_(nullptr) {
    if (n <= 0) {
        throw std::invalid_argument("Number of vertices must be positive");
    }

    adj_ = new Edge*[n];
    for (int i = 0; i < n; i++) {
        adj_[i] = nullptr;
    }
}

ShortestPath::~ShortestPath() {
    clear_graph();
    delete[] adj_;
}

ShortestPath::ShortestPath(ShortestPath&& other) noexcept
    : n_(other.n_), adj_(other.adj_) {
    other.n_ = 0;
    other.adj_ = nullptr;
}

ShortestPath& ShortestPath::operator=(ShortestPath&& other) noexcept {
    if (this != &other) {
        clear_graph();
        delete[] adj_;

        n_ = other.n_;
        adj_ = other.adj_;

        other.n_ = 0;
        other.adj_ = nullptr;
    }
    return *this;
}

void ShortestPath::clear_graph() {
    if (!adj_) {
        return;
    }
    for (int i = 0; i < n_; i++) {
        Edge* e = adj_[i];
        while (e) {
            Edge* next = e->next;
            delete e;
            e = next;
        }
        adj_[i] = nullptr;
    }
}

void ShortestPath::add_edge(int from, int to, long long weight) {
    if (from < 0 || from >= n_ || to < 0 || to >= n_) {
        throw std::out_of_range("Vertex index out of range");
    }

    Edge* e = new Edge(to, weight);
    e->next = adj_[from];
    adj_[from] = e;
}

std::vector<long long> ShortestPath::dijkstra(int source, long long inf) {
    if (source < 0 || source >= n_) {
        throw std::out_of_range("Source vertex index out of range");
    }

    std::vector<long long> dist(n_, inf);
    struct Node {
        int v;
        long long d;
        bool operator>(const Node& other) const {
            return d > other.d;
        }
    };

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;
    dist[source] = 0;
    pq.push(Node{source, 0});

    while (!pq.empty()) {
        Node cur = pq.top();
        pq.pop();

        if (cur.d != dist[cur.v]) {
            continue;
        }

        Edge* e = adj_[cur.v];
        while (e) {
            if (!e->enabled) {
                e = e->next;
                continue;
            }
            int to = e->to;
            long long w = e->weight;
            if (w < 0) { // Dijkstra doesn't support negative weights, but we skip check for perf
                // Actually, let's allow it but it might be wrong.
            }
            if (dist[cur.v] + w < dist[to]) {
                dist[to] = dist[cur.v] + w;
                pq.push(Node{to, dist[to]});
            }
            e = e->next;
        }
    }

    return dist;
}

std::vector<long long> ShortestPath::zero_one_bfs(int source, long long inf) {
    if (source < 0 || source >= n_) {
        throw std::out_of_range("Source vertex index out of range");
    }

    std::vector<long long> dist(n_, inf);
    std::deque<int> dq;

    dist[source] = 0;
    dq.push_front(source);

    while (!dq.empty()) {
        int u = dq.front();
        dq.pop_front();

        Edge* e = adj_[u];
        while (e) {
            if (!e->enabled) {
                e = e->next;
                continue;
            }
            int v = e->to;
            long long w = e->weight;
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                if (w == 0) {
                    dq.push_front(v);
                } else {
                    dq.push_back(v);
                }
            }
            e = e->next;
        }
    }
    return dist;
}

std::vector<long long> ShortestPath::bellman_ford(int source, long long inf, bool& has_negative_cycle) {
    has_negative_cycle = false;
    if (source < 0 || source >= n_) {
        return std::vector<long long>(n_, inf);
    }

    std::vector<long long> dist(n_, inf);
    dist[source] = 0;

    for (int i = 0; i < n_ - 1; ++i) {
        bool updated = false;
        for (int u = 0; u < n_; ++u) {
            if (dist[u] == inf) continue;
            Edge* e = adj_[u];
            while (e) {
                if (!e->enabled) {
                    e = e->next;
                    continue;
                }
                int v = e->to;
                long long w = e->weight;
                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    updated = true;
                }
                e = e->next;
            }
        }
        if (!updated) break;
    }

    for (int u = 0; u < n_; ++u) {
        if (dist[u] == inf) continue;
        Edge* e = adj_[u];
        while (e) {
            if (!e->enabled) {
                e = e->next;
                continue;
            }
            int v = e->to;
            long long w = e->weight;
            if (dist[u] + w < dist[v]) {
                has_negative_cycle = true;
                return dist;
            }
            e = e->next;
        }
    }

    return dist;
}

std::vector<std::vector<long long>> ShortestPath::floyd_warshall(long long inf) {
    std::vector<std::vector<long long>> dist(n_, std::vector<long long>(n_, inf));

    for (int i = 0; i < n_; ++i) {
        dist[i][i] = 0;
        Edge* e = adj_[i];
        while (e) {
            if (e->enabled) {
                dist[i][e->to] = std::min(dist[i][e->to], e->weight);
            }
            e = e->next;
        }
    }

    for (int k = 0; k < n_; ++k) {
        for (int i = 0; i < n_; ++i) {
            if (dist[i][k] == inf) continue;
            for (int j = 0; j < n_; ++j) {
                if (dist[k][j] == inf) continue;
                if (dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }

    return dist;
}

std::vector<long long> ShortestPath::a_star(int source, int target, const std::vector<long long>& heuristic, long long inf) {
    if (source < 0 || source >= n_ || target < 0 || target >= n_) {
        return std::vector<long long>(n_, inf);
    }
    if (static_cast<int>(heuristic.size()) != n_) {
        return std::vector<long long>(n_, inf);
    }

    std::vector<long long> dist(n_, inf);
    std::vector<bool> closed(n_, false);

    struct Node {
        int v;
        long long g;
        long long f;
        bool operator>(const Node& other) const {
            return f > other.f;
        }
    };

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

    dist[source] = 0;
    pq.push(Node{source, 0, heuristic[source]});

    while (!pq.empty()) {
        Node cur = pq.top();
        pq.pop();

        int v = cur.v;
        if (closed[v]) {
            continue;
        }
        if (v == target) {
            break;
        }
        closed[v] = true;

        Edge* e = adj_[v];
        while (e) {
            if (!e->enabled) {
                e = e->next;
                continue;
            }
            int to = e->to;
            long long w = e->weight;
            if (w < 0) {
                e = e->next;
                continue;
            }
            long long new_g = cur.g + w;
            if (new_g < dist[to]) {
                dist[to] = new_g;
                long long f = new_g + heuristic[to];
                pq.push(Node{to, new_g, f});
            }
            e = e->next;
        }
    }

    return dist;
}

std::vector<int> ShortestPath::astar_search(int source, int target, std::function<long long(int)> heuristic, long long inf) {
    if (source < 0 || source >= n_ || target < 0 || target >= n_) {
        return {};
    }

    std::vector<long long> g_score(n_, inf);
    std::vector<int> parent(n_, -1);
    std::vector<bool> closed(n_, false);

    struct Node {
        int v;
        long long g;
        long long f;
        bool operator>(const Node& other) const {
            return f > other.f;
        }
    };

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

    g_score[source] = 0;
    pq.push(Node{source, 0, heuristic(source)});

    while (!pq.empty()) {
        Node cur = pq.top();
        pq.pop();

        int v = cur.v;
        if (closed[v]) {
            continue;
        }
        if (v == target) {
            std::vector<int> path;
            for (int u = target; u != -1; u = parent[u]) {
                path.push_back(u);
            }
            std::reverse(path.begin(), path.end());
            return path;
        }
        closed[v] = true;

        Edge* e = adj_[v];
        while (e) {
            if (!e->enabled) {
                e = e->next;
                continue;
            }
            int to = e->to;
            long long w = e->weight;
            if (w < 0) {
                e = e->next;
                continue;
            }
            long long new_g = cur.g + w;
            if (new_g < g_score[to]) {
                g_score[to] = new_g;
                parent[to] = v;
                long long f = new_g + heuristic(to);
                pq.push(Node{to, new_g, f});
            }
            e = e->next;
        }
    }

    return {};
}

std::vector<std::vector<long long>> ShortestPath::johnson(long long inf, bool& has_negative_cycle) {
    has_negative_cycle = false;
    int n = n_;
    std::vector<long long> h(n, 0);

    for (int iter = 0; iter < n - 1; iter++) {
        bool updated = false;
        for (int u = 0; u < n; u++) {
            Edge* e = adj_[u];
            while (e) {
                if (!e->enabled) {
                    e = e->next;
                    continue;
                }
                int v = e->to;
                long long w = e->weight;
                if (h[v] > h[u] + w) {
                    h[v] = h[u] + w;
                    updated = true;
                }
                e = e->next;
            }
        }
        if (!updated) {
            break;
        }
    }

    for (int u = 0; u < n; u++) {
        Edge* e = adj_[u];
        while (e) {
            if (!e->enabled) {
                e = e->next;
                continue;
            }
            int v = e->to;
            long long w = e->weight;
            if (h[v] > h[u] + w) {
                has_negative_cycle = true;
                std::vector<std::vector<long long>> dist_all(n, std::vector<long long>(n, inf));
                return dist_all;
            }
            e = e->next;
        }
    }

    std::vector<std::vector<long long>> dist_all(n, std::vector<long long>(n, inf));

    struct Node {
        int v;
        long long d;
        bool operator>(const Node& other) const {
            return d > other.d;
        }
    };

    for (int s = 0; s < n; s++) {
        std::vector<long long> dist(n, inf);
        std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

        dist[s] = 0;
        pq.push(Node{s, 0});

        while (!pq.empty()) {
            Node cur = pq.top();
            pq.pop();

            if (cur.d != dist[cur.v]) {
                continue;
            }

            Edge* e = adj_[cur.v];
            while (e) {
                int to = e->to;
                long long w = e->weight + h[cur.v] - h[to];
                if (dist[to] > dist[cur.v] + w) {
                    dist[to] = dist[cur.v] + w;
                    pq.push(Node{to, dist[to]});
                }
                e = e->next;
            }
        }

        for (int v = 0; v < n; v++) {
            if (dist[v] != inf) {
                dist_all[s][v] = dist[v] - h[s] + h[v];
            }
        }
    }

    return dist_all;
}

std::vector<long long> ShortestPath::multi_source_dijkstra(const std::vector<int>& sources, long long inf) {
    std::vector<long long> dist(n_, inf);

    struct Node {
        int v;
        long long d;
        bool operator>(const Node& other) const {
            return d > other.d;
        }
    };

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

    int m = static_cast<int>(sources.size());
    for (int i = 0; i < m; i++) {
        int s = sources[i];
        if (s < 0 || s >= n_) {
            continue;
        }
        if (dist[s] == 0) {
            continue;
        }
        dist[s] = 0;
        pq.push(Node{s, 0});
    }

    while (!pq.empty()) {
        Node cur = pq.top();
        pq.pop();

        if (cur.d != dist[cur.v]) {
            continue;
        }

        Edge* e = adj_[cur.v];
        while (e) {
            int to = e->to;
            long long w = e->weight;
            if (w < 0) {
                e = e->next;
                continue;
            }
            long long cand = dist[cur.v] + w;
            if (cand < dist[to]) {
                dist[to] = cand;
                pq.push(Node{to, cand});
            }
            e = e->next;
        }
    }

    return dist;
}

std::vector<std::vector<long long>> ShortestPath::all_pairs_dijkstra(long long inf) {
    std::vector<std::vector<long long>> result(n_);
    for (int s = 0; s < n_; ++s) {
        result[s] = dijkstra(s, inf);
    }
    return result;
}

long double ShortestPath::minimum_mean_cycle(bool& has_cycle) {
    has_cycle = false;
    int n = n_;
    if (n <= 0) {
        return 0.0L;
    }

    const long long INF = std::numeric_limits<long long>::max() / 4;
    std::vector<std::vector<long long>> dp(n + 1, std::vector<long long>(n, INF));

    for (int v = 0; v < n; v++) {
        dp[0][v] = 0;
    }

    for (int k = 1; k <= n; k++) {
        for (int u = 0; u < n; u++) {
            if (dp[k - 1][u] == INF) {
                continue;
            }
            Edge* e = adj_[u];
            while (e) {
                if (!e->enabled) {
                    e = e->next;
                    continue;
                }
                int to = e->to;
                long long w = e->weight;
                long long cand = dp[k - 1][u] + w;
                if (cand < dp[k][to]) {
                    dp[k][to] = cand;
                }
                e = e->next;
            }
        }
    }

    bool found = false;
    long double best_mean = 0.0L;

    for (int v = 0; v < n; v++) {
        if (dp[n][v] == INF) {
            continue;
        }
        long double max_avg = -std::numeric_limits<long double>::infinity();
        bool any = false;
        for (int k = 0; k < n; k++) {
            if (dp[k][v] == INF) {
                continue;
            }
            long double num = static_cast<long double>(dp[n][v] - dp[k][v]);
            long double den = static_cast<long double>(n - k);
            long double val = num / den;
            if (val > max_avg) {
                max_avg = val;
                any = true;
            }
        }
        if (!any) {
            continue;
        }
        if (!found || max_avg < best_mean) {
            best_mean = max_avg;
            found = true;
        }
    }

    if (!found) {
        return 0.0L;
    }

    has_cycle = true;
    return best_mean;
}

std::vector<long long> ShortestPath::dijkstra_with_path(int source, int target, long long inf, std::vector<int>& parent) {
    std::vector<long long> dist(n_, inf);
    parent.assign(n_, -1);
    
    struct Node {
        int v;
        long long d;
        bool operator>(const Node& other) const {
            return d > other.d;
        }
    };

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

    if (source < 0 || source >= n_) return dist;
    
    dist[source] = 0;
    pq.push(Node{source, 0});

    while (!pq.empty()) {
        Node cur = pq.top();
        pq.pop();

        if (cur.d != dist[cur.v]) {
            continue;
        }
        if (cur.v == target) {
            // Optimization: if we only care about target
            // But for Yen's, we need full path? No, we just need path to target.
            // break; 
            // Actually, we can break.
            break;
        }

        Edge* e = adj_[cur.v];
        while (e) {
            if (!e->enabled) {
                e = e->next;
                continue;
            }
            int to = e->to;
            long long w = e->weight;
            if (w < 0) {
                 e = e->next;
                 continue;
            }
            if (dist[cur.v] + w < dist[to]) {
                dist[to] = dist[cur.v] + w;
                parent[to] = cur.v;
                pq.push(Node{to, dist[to]});
            }
            e = e->next;
        }
    }
    return dist;
}

std::vector<int> reconstruct_path(int source, int target, const std::vector<int>& parent) {
    std::vector<int> path;
    if (target < 0 || target >= (int)parent.size() || parent[target] == -1) {
        if (source == target && target >= 0 && target < (int)parent.size()) {
             path.push_back(source);
             return path;
        }
        return path; // Empty
    }
    
    int curr = target;
    while (curr != -1) {
        path.push_back(curr);
        if (curr == source) break;
        curr = parent[curr];
    }
    if (path.back() != source) return {}; // Not reachable
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<std::vector<int>> ShortestPath::k_shortest_paths(int source, int target, int k, long long inf) {
    std::vector<std::vector<int>> A;
    if (source < 0 || source >= n_ || target < 0 || target >= n_ || k <= 0) {
        return A;
    }

    // 1. Determine the shortest path from the source to the sink.
    std::vector<int> parent;
    std::vector<long long> dist = dijkstra_with_path(source, target, inf, parent);
    std::vector<int> path = reconstruct_path(source, target, parent);

    if (path.empty()) {
        return A;
    }

    A.push_back(path);

    // Use a set to track unique vertex paths to handle multigraph duplicates
    std::set<std::vector<int>> A_set;
    A_set.insert(path);

    // Potential paths B (min-heap)
    std::set<std::pair<long long, std::vector<int>>> B;
    
    // To calculate cost of a path (ignoring enabled status, using original weights)
    auto calculate_cost = [&](const std::vector<int>& p) -> long long {
        long long c = 0;
        for (size_t i = 0; i < p.size() - 1; ++i) {
            int u = p[i];
            int v = p[i+1];
            bool found = false;
            Edge* e = adj_[u];
            long long min_w = inf;
            while(e) {
                if (e->to == v) { 
                    // Use minimum weight edge regardless of enabled status
                    min_w = std::min(min_w, e->weight);
                    found = true;
                }
                e = e->next;
            }
            if (found) c += min_w;
            else return inf; 
        }
        return c;
    };

    int processed_idx = 0;
    while (A.size() < (size_t)k) {
        // The path to deviate from (always the last added path in previous iteration logic, 
        // but here we process sequentially)
        const std::vector<int>& prev_path = A[processed_idx];
        
        // Loop over spur nodes
        // Spur node ranges from 0 to size-2
        for (size_t i = 0; i < prev_path.size() - 1; ++i) {
            int spur_node = prev_path[i];
            std::vector<int> root_path(prev_path.begin(), prev_path.begin() + i + 1);
            
            // We need to remove edges that are part of the prefix in existing shortest paths
            // to ensure we branch out.
            std::vector<std::pair<int, int>> disabled_edges;
            
            for (const auto& p_path : A) {
                if (i < p_path.size() - 1) {
                    // Check if root_path matches p_path's prefix
                    bool match = true;
                    if (p_path.size() < root_path.size()) match = false;
                    else {
                        for(size_t j=0; j<root_path.size(); ++j) {
                            if (p_path[j] != root_path[j]) {
                                match = false; 
                                break;
                            }
                        }
                    }
                    
                    if (match) {
                        int u = p_path[i];
                        int v = p_path[i+1];
                        // Disable edge (u, v)
                        Edge* e = adj_[u];
                        while(e) {
                            if (e->to == v && e->enabled) {
                                e->enabled = false;
                                disabled_edges.push_back({u, v});
                                // Only disable ONE edge per matching path
                                break; 
                            }
                            e = e->next;
                        }
                    }
                }
            }
            
            std::vector<int> disabled_nodes; // We only disable outgoing edges from them
            std::vector<std::pair<int, Edge*>> node_disabled_edges;
            
            for (size_t j = 0; j < i; ++j) {
                int u = root_path[j];
                Edge* e = adj_[u];
                while(e) {
                    if (e->enabled) {
                         e->enabled = false;
                         node_disabled_edges.push_back({u, e});
                    }
                    e = e->next;
                }
            }
            
            // Calculate spur path from spur_node to target
            std::vector<int> spur_parent;
            std::vector<long long> spur_dist = dijkstra_with_path(spur_node, target, inf, spur_parent);
            std::vector<int> spur_path = reconstruct_path(spur_node, target, spur_parent);
            
            if (!spur_path.empty()) {
                // Total path = root_path + spur_path (excluding spur_node duplicate)
                std::vector<int> total_path = root_path;
                total_path.insert(total_path.end(), spur_path.begin() + 1, spur_path.end());
                
                long long cost = calculate_cost(total_path);
                if (cost != inf) {
                    B.insert({cost, total_path});
                }
            }
            
            // Restore edges
            // 1. Restore node edges
            for (auto& item : node_disabled_edges) {
                item.second->enabled = true;
            }
            // 2. Restore specific edges
            for (auto& item : disabled_edges) {
                int u = item.first;
                int v = item.second;
                Edge* e = adj_[u];
                while(e) {
                    if (e->to == v && !e->enabled) {
                        e->enabled = true;
                        break; 
                    }
                    e = e->next;
                }
            }
        }
        
        // Done processing this path
        processed_idx++;

        // Pick next best unique path from B
        bool found = false;
        while (!B.empty()) {
            auto it = B.begin();
            std::vector<int> next_path = it->second;
            B.erase(it);
            
            if (A_set.find(next_path) == A_set.end()) {
                A.push_back(next_path);
                A_set.insert(next_path);
                found = true;
                break;
            }
        }
        
        if (!found) break;
    }

    return A;
}

std::vector<int> ShortestPath::find_negative_cycle(long long inf) {
    (void)inf;
    // Bellman-Ford / SPFA style detection.
    std::vector<long long> dist(n_, 0); // Initialize with 0 to detect cycles reachable from "super source" or just anywhere
    std::vector<int> parent(n_, -1);
    
    // We run for n iterations. The last one checks for negative cycle.
    int x = -1;
    for (int i = 0; i < n_; ++i) {
        x = -1;
        for (int u = 0; u < n_; ++u) {
            Edge* e = adj_[u];
            while (e) {
                if (e->enabled) {
                    if (dist[u] + e->weight < dist[e->to]) {
                        dist[e->to] = dist[u] + e->weight;
                        parent[e->to] = u;
                        x = e->to;
                    }
                }
                e = e->next;
            }
        }
    }
    
    if (x == -1) {
        return {}; // No negative cycle
    }
    
    // Trace back n times to find a node in the cycle
    for (int i = 0; i < n_; ++i) {
        x = parent[x];
    }
    
    std::vector<int> cycle;
    int curr = x;
    while (true) {
        cycle.push_back(curr);
        if (curr == x && cycle.size() > 1) {
            break;
        }
        curr = parent[curr];
    }
    std::reverse(cycle.begin(), cycle.end());
    // The cycle repeats the start node at end: u, v, w, u.
    
    return cycle;
}

std::pair<int, std::vector<int>> ShortestPath::bidirectional_bfs(int source, int target) {
    if (source < 0 || source >= n_ || target < 0 || target >= n_) {
        throw std::out_of_range("Vertex index out of range");
    }
    
    if (source == target) {
        return {0, {source}};
    }
    
    std::vector<int> dist_forward(n_, -1);
    std::vector<int> dist_backward(n_, -1);
    std::vector<int> parent_forward(n_, -1);
    std::vector<int> parent_backward(n_, -1);
    
    std::queue<int> q_forward, q_backward;
    
    dist_forward[source] = 0;
    dist_backward[target] = 0;
    q_forward.push(source);
    q_backward.push(target);
    
    int meeting_node = -1;
    int best_dist = -1;
    
    while (!q_forward.empty() || !q_backward.empty()) {
        if (!q_forward.empty()) {
            int u = q_forward.front();
            q_forward.pop();
            
            if (dist_backward[u] != -1) {
                int total = dist_forward[u] + dist_backward[u];
                if (meeting_node == -1 || total < best_dist) {
                    meeting_node = u;
                    best_dist = total;
                }
            }
            
            Edge* e = adj_[u];
            while (e) {
                if (e->enabled && dist_forward[e->to] == -1) {
                    dist_forward[e->to] = dist_forward[u] + 1;
                    parent_forward[e->to] = u;
                    q_forward.push(e->to);
                    
                    if (dist_backward[e->to] != -1) {
                        int total = dist_forward[e->to] + dist_backward[e->to];
                        if (meeting_node == -1 || total < best_dist) {
                            meeting_node = e->to;
                            best_dist = total;
                        }
                    }
                }
                e = e->next;
            }
        }
        
        if (meeting_node != -1 && (q_forward.empty() || dist_forward[q_forward.front()] >= best_dist)) {
            break;
        }
        
        if (!q_backward.empty()) {
            int u = q_backward.front();
            q_backward.pop();
            
            if (dist_forward[u] != -1) {
                int total = dist_forward[u] + dist_backward[u];
                if (meeting_node == -1 || total < best_dist) {
                    meeting_node = u;
                    best_dist = total;
                }
            }
            
            for (int v = 0; v < n_; ++v) {
                Edge* e = adj_[v];
                while (e) {
                    if (e->enabled && e->to == u && dist_backward[v] == -1) {
                        dist_backward[v] = dist_backward[u] + 1;
                        parent_backward[v] = u;
                        q_backward.push(v);
                        
                        if (dist_forward[v] != -1) {
                            int total = dist_forward[v] + dist_backward[v];
                            if (meeting_node == -1 || total < best_dist) {
                                meeting_node = v;
                                best_dist = total;
                            }
                        }
                    }
                    e = e->next;
                }
            }
        }
        
        if (meeting_node != -1 && (q_backward.empty() || dist_backward[q_backward.front()] >= best_dist)) {
            break;
        }
    }
    
    if (meeting_node == -1) {
        return {-1, {}};
    }
    
    std::vector<int> path;
    int curr = meeting_node;
    while (curr != -1) {
        path.push_back(curr);
        curr = parent_forward[curr];
    }
    std::reverse(path.begin(), path.end());
    
    curr = parent_backward[meeting_node];
    while (curr != -1) {
        path.push_back(curr);
        curr = parent_backward[curr];
    }
    
    return {best_dist, path};
}

std::vector<int> topological_sort(const Graph& g) {
    int n = g.vertex_count();
    std::vector<int> in_degree(n, 0);
    
    for (int u = 0; u < n; ++u) {
        Edge* e = g.get_edges(u);
        while (e) {
            in_degree[e->to]++;
            e = e->next;
        }
    }
    
    std::queue<int> q;
    for (int i = 0; i < n; ++i) {
        if (in_degree[i] == 0) {
            q.push(i);
        }
    }
    
    std::vector<int> order;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        order.push_back(u);
        
        Edge* e = g.get_edges(u);
        while (e) {
            int v = e->to;
            in_degree[v]--;
            if (in_degree[v] == 0) {
                q.push(v);
            }
            e = e->next;
        }
    }
    
    if ((int)order.size() != n) {
        return {}; // Cycle detected
    }
    return order;
}

} // namespace graphlib
