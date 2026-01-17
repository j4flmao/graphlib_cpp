#include "graphlib/mst.h"
#include <algorithm>
#include <queue>
#include <stdexcept>

namespace graphlib {

UnionFind::UnionFind(int n)
    : n_(n), parent_(nullptr), rank_(nullptr) {
    if (n <= 0) {
        throw std::invalid_argument("Size must be positive");
    }

    parent_ = new int[n];
    rank_ = new int[n];
    for (int i = 0; i < n; i++) {
        parent_[i] = i;
        rank_[i] = 0;
    }
}

UnionFind::~UnionFind() {
    delete[] parent_;
    delete[] rank_;
}

UnionFind::UnionFind(UnionFind&& other) noexcept
    : n_(other.n_), parent_(other.parent_), rank_(other.rank_) {
    other.n_ = 0;
    other.parent_ = nullptr;
    other.rank_ = nullptr;
}

UnionFind& UnionFind::operator=(UnionFind&& other) noexcept {
    if (this != &other) {
        delete[] parent_;
        delete[] rank_;

        n_ = other.n_;
        parent_ = other.parent_;
        rank_ = other.rank_;

        other.n_ = 0;
        other.parent_ = nullptr;
        other.rank_ = nullptr;
    }
    return *this;
}

int UnionFind::find(int x) {
    if (x < 0 || x >= n_) {
        throw std::out_of_range("Index out of range");
    }
    if (parent_[x] != x) {
        parent_[x] = find(parent_[x]);
    }
    return parent_[x];
}

bool UnionFind::unite(int x, int y) {
    int rx = find(x);
    int ry = find(y);
    if (rx == ry) {
        return false;
    }
    if (rank_[rx] < rank_[ry]) {
        parent_[rx] = ry;
    } else if (rank_[rx] > rank_[ry]) {
        parent_[ry] = rx;
    } else {
        parent_[ry] = rx;
        rank_[rx]++;
    }
    return true;
}

long long MST::kruskal(int n, std::vector<MstEdge>& edges) {
    if (n <= 0) {
        throw std::invalid_argument("Number of vertices must be positive");
    }

    std::sort(edges.begin(), edges.end());
    UnionFind uf(n);
    long long total = 0;
    int used = 0;

    for (std::size_t i = 0; i < edges.size(); i++) {
        int u = edges[i].u;
        int v = edges[i].v;
        if (u < 0 || u >= n || v < 0 || v >= n) {
            throw std::out_of_range("Vertex index out of range");
        }
        if (uf.unite(u, v)) {
            total += edges[i].weight;
            used++;
            if (used == n - 1) {
                break;
            }
        }
    }

    return total;
}

long long MST::prim(int n, int start, const std::vector<std::vector<std::pair<int, long long>>>& adj, long long inf) {
    if (n <= 0) {
        throw std::invalid_argument("Number of vertices must be positive");
    }
    if (start < 0 || start >= n) {
        throw std::out_of_range("Start vertex out of range");
    }

    std::vector<long long> dist(n, inf);
    std::vector<bool> used(n, false);
    dist[start] = 0;

    using Node = std::pair<long long, int>;
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;
    pq.push(Node(0, start));

    long long total = 0;

    while (!pq.empty()) {
        Node cur = pq.top();
        pq.pop();
        int v = cur.second;
        if (used[v]) {
            continue;
        }
        used[v] = true;
        total += cur.first;

        if (v >= 0 && v < static_cast<int>(adj.size())) {
            const std::vector<std::pair<int, long long>>& edges = adj[v];
            for (std::size_t i = 0; i < edges.size(); i++) {
                int to = edges[i].first;
                long long w = edges[i].second;
                if (to < 0 || to >= n) {
                    continue;
                }
                if (!used[to] && w < dist[to]) {
                    dist[to] = w;
                    pq.push(Node(dist[to], to));
                }
            }
        }
    }

    return total;
}

}

