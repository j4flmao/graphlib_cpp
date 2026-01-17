#include "graphlib/bipartite.h"
#include <queue>
#include <stdexcept>
#include <vector>
#include <limits>

namespace graphlib {

BipartiteGraph::BipartiteGraph(int n_left, int n_right)
    : Graph(n_left + n_right, false), n_left_(n_left), n_right_(n_right) {
    if (n_left <= 0 || n_right <= 0) {
        throw std::invalid_argument("Partition sizes must be positive");
    }
}

bool BipartiteGraph::is_bipartite() {
    int total = n_left_ + n_right_;
    int* color = new int[total];
    for (int i = 0; i < total; i++) {
        color[i] = -1;
    }

    std::queue<int> q;
    for (int start = 0; start < total; start++) {
        if (color[start] != -1) {
            continue;
        }
        color[start] = 0;
        q.push(start);

        while (!q.empty()) {
            int v = q.front();
            q.pop();

            Edge* e = get_edges(v);
            while (e) {
                int to = e->to;
                if (color[to] == -1) {
                    color[to] = 1 - color[v];
                    q.push(to);
                } else if (color[to] == color[v]) {
                    delete[] color;
                    return false;
                }
                e = e->next;
            }
        }
    }

    delete[] color;
    return true;
}

bool BipartiteGraph::bfs_hopcroft_karp(int* dist, int* pair_u, int* pair_v) {
    std::queue<int> q;
    const int inf = n_left_ + n_right_ + 1;

    for (int u = 0; u < n_left_; u++) {
        if (pair_u[u] == -1) {
            dist[u] = 0;
            q.push(u);
        } else {
            dist[u] = inf;
        }
    }

    int max_dist = inf;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        if (dist[u] >= max_dist) {
            continue;
        }

        Edge* e = get_edges(u);
        while (e) {
            int v_index = e->to - n_left_;
            if (v_index >= 0 && v_index < n_right_) {
                int matched_u = pair_v[v_index];
                if (matched_u == -1) {
                    max_dist = dist[u] + 1;
                } else if (dist[matched_u] == inf) {
                    dist[matched_u] = dist[u] + 1;
                    q.push(matched_u);
                }
            }
            e = e->next;
        }
    }

    return max_dist != inf;
}

bool BipartiteGraph::dfs_hopcroft_karp(int u, int* dist, int* pair_u, int* pair_v) {
    const int inf = n_left_ + n_right_ + 1;

    Edge* e = get_edges(u);
    while (e) {
        int v_index = e->to - n_left_;
        if (v_index >= 0 && v_index < n_right_) {
            int matched_u = pair_v[v_index];
            if (matched_u == -1 || (dist[matched_u] == dist[u] + 1 && dfs_hopcroft_karp(matched_u, dist, pair_u, pair_v))) {
                pair_u[u] = v_index;
                pair_v[v_index] = u;
                return true;
            }
        }
        e = e->next;
    }

    dist[u] = inf;
    return false;
}

int BipartiteGraph::maximum_matching() {
    int* pair_u = new int[n_left_];
    int* pair_v = new int[n_right_];
    int* dist = new int[n_left_];

    for (int i = 0; i < n_left_; i++) {
        pair_u[i] = -1;
    }
    for (int j = 0; j < n_right_; j++) {
        pair_v[j] = -1;
    }

    int result = 0;

    while (bfs_hopcroft_karp(dist, pair_u, pair_v)) {
        for (int u = 0; u < n_left_; u++) {
            if (pair_u[u] == -1) {
                if (dfs_hopcroft_karp(u, dist, pair_u, pair_v)) {
                    result++;
                }
            }
        }
    }

    delete[] pair_u;
    delete[] pair_v;
    delete[] dist;

    return result;
}

long long BipartiteGraph::hungarian_min_cost(long long inf) {
    if (n_left_ <= 0 || n_right_ <= 0) {
        return 0;
    }

    int n = n_left_;
    int m = n_right_;

    std::vector<std::vector<long long>> cost(n, std::vector<long long>(m, inf));

    for (int u = 0; u < n_left_; u++) {
        Edge* e = get_edges(u);
        while (e) {
            int v_index = e->to - n_left_;
            if (v_index >= 0 && v_index < n_right_) {
                if (e->weight < cost[u][v_index]) {
                    cost[u][v_index] = e->weight;
                }
            }
            e = e->next;
        }
    }

    std::vector<long long> u(n + 1, 0);
    std::vector<long long> v(m + 1, 0);
    std::vector<int> p(m + 1, 0);
    std::vector<int> way(m + 1, 0);

    for (int i = 1; i <= n; i++) {
        p[0] = i;
        long long j0 = 0;
        std::vector<long long> minv(m + 1, inf);
        std::vector<char> used(m + 1, 0);
        do {
            used[j0] = 1;
            int i0 = p[j0];
            long long delta = std::numeric_limits<long long>::max();
            int j1 = 0;
            for (int j = 1; j <= m; j++) {
                if (used[j]) {
                    continue;
                }
                long long cur = cost[i0 - 1][j - 1] - u[i0] - v[j];
                if (cur < minv[j]) {
                    minv[j] = cur;
                    way[j] = static_cast<int>(j0);
                }
                if (minv[j] < delta) {
                    delta = minv[j];
                    j1 = j;
                }
            }
            for (int j = 0; j <= m; j++) {
                if (used[j]) {
                    u[p[j]] += delta;
                    v[j] -= delta;
                } else {
                    minv[j] -= delta;
                }
            }
            j0 = j1;
        } while (p[j0] != 0);

        do {
            int j1 = way[j0];
            p[j0] = p[j1];
            j0 = j1;
        } while (j0 != 0);
    }

    long long result = 0;
    for (int j = 1; j <= m; j++) {
        if (p[j] != 0) {
            int i = p[j] - 1;
            if (i >= 0 && i < n) {
                result += cost[i][j - 1];
            }
        }
    }

    return result;
}

}
