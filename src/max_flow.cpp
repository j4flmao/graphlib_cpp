#include "graphlib/max_flow.h"
#include <queue>
#include <algorithm>
#include <limits>
#include <vector>
#include <cstring>

namespace graphlib {

// ==========================================
// MaxFlow Implementation (Dinic)
// ==========================================

MaxFlow::Edge::Edge(int to, long long cap, long long cost)
    : to(to), cap(cap), cost(cost), rev(nullptr), next(nullptr) {}

MaxFlow::MaxFlow(int n) : n_(n) {
    graph_ = new Edge*[n];
    for(int i=0; i<n; ++i) graph_[i] = nullptr;
    level_ = new int[n];
    iter_ = new Edge*[n];
}

MaxFlow::~MaxFlow() {
    clear_graph();
    delete[] graph_;
    delete[] level_;
    delete[] iter_;
}

MaxFlow::MaxFlow(MaxFlow&& other) noexcept 
    : n_(other.n_), graph_(other.graph_), level_(other.level_), iter_(other.iter_) {
    other.graph_ = nullptr;
    other.level_ = nullptr;
    other.iter_ = nullptr;
    other.n_ = 0;
}

MaxFlow& MaxFlow::operator=(MaxFlow&& other) noexcept {
    if (this != &other) {
        clear_graph();
        delete[] graph_;
        delete[] level_;
        delete[] iter_;
        
        n_ = other.n_;
        graph_ = other.graph_;
        level_ = other.level_;
        iter_ = other.iter_;
        
        other.graph_ = nullptr;
        other.level_ = nullptr;
        other.iter_ = nullptr;
        other.n_ = 0;
    }
    return *this;
}

void MaxFlow::clear_graph() {
    if (graph_) {
        for (int i = 0; i < n_; ++i) {
            Edge* e = graph_[i];
            while (e) {
                Edge* next = e->next;
                delete e;
                e = next;
            }
            graph_[i] = nullptr;
        }
    }
}

void MaxFlow::add_edge(int from, int to, long long capacity) {
    add_edge(from, to, capacity, 0);
}

void MaxFlow::add_edge(int from, int to, long long capacity, long long cost) {
    Edge* fwd = new Edge(to, capacity, cost);
    Edge* bwd = new Edge(from, 0, -cost);
    fwd->rev = bwd;
    bwd->rev = fwd;
    
    fwd->next = graph_[from];
    graph_[from] = fwd;
    
    bwd->next = graph_[to];
    graph_[to] = bwd;
}

void MaxFlow::add_undirected_edge(int u, int v, long long capacity) {
    add_undirected_edge(u, v, capacity, 0);
}

void MaxFlow::add_undirected_edge(int u, int v, long long capacity, long long cost) {
    Edge* fwd = new Edge(v, capacity, cost);
    Edge* bwd = new Edge(u, capacity, cost); // Undirected means capacity both ways? Usually.
    // But for undirected max flow, we usually add two directed edges.
    // If we model as one undirected edge, it's equivalent to u->v cap C and v->u cap C.
    // But flow in one direction consumes capacity.
    // Standard implementation:
    fwd->rev = bwd;
    bwd->rev = fwd;
    
    fwd->next = graph_[u];
    graph_[u] = fwd;
    
    bwd->next = graph_[v];
    graph_[v] = bwd;
}

bool MaxFlow::bfs(int source, int sink) {
    std::fill(level_, level_ + n_, -1);
    level_[source] = 0;
    std::queue<int> q;
    q.push(source);
    
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        
        for (Edge* e = graph_[v]; e != nullptr; e = e->next) {
            if (e->cap > 0 && level_[e->to] < 0) {
                level_[e->to] = level_[v] + 1;
                q.push(e->to);
            }
        }
    }
    return level_[sink] >= 0;
}

long long MaxFlow::edmonds_karp(int source, int sink) {
    long long flow = 0;
    std::vector<int> parent(n_);
    std::vector<Edge*> pred_edge(n_);

    while (true) {
        std::fill(parent.begin(), parent.end(), -1);
        std::queue<int> q;
        q.push(source);
        parent[source] = source; // Mark source as visited

        while (!q.empty()) {
            int u = q.front();
            q.pop();
            if (u == sink) break;

            for (Edge* e = graph_[u]; e != nullptr; e = e->next) {
                if (e->cap > 0 && parent[e->to] == -1) {
                    parent[e->to] = u;
                    pred_edge[e->to] = e;
                    q.push(e->to);
                }
            }
        }

        if (parent[sink] == -1) break;

        long long path_flow = std::numeric_limits<long long>::max();
        for (int v = sink; v != source; v = parent[v]) {
            path_flow = std::min(path_flow, pred_edge[v]->cap);
        }

        for (int v = sink; v != source; v = parent[v]) {
            pred_edge[v]->cap -= path_flow;
            pred_edge[v]->rev->cap += path_flow;
        }

        flow += path_flow;
    }
    return flow;
}

long long MaxFlow::push_relabel(int source, int sink) {
    std::vector<long long> excess(n_, 0);
    std::vector<int> height(n_, 0);
    std::vector<int> count(2 * n_ + 1, 0); // Optimization: gap heuristic could be used, but keeping simple for now
    std::queue<int> active_nodes;
    std::vector<bool> in_queue(n_, false);

    height[source] = n_;
    count[0] = n_ - 1;
    count[n_] = 1;

    for (Edge* e = graph_[source]; e != nullptr; e = e->next) {
        if (e->cap > 0) {
            long long pushed = e->cap;
            e->cap -= pushed;
            e->rev->cap += pushed;
            excess[source] -= pushed;
            excess[e->to] += pushed;
            if (e->to != source && e->to != sink && !in_queue[e->to]) {
                active_nodes.push(e->to);
                in_queue[e->to] = true;
            }
        }
    }

    while (!active_nodes.empty()) {
        int u = active_nodes.front();
        active_nodes.pop();
        in_queue[u] = false;

        for (Edge* e = graph_[u]; e != nullptr; e = e->next) {
            if (excess[u] == 0) break;
            if (e->cap > 0 && height[u] == height[e->to] + 1) {
                long long pushed = std::min(excess[u], e->cap);
                e->cap -= pushed;
                e->rev->cap += pushed;
                excess[u] -= pushed;
                excess[e->to] += pushed;
                if (e->to != source && e->to != sink && !in_queue[e->to]) {
                    active_nodes.push(e->to);
                    in_queue[e->to] = true;
                }
            }
        }

        if (excess[u] > 0) {
            int min_height = 2 * n_;
            for (Edge* e = graph_[u]; e != nullptr; e = e->next) {
                if (e->cap > 0) {
                    min_height = std::min(min_height, height[e->to]);
                }
            }
            if (min_height != 2 * n_) {
                height[u] = min_height + 1;
                active_nodes.push(u);
                in_queue[u] = true;
            }
        }
    }

    return excess[sink]; // Flow value is the excess at sink
}

long long MaxFlow::dfs(int v, int sink, long long f) {
    if (v == sink) return f;
    
    for (Edge*& e = iter_[v]; e != nullptr; e = e->next) {
        if (e->cap > 0 && level_[v] < level_[e->to]) {
            long long d = dfs(e->to, sink, std::min(f, e->cap));
            if (d > 0) {
                e->cap -= d;
                e->rev->cap += d;
                return d;
            }
        }
    }
    return 0;
}

long long MaxFlow::dinic(int source, int sink) {
    long long flow = 0;
    while (bfs(source, sink)) {
        for (int i = 0; i < n_; ++i) iter_[i] = graph_[i];
        while (long long f = dfs(source, sink, std::numeric_limits<long long>::max())) {
            flow += f;
        }
    }
    return flow;
}

void MaxFlow::min_cut_reachable_from_source(int source, std::vector<char>& reachable) const {
    reachable.assign(n_, 0);
    std::queue<int> q;
    q.push(source);
    reachable[source] = 1;
    
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        
        for (Edge* e = graph_[u]; e != nullptr; e = e->next) {
            if (e->cap > 0 && !reachable[e->to]) {
                reachable[e->to] = 1;
                q.push(e->to);
            }
        }
    }
}

// ==========================================
// Min Cost Max Flow (SPFA)
// ==========================================

std::pair<long long, long long> MaxFlow::min_cost_max_flow(int source, int sink) {
    long long total_flow = 0;
    long long total_cost = 0;
    
    while (true) {
        std::vector<long long> dist(n_, std::numeric_limits<long long>::max());
        std::vector<int> p_node(n_, -1);
        std::vector<Edge*> p_edge(n_, nullptr);
        std::vector<bool> in_queue(n_, false);
        std::queue<int> q;
        
        dist[source] = 0;
        q.push(source);
        in_queue[source] = true;
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            in_queue[u] = false;
            
            for (Edge* e = graph_[u]; e != nullptr; e = e->next) {
                if (e->cap > 0 && dist[e->to] > dist[u] + e->cost) {
                    dist[e->to] = dist[u] + e->cost;
                    p_node[e->to] = u;
                    p_edge[e->to] = e;
                    if (!in_queue[e->to]) {
                        q.push(e->to);
                        in_queue[e->to] = true;
                    }
                }
            }
        }
        
        if (dist[sink] == std::numeric_limits<long long>::max()) break;
        
        long long f = std::numeric_limits<long long>::max();
        int curr = sink;
        while (curr != source) {
            f = std::min(f, p_edge[curr]->cap);
            curr = p_node[curr];
        }
        
        total_flow += f;
        total_cost += f * dist[sink];
        
        curr = sink;
        while (curr != source) {
            p_edge[curr]->cap -= f;
            p_edge[curr]->rev->cap += f;
            curr = p_node[curr];
        }
    }
    
    return {total_flow, total_cost};
}

// ==========================================
// Maximum Weight Closure
// ==========================================

GRAPHLIB_API long long maximum_weight_closure(const std::vector<long long>& weights, const std::vector<std::pair<int, int>>& dependencies) {
    int n = static_cast<int>(weights.size());
    if (n == 0) return 0;
    
    int S = n;
    int T = n + 1;
    MaxFlow mf(n + 2);
    
    long long positive_weight_sum = 0;
    
    for (int i = 0; i < n; ++i) {
        if (weights[i] > 0) {
            positive_weight_sum += weights[i];
            mf.add_edge(S, i, weights[i]);
        } else if (weights[i] < 0) {
            mf.add_edge(i, T, -weights[i]);
        }
    }
    
    for (const auto& dep : dependencies) {
        // u depends on v: if we pick u, we must pick v.
        // Closure property: u in C => v in C.
        // Min-Cut formulation:
        // S -> u (capacity w_u if w_u > 0)
        // u -> T (capacity -w_u if w_u < 0)
        // u -> v (capacity INF)
        // Max Weight = Sum(positive) - Min Cut.
        mf.add_edge(dep.first, dep.second, std::numeric_limits<long long>::max());
    }
    
    long long min_cut = mf.dinic(S, T);
    return positive_weight_sum - min_cut;
}

// ==========================================
// Maximum Density Subgraph
// ==========================================

GRAPHLIB_API std::pair<double, std::vector<int>> maximum_density_subgraph(const Graph& g) {
    int n = g.vertex_count();
    if (n == 0) return {0.0, {}};

    // Collect all edges
    struct E { int u, v; };
    std::vector<E> edges;
    for (int i = 0; i < n; ++i) {
        Edge* e = g.get_edges(i);
        while (e) {
            if (e->to > i) { // Undirected, take each edge once
                edges.push_back({i, e->to});
            }
            e = e->next;
        }
    }

    int m = static_cast<int>(edges.size());
    if (m == 0) return {0.0, {}};

    // Binary search
    double L = 0.0, R = static_cast<double>(m);
    
    // Nodes in flow network:
    // Source S = 0
    // Sink T = 1
    // Edge nodes: 2 .. m+1
    // Vertex nodes: m+2 .. m+n+1
    
    int S = 0;
    int T = 1;
    int num_nodes = 2 + m + n;
    
    const long long SCALE = 100000;

    for (int iter = 0; iter < 50; ++iter) {
        double mid = L + (R - L) / 2.0;
        
        MaxFlow mf(num_nodes);
        
        // S -> Edge nodes
        for (int i = 0; i < m; ++i) {
            mf.add_edge(S, 2 + i, SCALE);
            
            int u_idx = m + 2 + edges[i].u;
            int v_idx = m + 2 + edges[i].v;
            mf.add_edge(2 + i, u_idx, std::numeric_limits<long long>::max());
            mf.add_edge(2 + i, v_idx, std::numeric_limits<long long>::max());
        }
        
        // Vertices -> T
        for (int i = 0; i < n; ++i) {
            long long cap = static_cast<long long>(mid * SCALE);
            mf.add_edge(m + 2 + i, T, cap);
        }
        
        long long flow = mf.dinic(S, T);
        
        if (flow < m * SCALE) {
            L = mid;
        } else {
            R = mid;
        }
    }
    
    double final_g = L; 
    if (final_g > 1e-9) final_g -= 1e-5;
    
    MaxFlow mf(num_nodes);
    for (int i = 0; i < m; ++i) {
        mf.add_edge(S, 2 + i, SCALE);
        int u_idx = m + 2 + edges[i].u;
        int v_idx = m + 2 + edges[i].v;
        mf.add_edge(2 + i, u_idx, std::numeric_limits<long long>::max());
        mf.add_edge(2 + i, v_idx, std::numeric_limits<long long>::max());
    }
    for (int i = 0; i < n; ++i) {
        long long cap = static_cast<long long>(final_g * SCALE);
        mf.add_edge(m + 2 + i, T, cap);
    }
    
    mf.dinic(S, T);
    
    std::vector<char> reachable;
    mf.min_cut_reachable_from_source(S, reachable);
    
    std::vector<int> result_vertices;
    for (int i = 0; i < n; ++i) {
        if (reachable[m + 2 + i]) {
            result_vertices.push_back(i);
        }
    }
    
    return {L, result_vertices};
}

GRAPHLIB_API bool min_cost_circulation(int n, const std::vector<CirculationEdge>& edges, long long& total_cost) {
    // Transform to Min Cost Max Flow
    // Create new source S and sink T
    // For each edge (u, v) with lower bound l, upper u, cost c:
    // 1. Add cost += l * c
    // 2. Add edge u -> v with cap (u - l), cost c
    // 3. Demand/Supply:
    //    balance[v] += l
    //    balance[u] -= l
    
    // Then for each node i:
    // if balance[i] > 0: S -> i with cap balance[i], cost 0
    // if balance[i] < 0: i -> T with cap -balance[i], cost 0
    
    // Check if max flow satisfies all demands.
    
    int S = n;
    int T = n + 1;
    MaxFlow mf(n + 2);
    
    long long initial_cost = 0;
    std::vector<long long> balance(n, 0);
    
    for (const auto& e : edges) {
        if (e.lower > e.upper) return false;
        
        initial_cost += e.lower * e.cost;
        balance[e.to] += e.lower;
        balance[e.from] -= e.lower;
        
        mf.add_edge(e.from, e.to, e.upper - e.lower, e.cost);
    }
    
    long long total_demand = 0;
    for (int i = 0; i < n; ++i) {
        if (balance[i] > 0) {
            mf.add_edge(S, i, balance[i], 0);
            total_demand += balance[i];
        } else if (balance[i] < 0) {
            mf.add_edge(i, T, -balance[i], 0);
        }
    }
    
    auto result = mf.min_cost_max_flow(S, T);
    
    if (result.first != total_demand) return false;
    
    total_cost = initial_cost + result.second;
    return true;
}

} // namespace graphlib
