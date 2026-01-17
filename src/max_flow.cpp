#include "graphlib/max_flow.h"
#include <stdexcept>
#include <queue>
#include <vector>
#include <limits>

namespace graphlib {

MaxFlow::Edge::Edge(int to, long long cap, long long cost)
    : to(to), cap(cap), cost(cost), rev(nullptr), next(nullptr) {
}

MaxFlow::MaxFlow(int n)
    : n_(n), graph_(nullptr), level_(nullptr), iter_(nullptr) {
    if (n <= 0) {
        throw std::invalid_argument("Number of vertices must be positive");
    }

    graph_ = new Edge*[n];
    for (int i = 0; i < n; i++) {
        graph_[i] = nullptr;
    }

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
    other.n_ = 0;
    other.graph_ = nullptr;
    other.level_ = nullptr;
    other.iter_ = nullptr;
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

        other.n_ = 0;
        other.graph_ = nullptr;
        other.level_ = nullptr;
        other.iter_ = nullptr;
    }
    return *this;
}

void MaxFlow::clear_graph() {
    if (!graph_) {
        return;
    }
    for (int i = 0; i < n_; i++) {
        Edge* current = graph_[i];
        while (current) {
            Edge* next = current->next;
            delete current;
            current = next;
        }
        graph_[i] = nullptr;
    }
}

bool MaxFlow::bfs(int source, int sink) {
    for (int i = 0; i < n_; i++) {
        level_[i] = -1;
    }

    std::queue<int> q;
    level_[source] = 0;
    q.push(source);

    while (!q.empty()) {
        int v = q.front();
        q.pop();

        Edge* e = graph_[v];
        while (e) {
            if (e->cap > 0 && level_[e->to] < 0) {
                level_[e->to] = level_[v] + 1;
                q.push(e->to);
            }
            e = e->next;
        }
    }

    return level_[sink] >= 0;
}

long long MaxFlow::dfs(int v, int sink, long long f) {
    if (v == sink) {
        return f;
    }

    Edge*& cur = iter_[v];
    while (cur) {
        if (cur->cap > 0 && level_[v] < level_[cur->to]) {
            long long d = dfs(cur->to, sink, f < cur->cap ? f : cur->cap);
            if (d > 0) {
                cur->cap -= d;
                cur->rev->cap += d;
                return d;
            }
        }
        cur = cur->next;
    }

    return 0;
}

void MaxFlow::add_edge(int from, int to, long long capacity) {
    if (from < 0 || from >= n_ || to < 0 || to >= n_) {
        throw std::out_of_range("Vertex index out of range");
    }
    if (capacity < 0) {
        throw std::invalid_argument("Capacity must be non-negative");
    }

    Edge* forward = new Edge(to, capacity);
    Edge* backward = new Edge(from, 0);

    forward->rev = backward;
    backward->rev = forward;

    forward->next = graph_[from];
    graph_[from] = forward;

    backward->next = graph_[to];
    graph_[to] = backward;
}

void MaxFlow::add_edge(int from, int to, long long capacity, long long cost) {
    if (from < 0 || from >= n_ || to < 0 || to >= n_) {
        throw std::out_of_range("Vertex index out of range");
    }
    if (capacity < 0) {
        throw std::invalid_argument("Capacity must be non-negative");
    }

    Edge* forward = new Edge(to, capacity, cost);
    Edge* backward = new Edge(from, 0, -cost);

    forward->rev = backward;
    backward->rev = forward;

    forward->next = graph_[from];
    graph_[from] = forward;

    backward->next = graph_[to];
    graph_[to] = backward;
}

void MaxFlow::add_undirected_edge(int u, int v, long long capacity) {
    add_edge(u, v, capacity);
    add_edge(v, u, capacity);
}

void MaxFlow::add_undirected_edge(int u, int v, long long capacity, long long cost) {
    add_edge(u, v, capacity, cost);
    add_edge(v, u, capacity, cost);
}

long long MaxFlow::edmonds_karp(int source, int sink) {
    if (source < 0 || source >= n_ || sink < 0 || sink >= n_) {
        throw std::out_of_range("Vertex index out of range");
    }
    if (source == sink) {
        return 0;
    }

    long long max_flow = 0;
    std::vector<Edge*> parent_edge(n_);

    while (true) {
        std::fill(parent_edge.begin(), parent_edge.end(), nullptr);
        std::queue<int> q;
        q.push(source);

        while (!q.empty() && !parent_edge[sink]) {
            int v = q.front();
            q.pop();

            Edge* e = graph_[v];
            while (e) {
                if (!parent_edge[e->to] && e->to != source && e->cap > 0) {
                    parent_edge[e->to] = e;
                    q.push(e->to);
                    if (e->to == sink) {
                        break;
                    }
                }
                e = e->next;
            }
        }

        if (!parent_edge[sink]) {
            break;
        }

        long long path_flow = std::numeric_limits<long long>::max();
        int v = sink;
        while (v != source) {
            Edge* e = parent_edge[v];
            if (!e) {
                break;
            }
            if (e->cap < path_flow) {
                path_flow = e->cap;
            }
            v = e->rev->to;
        }

        v = sink;
        while (v != source) {
            Edge* e = parent_edge[v];
            e->cap -= path_flow;
            e->rev->cap += path_flow;
            v = e->rev->to;
        }

        max_flow += path_flow;
    }

    return max_flow;
}

long long MaxFlow::dinic(int source, int sink) {
    if (source < 0 || source >= n_ || sink < 0 || sink >= n_) {
        throw std::out_of_range("Vertex index out of range");
    }
    if (source == sink) {
        return 0;
    }

    long long max_flow = 0;

    while (bfs(source, sink)) {
        for (int i = 0; i < n_; i++) {
            iter_[i] = graph_[i];
        }

        while (true) {
            long long f = dfs(source, sink, std::numeric_limits<long long>::max());
            if (f == 0) {
                break;
            }
            max_flow += f;
        }
    }

    return max_flow;
}

std::pair<long long, long long> MaxFlow::min_cost_max_flow(int source, int sink) {
    if (source < 0 || source >= n_ || sink < 0 || sink >= n_) {
        throw std::out_of_range("Vertex index out of range");
    }
    if (source == sink) {
        return std::pair<long long, long long>(0, 0);
    }

    const long long INF = std::numeric_limits<long long>::max() / 4;
    std::vector<long long> h(n_, 0);
    std::vector<long long> dist(n_);
    std::vector<Edge*> prev_edge(n_);
    std::vector<int> prev_vertex(n_);

    long long flow = 0;
    long long cost = 0;

    while (true) {
        std::fill(dist.begin(), dist.end(), INF);
        std::fill(prev_edge.begin(), prev_edge.end(), static_cast<Edge*>(nullptr));

        dist[source] = 0;

        typedef std::pair<long long, int> Node;
        std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;
        pq.push(Node(0, source));

        while (!pq.empty()) {
            Node cur = pq.top();
            pq.pop();

            long long d = cur.first;
            int v = cur.second;

            if (d != dist[v]) {
                continue;
            }

            Edge* e = graph_[v];
            while (e) {
                if (e->cap > 0) {
                    long long nd = d + e->cost + h[v] - h[e->to];
                    if (nd < dist[e->to]) {
                        dist[e->to] = nd;
                        prev_edge[e->to] = e;
                        prev_vertex[e->to] = v;
                        pq.push(Node(nd, e->to));
                    }
                }
                e = e->next;
            }
        }

        if (dist[sink] == INF) {
            break;
        }

        for (int v = 0; v < n_; v++) {
            if (dist[v] < INF) {
                h[v] += dist[v];
            }
        }

        long long add_flow = std::numeric_limits<long long>::max();
        long long path_cost = 0;
        int v = sink;
        while (v != source) {
            Edge* e = prev_edge[v];
            if (!e) {
                add_flow = 0;
                break;
            }
            if (e->cap < add_flow) {
                add_flow = e->cap;
            }
            path_cost += e->cost;
            v = prev_vertex[v];
        }

        if (add_flow == 0 || add_flow == std::numeric_limits<long long>::max()) {
            break;
        }

        v = sink;
        while (v != source) {
            Edge* e = prev_edge[v];
            e->cap -= add_flow;
            e->rev->cap += add_flow;
            v = prev_vertex[v];
        }

        flow += add_flow;
        cost += add_flow * path_cost;
    }

    return std::pair<long long, long long>(flow, cost);
}

void MaxFlow::min_cut_reachable_from_source(int source, std::vector<char>& reachable) const {
    if (source < 0 || source >= n_) {
        throw std::out_of_range("Vertex index out of range");
    }

    reachable.assign(n_, 0);
    std::queue<int> q;
    reachable[source] = 1;
    q.push(source);

    while (!q.empty()) {
        int v = q.front();
        q.pop();
        Edge* e = graph_[v];
        while (e) {
            if (e->cap > 0 && !reachable[e->to]) {
                reachable[e->to] = 1;
                q.push(e->to);
            }
            e = e->next;
        }
    }
}

bool min_cost_circulation(int n, const std::vector<CirculationEdge>& edges, long long& total_cost) {
    if (n <= 0) {
        total_cost = 0;
        return true;
    }

    long long sum_pos = 0;
    std::vector<long long> balance(n, 0);

    int S = n;
    int T = n + 1;
    MaxFlow mf(n + 2);

    long long base_cost = 0;

    for (std::size_t i = 0; i < edges.size(); i++) {
        CirculationEdge e = edges[i];
        int u = e.from;
        int v = e.to;
        long long lower = e.lower;
        long long upper = e.upper;
        long long cost = e.cost;

        if (u < 0 || u >= n || v < 0 || v >= n) {
            continue;
        }
        if (lower < 0 || upper < lower) {
            continue;
        }

        if (upper > lower) {
            mf.add_edge(u, v, upper - lower, cost);
        }

        balance[u] -= lower;
        balance[v] += lower;
        base_cost += lower * cost;
    }

    for (int i = 0; i < n; i++) {
        if (balance[i] > 0) {
            mf.add_edge(S, i, balance[i], 0);
            sum_pos += balance[i];
        } else if (balance[i] < 0) {
            mf.add_edge(i, T, -balance[i], 0);
        }
    }

    std::pair<long long, long long> res = mf.min_cost_max_flow(S, T);
    if (res.first != sum_pos) {
        total_cost = 0;
        return false;
    }

    total_cost = base_cost + res.second;
    return true;
}

}
