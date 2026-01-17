#include "graphlib/graph_core.h"
#include "graphlib/max_flow.h"
#include <stdexcept>
#include <utility>
#include <vector>
#include <queue>
#include <random>
#include <tuple>
#include <limits>
#include <algorithm>

namespace graphlib {

Edge::Edge(int to, long long weight)
    : to(to), weight(weight), reverse_idx(-1), next(nullptr) {
}

Edge::~Edge() {
}

Graph::Graph(int n, bool directed)
    : n_(n), adj_(nullptr), directed_(directed) {
    if (n <= 0) {
        throw std::invalid_argument("Number of vertices must be positive");
    }

    adj_ = new Edge*[n];
    for (int i = 0; i < n; i++) {
        adj_[i] = nullptr;
    }
}

Graph::~Graph() {
    clear_adj_list();
    delete[] adj_;
}

Graph::Graph(Graph&& other) noexcept
    : n_(other.n_), adj_(other.adj_), directed_(other.directed_) {
    other.adj_ = nullptr;
    other.n_ = 0;
}

Graph& Graph::operator=(Graph&& other) noexcept {
    if (this != &other) {
        clear_adj_list();
        delete[] adj_;

        n_ = other.n_;
        adj_ = other.adj_;
        directed_ = other.directed_;

        other.adj_ = nullptr;
        other.n_ = 0;
    }
    return *this;
}

void Graph::clear_adj_list() {
    if (!adj_) {
        return;
    }
    for (int i = 0; i < n_; i++) {
        Edge* current = adj_[i];
        while (current) {
            Edge* next = current->next;
            delete current;
            current = next;
        }
        adj_[i] = nullptr;
    }
}

void Graph::add_edge(int from, int to, long long weight) {
    if (from < 0 || from >= n_ || to < 0 || to >= n_) {
        throw std::out_of_range("Vertex index out of range");
    }

    Edge* new_edge = new Edge(to, weight);
    new_edge->next = adj_[from];
    adj_[from] = new_edge;

    if (!directed_) {
        Edge* rev_edge = new Edge(from, weight);
        rev_edge->next = adj_[to];
        adj_[to] = rev_edge;
    }
}

Graph make_complete_graph(int n, bool directed) {
    Graph g(n, directed);
    if (directed) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (i == j) {
                    continue;
                }
                g.add_edge(i, j);
            }
        }
    } else {
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                g.add_edge(i, j);
            }
        }
    }
    return g;
}

Graph make_random_graph(int n, int m, bool directed, long long min_weight, long long max_weight, unsigned int seed) {
    if (m < 0) {
        m = 0;
    }
    if (min_weight > max_weight) {
        long long tmp = min_weight;
        min_weight = max_weight;
        max_weight = tmp;
    }

    Graph g(n, directed);
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist_vertex(0, n - 1);
    std::uniform_int_distribution<long long> dist_weight(min_weight, max_weight);

    int added = 0;
    int limit = m * 5 + 10;
    int attempts = 0;

    while (added < m && attempts < limit) {
        attempts++;
        int u = dist_vertex(rng);
        int v = dist_vertex(rng);
        if (u == v) {
            continue;
        }
        long long w = dist_weight(rng);
        g.add_edge(u, v, w);
        added++;
    }

    return g;
}

bool is_connected(const Graph& g) {
    int n = g.vertex_count();
    if (n <= 1) {
        return true;
    }

    std::vector<char> visited(n, 0);
    std::queue<int> q;
    visited[0] = 1;
    q.push(0);

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        Edge* e = g.get_edges(u);
        while (e) {
            int v = e->to;
            if (!visited[v]) {
                visited[v] = 1;
                q.push(v);
            }
            e = e->next;
        }
    }

    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            return false;
        }
    }
    return true;
}

bool is_tree(const Graph& g) {
    if (g.is_directed()) {
        return false;
    }

    int n = g.vertex_count();
    if (n <= 0) {
        return false;
    }

    long long edge_count_twice = 0;
    for (int i = 0; i < n; i++) {
        Edge* e = g.get_edges(i);
        while (e) {
            edge_count_twice++;
            e = e->next;
        }
    }

    long long edges = edge_count_twice / 2;
    if (edges != n - 1) {
        return false;
    }

    if (!is_connected(g)) {
        return false;
    }

    return true;
}

Graph make_graph_from_edges(int n, const std::vector<std::pair<int, int>>& edges, bool directed) {
    Graph g(n, directed);
    for (std::size_t i = 0; i < edges.size(); i++) {
        int u = edges[i].first;
        int v = edges[i].second;
        g.add_edge(u, v);
    }
    return g;
}

Graph make_graph_from_edges_one_based(int n, const std::vector<std::pair<int, int>>& edges, bool directed) {
    Graph g(n, directed);
    for (std::size_t i = 0; i < edges.size(); i++) {
        int u = edges[i].first;
        int v = edges[i].second;
        if (u < 1 || u > n || v < 1 || v > n) {
            throw std::out_of_range("Vertex index out of range in make_graph_from_edges_one_based");
        }
        g.add_edge(u - 1, v - 1);
    }
    return g;
}

Graph make_weighted_graph_from_edges(int n, const std::vector<std::tuple<int, int, long long>>& edges, bool directed) {
    Graph g(n, directed);
    for (std::size_t i = 0; i < edges.size(); i++) {
        int u = static_cast<int>(std::get<0>(edges[i]));
        int v = static_cast<int>(std::get<1>(edges[i]));
        long long w = std::get<2>(edges[i]);
        g.add_edge(u, v, w);
    }
    return g;
}

Graph make_weighted_graph_from_edges_one_based(int n, const std::vector<std::tuple<int, int, long long>>& edges, bool directed) {
    Graph g(n, directed);
    for (std::size_t i = 0; i < edges.size(); i++) {
        int u = static_cast<int>(std::get<0>(edges[i]));
        int v = static_cast<int>(std::get<1>(edges[i]));
        long long w = std::get<2>(edges[i]);
        if (u < 1 || u > n || v < 1 || v > n) {
            throw std::out_of_range("Vertex index out of range in make_weighted_graph_from_edges_one_based");
        }
        g.add_edge(u - 1, v - 1, w);
    }
    return g;
}

std::vector<int> bfs_distances(const Graph& g, int source, int unreachable) {
    int n = g.vertex_count();
    if (source < 0 || source >= n) {
        throw std::out_of_range("Source vertex index out of range in bfs_distances");
    }

    std::vector<int> dist(n, unreachable);
    std::queue<int> q;
    dist[source] = 0;
    q.push(source);

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        Edge* e = g.get_edges(u);
        while (e) {
            int v = e->to;
            if (dist[v] == unreachable) {
                dist[v] = dist[u] + 1;
                q.push(v);
            }
            e = e->next;
        }
    }

    return dist;
}

std::vector<int> bfs_multi_source(const Graph& g, const std::vector<int>& sources, int unreachable) {
    int n = g.vertex_count();
    std::vector<int> dist(n, unreachable);
    std::queue<int> q;

    for (std::size_t i = 0; i < sources.size(); i++) {
        int s = sources[i];
        if (s < 0 || s >= n) {
            continue;
        }
        if (dist[s] != unreachable) {
            continue;
        }
        dist[s] = 0;
        q.push(s);
    }

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        Edge* e = g.get_edges(u);
        while (e) {
            int v = e->to;
            if (dist[v] == unreachable) {
                dist[v] = dist[u] + 1;
                q.push(v);
            }
            e = e->next;
        }
    }

    return dist;
}

bool has_eulerian_trail_undirected(const Graph& g) {
    if (g.is_directed()) {
        return false;
    }

    int n = g.vertex_count();
    std::vector<int> degree(n, 0);
    int edge_count = 0;

    for (int u = 0; u < n; u++) {
        Edge* e = g.get_edges(u);
        while (e) {
            int v = e->to;
            if (u <= v) {
                degree[u]++;
                degree[v]++;
                edge_count++;
            }
            e = e->next;
        }
    }

    if (edge_count == 0) {
        return true;
    }

    int start = -1;
    for (int i = 0; i < n; i++) {
        if (degree[i] > 0) {
            start = i;
            break;
        }
    }
    if (start == -1) {
        return true;
    }

    std::vector<char> visited(n, 0);
    std::queue<int> q;
    visited[start] = 1;
    q.push(start);

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        Edge* e = g.get_edges(u);
        while (e) {
            int v = e->to;
            if (!visited[v]) {
                visited[v] = 1;
                q.push(v);
            }
            e = e->next;
        }
    }

    for (int i = 0; i < n; i++) {
        if (degree[i] > 0 && !visited[i]) {
            return false;
        }
    }

    int odd = 0;
    for (int i = 0; i < n; i++) {
        if (degree[i] % 2 != 0) {
            odd++;
        }
    }

    if (odd == 0 || odd == 2) {
        return true;
    }
    return false;
}

bool has_eulerian_cycle_undirected(const Graph& g) {
    if (!has_eulerian_trail_undirected(g)) {
        return false;
    }

    int n = g.vertex_count();
    std::vector<int> degree(n, 0);

    for (int u = 0; u < n; u++) {
        Edge* e = g.get_edges(u);
        while (e) {
            int v = e->to;
            if (u <= v) {
                degree[u]++;
                degree[v]++;
            }
            e = e->next;
        }
    }

    for (int i = 0; i < n; i++) {
        if (degree[i] % 2 != 0) {
            return false;
        }
    }
    return true;
}

std::vector<int> eulerian_trail_undirected(const Graph& g) {
    if (!has_eulerian_trail_undirected(g)) {
        return std::vector<int>();
    }
    if (g.is_directed()) {
        return std::vector<int>();
    }

    int n = g.vertex_count();
    std::vector<std::vector<std::pair<int, int>>> adj(n);
    std::vector<int> degree(n, 0);
    int edge_count = 0;

    for (int u = 0; u < n; u++) {
        Edge* e = g.get_edges(u);
        while (e) {
            int v = e->to;
            if (u <= v) {
                adj[u].push_back(std::pair<int, int>(v, edge_count));
                adj[v].push_back(std::pair<int, int>(u, edge_count));
                degree[u]++;
                degree[v]++;
                edge_count++;
            }
            e = e->next;
        }
    }

    if (edge_count == 0) {
        return std::vector<int>();
    }

    int start = 0;
    for (int i = 0; i < n; i++) {
        if (degree[i] % 2 != 0) {
            start = i;
            break;
        }
        if (degree[i] > 0) {
            start = i;
        }
    }

    std::vector<int> it(n, 0);
    std::vector<char> used(edge_count, 0);
    std::vector<int> stack;
    std::vector<int> path;

    stack.push_back(start);
    while (!stack.empty()) {
        int v = stack.back();
        while (it[v] < static_cast<int>(adj[v].size()) && used[adj[v][it[v]].second]) {
            it[v]++;
        }
        if (it[v] == static_cast<int>(adj[v].size())) {
            path.push_back(v);
            stack.pop_back();
        } else {
            int to = adj[v][it[v]].first;
            int id = adj[v][it[v]].second;
            it[v]++;
            if (used[id]) {
                continue;
            }
            used[id] = 1;
            stack.push_back(to);
        }
    }

    if (static_cast<int>(path.size()) != edge_count + 1) {
        return std::vector<int>();
    }

    std::reverse(path.begin(), path.end());
    return path;
}

bool has_eulerian_trail_directed(const Graph& g) {
    if (!g.is_directed()) {
        return false;
    }

    int n = g.vertex_count();
    std::vector<int> indeg(n, 0);
    std::vector<int> outdeg(n, 0);
    std::vector<std::vector<int>> und(n);

    int edge_count = 0;
    for (int u = 0; u < n; u++) {
        Edge* e = g.get_edges(u);
        while (e) {
            int v = e->to;
            outdeg[u]++;
            indeg[v]++;
            und[u].push_back(v);
            und[v].push_back(u);
            edge_count++;
            e = e->next;
        }
    }

    if (edge_count == 0) {
        return true;
    }

    int start = -1;
    for (int i = 0; i < n; i++) {
        if (indeg[i] + outdeg[i] > 0) {
            start = i;
            break;
        }
    }
    if (start == -1) {
        return true;
    }

    std::vector<char> visited(n, 0);
    std::queue<int> q;
    visited[start] = 1;
    q.push(start);

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (std::size_t i = 0; i < und[u].size(); i++) {
            int v = und[u][i];
            if (!visited[v]) {
                visited[v] = 1;
                q.push(v);
            }
        }
    }

    for (int i = 0; i < n; i++) {
        if (indeg[i] + outdeg[i] > 0 && !visited[i]) {
            return false;
        }
    }

    int start_candidates = 0;
    int end_candidates = 0;
    for (int i = 0; i < n; i++) {
        int diff = outdeg[i] - indeg[i];
        if (diff == 1) {
            start_candidates++;
        } else if (diff == -1) {
            end_candidates++;
        } else if (diff != 0) {
            return false;
        }
    }

    if (start_candidates == 0 && end_candidates == 0) {
        return true;
    }
    if (start_candidates == 1 && end_candidates == 1) {
        return true;
    }
    return false;
}

bool has_eulerian_cycle_directed(const Graph& g) {
    if (!has_eulerian_trail_directed(g)) {
        return false;
    }

    if (!g.is_directed()) {
        return false;
    }

    int n = g.vertex_count();
    std::vector<int> indeg(n, 0);
    std::vector<int> outdeg(n, 0);

    for (int u = 0; u < n; u++) {
        Edge* e = g.get_edges(u);
        while (e) {
            int v = e->to;
            outdeg[u]++;
            indeg[v]++;
            e = e->next;
        }
    }

    for (int i = 0; i < n; i++) {
        if (indeg[i] != outdeg[i]) {
            return false;
        }
    }
    return true;
}

std::vector<int> eulerian_trail_directed(const Graph& g) {
    if (!has_eulerian_trail_directed(g)) {
        return std::vector<int>();
    }
    if (!g.is_directed()) {
        return std::vector<int>();
    }

    int n = g.vertex_count();
    std::vector<std::vector<std::pair<int, int>>> adj(n);
    int edge_count = 0;

    std::vector<int> indeg(n, 0);
    std::vector<int> outdeg(n, 0);

    for (int u = 0; u < n; u++) {
        Edge* e = g.get_edges(u);
        while (e) {
            int v = e->to;
            adj[u].push_back(std::pair<int, int>(v, edge_count));
            outdeg[u]++;
            indeg[v]++;
            edge_count++;
            e = e->next;
        }
    }

    if (edge_count == 0) {
        return std::vector<int>();
    }

    int start = 0;
    for (int i = 0; i < n; i++) {
        int diff = outdeg[i] - indeg[i];
        if (diff == 1) {
            start = i;
            break;
        }
        if (outdeg[i] > 0) {
            start = i;
        }
    }

    std::vector<int> it(n, 0);
    std::vector<char> used(edge_count, 0);
    std::vector<int> stack;
    std::vector<int> path;

    stack.push_back(start);
    while (!stack.empty()) {
        int v = stack.back();
        while (it[v] < static_cast<int>(adj[v].size()) && used[adj[v][it[v]].second]) {
            it[v]++;
        }
        if (it[v] == static_cast<int>(adj[v].size())) {
            path.push_back(v);
            stack.pop_back();
        } else {
            int to = adj[v][it[v]].first;
            int id = adj[v][it[v]].second;
            it[v]++;
            if (used[id]) {
                continue;
            }
            used[id] = 1;
            stack.push_back(to);
        }
    }

    if (static_cast<int>(path.size()) != edge_count + 1) {
        return std::vector<int>();
    }

    std::reverse(path.begin(), path.end());
    return path;
}

long long global_min_cut_undirected(const Graph& g) {
    if (g.is_directed()) {
        throw std::invalid_argument("global_min_cut_undirected requires an undirected graph");
    }

    int n = g.vertex_count();
    if (n <= 1) {
        return 0;
    }

    std::vector<std::vector<long long>> w(n, std::vector<long long>(n, 0));
    for (int u = 0; u < n; u++) {
        Edge* e = g.get_edges(u);
        while (e) {
            int v = e->to;
            if (u <= v && u != v) {
                long long weight = e->weight;
                w[u][v] += weight;
                w[v][u] += weight;
            }
            e = e->next;
        }
    }

    std::vector<int> vertices(n);
    for (int i = 0; i < n; i++) {
        vertices[i] = i;
    }

    long long best = std::numeric_limits<long long>::max();
    std::vector<long long> agg(n);
    std::vector<char> added(n);

    int size = n;
    while (size > 1) {
        for (int i = 0; i < n; i++) {
            agg[i] = 0;
            added[i] = 0;
        }

        int prev = -1;
        int last = -1;

        for (int i = 0; i < size; i++) {
            int select_index = -1;
            for (int j = 0; j < size; j++) {
                int v = vertices[j];
                if (!added[v] && (select_index == -1 || agg[v] > agg[vertices[select_index]])) {
                    select_index = j;
                }
            }

            int v = vertices[select_index];
            added[v] = 1;

            if (i == size - 1) {
                last = v;
                if (agg[v] < best) {
                    best = agg[v];
                }
                if (prev != -1) {
                    for (int j = 0; j < n; j++) {
                        w[prev][j] += w[v][j];
                        w[j][prev] = w[prev][j];
                    }
                    vertices.erase(vertices.begin() + select_index);
                    size--;
                }
            } else {
                prev = v;
                for (int j = 0; j < n; j++) {
                    if (!added[j]) {
                        agg[j] += w[v][j];
                    }
                }
            }
        }
    }

    if (best == std::numeric_limits<long long>::max()) {
        return 0;
    }
    return best;
}

void gomory_hu_tree(const Graph& g, std::vector<int>& parent, std::vector<long long>& min_cut) {
    if (g.is_directed()) {
        throw std::invalid_argument("gomory_hu_tree requires an undirected graph");
    }

    int n = g.vertex_count();
    parent.assign(n, 0);
    min_cut.assign(n, 0);

    if (n <= 1) {
        if (n == 1) {
            parent[0] = 0;
            min_cut[0] = 0;
        }
        return;
    }

    std::vector<std::vector<long long>> w(n, std::vector<long long>(n, 0));
    for (int u = 0; u < n; u++) {
        Edge* e = g.get_edges(u);
        while (e) {
            int v = e->to;
            if (u <= v && u != v) {
                long long weight = e->weight;
                w[u][v] += weight;
                w[v][u] += weight;
            }
            e = e->next;
        }
    }

    parent[0] = 0;
    for (int i = 1; i < n; i++) {
        parent[i] = 0;
    }

    for (int s = 1; s < n; s++) {
        int t = parent[s];
        if (t == s) {
            continue;
        }

        MaxFlow mf(n);
        for (int u = 0; u < n; u++) {
            for (int v = u + 1; v < n; v++) {
                long long cap = w[u][v];
                if (cap > 0) {
                    mf.add_undirected_edge(u, v, cap);
                }
            }
        }

        long long flow = mf.dinic(s, t);
        min_cut[s] = flow;

        std::vector<char> reachable;
        mf.min_cut_reachable_from_source(s, reachable);

        for (int i = 0; i < n; i++) {
            if (i != s && parent[i] == t && reachable[i]) {
                parent[i] = s;
            }
        }

        if (reachable[parent[t]]) {
            parent[s] = parent[t];
            parent[t] = s;
            long long tmp = min_cut[s];
            min_cut[s] = min_cut[t];
            min_cut[t] = tmp;
        }
    }
}

}
