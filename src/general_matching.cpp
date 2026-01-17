#include "graphlib/general_matching.h"
#include <vector>
#include <queue>
#include <algorithm>
#include <limits>

namespace graphlib {

GeneralMatching::GeneralMatching(int n)
    : Graph(n, false) {
}

int GeneralMatching::maximum_matching() {
    int n = vertex_count();
    std::vector<std::vector<int>> g(n);

    for (int u = 0; u < n; u++) {
        Edge* e = get_edges(u);
        while (e) {
            int v = e->to;
            if (v >= 0 && v < n && v != u) {
                g[u].push_back(v);
            }
            e = e->next;
        }
    }

    std::vector<int> match(n, -1);
    std::vector<int> p(n);
    std::vector<int> base(n);
    std::vector<int> q(n);
    std::vector<int> used(n);
    std::vector<int> blossom(n);

    auto lca = [&](int a, int b) {
        std::vector<int> used_lca(n);
        while (true) {
            a = base[a];
            used_lca[a] = 1;
            if (match[a] == -1) {
                break;
            }
            a = p[match[a]];
        }
        while (true) {
            b = base[b];
            if (used_lca[b]) {
                return b;
            }
            if (match[b] == -1) {
                break;
            }
            b = p[match[b]];
        }
        return -1;
    };

    auto mark_path = [&](int v, int b, int children) {
        while (base[v] != b) {
            blossom[base[v]] = 1;
            blossom[base[match[v]]] = 1;
            p[v] = children;
            children = match[v];
            v = p[match[v]];
        }
    };

    auto find_path = [&](int root) {
        std::fill(used.begin(), used.end(), 0);
        std::fill(p.begin(), p.end(), -1);
        for (int i = 0; i < n; i++) {
            base[i] = i;
        }

        int qh = 0;
        int qt = 0;
        q[qt++] = root;
        used[root] = 1;

        while (qh < qt) {
            int v = q[qh++];
            for (int i = 0; i < static_cast<int>(g[v].size()); i++) {
                int to = g[v][i];
                if (base[v] == base[to] || match[v] == to) {
                    continue;
                }
                if (to == root || (match[to] != -1 && p[match[to]] != -1)) {
                    int cur_base = lca(v, to);
                    std::fill(blossom.begin(), blossom.end(), 0);
                    mark_path(v, cur_base, to);
                    mark_path(to, cur_base, v);
                    for (int j = 0; j < n; j++) {
                        if (blossom[base[j]]) {
                            base[j] = cur_base;
                            if (!used[j]) {
                                used[j] = 1;
                                q[qt++] = j;
                            }
                        }
                    }
                } else if (p[to] == -1) {
                    p[to] = v;
                    if (match[to] == -1) {
                        int cur = to;
                        while (cur != -1) {
                            int prev = p[cur];
                            int next = prev == -1 ? -1 : match[prev];
                            match[cur] = prev;
                            if (prev != -1) {
                                match[prev] = cur;
                            }
                            cur = next;
                        }
                        return true;
                    } else {
                        used[to] = 1;
                        q[qt++] = match[to];
                    }
                }
            }
        }

        return false;
    };

    int result = 0;
    for (int i = 0; i < n; i++) {
        if (match[i] == -1) {
            if (find_path(i)) {
                result++;
            }
        }
    }

    return result;
}

long long GeneralMatching::maximum_weight_matching() {
    int n = vertex_count();
    if (n == 0) {
        return 0;
    }

    std::vector<std::vector<long long>> w(n, std::vector<long long>(n, 0));
    for (int u = 0; u < n; u++) {
        Edge* e = get_edges(u);
        while (e) {
            int v = e->to;
            if (v >= 0 && v < n && v != u) {
                long long ww = e->weight;
                if (ww > w[u][v]) {
                    w[u][v] = ww;
                    w[v][u] = ww;
                }
            }
            e = e->next;
        }
    }

    int N = n;
    if (N % 2 == 1) {
        N++;
        w.resize(N, std::vector<long long>(N, 0));
        for (int i = 0; i < N; i++) {
            w[i].resize(N, 0);
        }
    }

    std::vector<long long> lab(N);
    for (int i = 0; i < N; i++) {
        long long mx = 0;
        for (int j = 0; j < N; j++) {
            if (w[i][j] > mx) {
                mx = w[i][j];
            }
        }
        lab[i] = mx;
    }

    std::vector<int> match(N, -1);
    std::vector<int> slackx(N);
    std::vector<long long> slack(N);
    std::vector<int> par(N);
    std::vector<int> q(N);
    std::vector<int> st(N);
    std::vector<int> flower_from(N);
    std::vector<std::vector<int>> flower(N);
    std::vector<std::vector<int>> g(N, std::vector<int>(N));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            g[i][j] = (w[i][j] > 0 ? 1 : 0);
        }
    }

    auto lca = [&](int a, int b) {
        std::vector<int> used(N);
        while (true) {
            a = st[a];
            used[a] = 1;
            if (match[a] == -1) {
                break;
            }
            a = par[match[a]];
        }
        while (true) {
            b = st[b];
            if (used[b]) {
                return b;
            }
            if (match[b] == -1) {
                break;
            }
            b = par[match[b]];
        }
        return -1;
    };

    auto mark_path = [&](int v, int b, int x, std::vector<int>& S) {
        while (st[v] != b) {
            int u = match[v];
            int mv = par[u];
            S[st[v]] = S[st[u]] = 1;
            par[v] = x;
            x = u;
            v = mv;
        }
    };

    auto blossom_contract = [&](int v, int u, int root, std::vector<int>& S, std::vector<int>& vis) {
        (void)root;
        int b = lca(v, u);
        std::fill(S.begin(), S.end(), 0);
        mark_path(v, b, u, S);
        mark_path(u, b, v, S);
        for (int i = 0; i < N; i++) {
            if (S[st[i]]) {
                st[i] = b;
                if (!vis[i]) {
                    vis[i] = 1;
                }
            }
        }
    };

    auto add_to_tree = [&](int v, int root, std::vector<int>& S, std::vector<int>& vis) {
        (void)root;
        (void)vis;
        S[v] = 1;
        par[v] = root;
        for (int u = 0; u < N; u++) {
            if (g[v][u] && st[v] != st[u] && match[v] != u) {
                long long delta = lab[v] + lab[u] - w[st[v]][st[u]];
                if (delta < slack[u]) {
                    slack[u] = delta;
                    slackx[u] = v;
                }
            }
        }
    };

    auto augment_path = [&](int v, int u) {
        int pv = u;
        int nv;
        while (v != -1) {
            nv = match[v];
            match[v] = pv;
            match[pv] = v;
            pv = nv;
            if (pv != -1) {
                v = par[pv];
            } else {
                v = -1;
            }
        }
    };

    auto bfs = [&](int root) {
        std::vector<int> S(N);
        std::vector<int> vis(N);
        std::fill(slack.begin(), slack.end(), std::numeric_limits<long long>::max());
        int qh = 0;
        int qt = 0;
        q[qt++] = root;
        S[root] = 1;
        st[root] = root;
        par[root] = -1;
        while (true) {
            while (qh < qt) {
                int v = q[qh++];
                for (int u = 0; u < N; u++) {
                    if (g[v][u] && st[v] != st[u] && match[v] != u) {
                        long long delta = lab[v] + lab[u] - w[st[v]][st[u]];
                        if (delta < slack[u]) {
                            slack[u] = delta;
                            slackx[u] = v;
                        }
                        if (slack[u] == 0) {
                            if (!S[st[u]]) {
                                if (match[u] == -1) {
                                    augment_path(v, u);
                                    return;
                                }
                                S[st[u]] = 1;
                                q[qt++] = match[u];
                                add_to_tree(match[u], v, S, vis);
                            }
                        }
                    }
                }
            }
            long long delta = std::numeric_limits<long long>::max();
            for (int u = 0; u < N; u++) {
                if (match[u] == -1 && slack[u] < delta) {
                    delta = slack[u];
                }
            }
            for (int v = 0; v < N; v++) {
                if (S[st[v]]) {
                    lab[st[v]] -= delta;
                } else if (match[v] != -1) {
                    lab[st[v]] += delta;
                }
            }
            for (int u = 0; u < N; u++) {
                if (match[u] == -1) {
                    slack[u] -= delta;
                    if (slack[u] == 0) {
                        int v = slackx[u];
                        if (st[v] != st[u]) {
                            if (match[u] == -1) {
                                augment_path(v, u);
                                return;
                            } else {
                                if (!std::count(q.begin(), q.begin() + qt, match[u])) {
                                    q[qt++] = match[u];
                                }
                                add_to_tree(match[u], v, S, vis);
                            }
                        }
                    }
                }
            }
        }
    };

    for (int i = 0; i < N; i++) {
        st[i] = i;
    }

    for (int i = 0; i < N; i++) {
        if (match[i] == -1) {
            bfs(i);
        }
    }

    long long total = 0;
    for (int i = 0; i < n; i++) {
        if (match[i] != -1 && i < match[i]) {
            total += w[i][match[i]];
        }
    }

    return total;
}

}
