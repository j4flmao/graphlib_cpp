#include "graphlib/shortest_path.h"
#include <queue>
#include <stdexcept>
#include <limits>
#include <algorithm>
#include <deque>

namespace graphlib {

ShortestPath::Edge::Edge(int to, long long weight)
    : to(to), weight(weight), next(nullptr) {
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
            int to = e->to;
            long long w = e->weight;
            if (w < 0) {
                e = e->next;
                continue;
            }
            if (dist[to] > dist[cur.v] + w) {
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
    dq.push_back(source);

    while (!dq.empty()) {
        int v = dq.front();
        dq.pop_front();

        Edge* e = adj_[v];
        while (e) {
            int to = e->to;
            long long w = e->weight;
            if (w < 0 || w > 1) {
                e = e->next;
                continue;
            }
            long long cand = dist[v] + w;
            if (cand < dist[to]) {
                dist[to] = cand;
                if (w == 0) {
                    dq.push_front(to);
                } else {
                    dq.push_back(to);
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
        throw std::out_of_range("Source vertex index out of range");
    }

    std::vector<long long> dist(n_, inf);
    dist[source] = 0;

    for (int iter = 0; iter < n_ - 1; iter++) {
        bool updated = false;
        for (int u = 0; u < n_; u++) {
            if (dist[u] == inf) {
                continue;
            }
            Edge* e = adj_[u];
            while (e) {
                int v = e->to;
                long long w = e->weight;
                if (dist[v] > dist[u] + w) {
                    dist[v] = dist[u] + w;
                    updated = true;
                }
                e = e->next;
            }
        }
        if (!updated) {
            break;
        }
    }

    for (int u = 0; u < n_; u++) {
        if (dist[u] == inf) {
            continue;
        }
        Edge* e = adj_[u];
        while (e) {
            int v = e->to;
            long long w = e->weight;
            if (dist[v] > dist[u] + w) {
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

    for (int i = 0; i < n_; i++) {
        dist[i][i] = 0;
        Edge* e = adj_[i];
        while (e) {
            if (e->weight < dist[i][e->to]) {
                dist[i][e->to] = e->weight;
            }
            e = e->next;
        }
    }

    for (int k = 0; k < n_; k++) {
        for (int i = 0; i < n_; i++) {
            if (dist[i][k] == inf) {
                continue;
            }
            for (int j = 0; j < n_; j++) {
                if (dist[k][j] == inf) {
                    continue;
                }
                long long cand = dist[i][k] + dist[k][j];
                if (cand < dist[i][j]) {
                    dist[i][j] = cand;
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

std::vector<std::vector<long long>> ShortestPath::johnson(long long inf, bool& has_negative_cycle) {
    has_negative_cycle = false;
    int n = n_;
    std::vector<long long> h(n, 0);

    for (int iter = 0; iter < n - 1; iter++) {
        bool updated = false;
        for (int u = 0; u < n; u++) {
            Edge* e = adj_[u];
            while (e) {
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

std::vector<int> reconstruct_path(int source, int target, const std::vector<int>& parent) {
    int n = static_cast<int>(parent.size());
    if (source < 0 || target < 0 || source >= n || target >= n) {
        return std::vector<int>();
    }

    std::vector<int> path;
    int cur = target;
    while (cur != -1) {
        if (cur < 0 || cur >= n) {
            return std::vector<int>();
        }
        path.push_back(cur);
        if (cur == source) {
            break;
        }
        cur = parent[cur];
    }

    if (path.empty() || path.back() != source) {
        return std::vector<int>();
    }

    std::reverse(path.begin(), path.end());
    return path;
}

}
