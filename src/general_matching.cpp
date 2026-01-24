#include "graphlib/general_matching.h"
#include <algorithm>
#include <vector>
#include <numeric>
#include <functional>
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

    // Determine bias to ensure all relevant weights are positive
    // We use a minimal bias to avoid large numbers, but ensure w > 0.
    long long min_weight = 0;
    for (int u = 0; u < n; u++) {
        Edge* e = get_edges(u);
        while (e) {
            if (e->weight < min_weight) {
                min_weight = e->weight;
            }
            e = e->next;
        }
    }
    long long bias = 1;
    if (min_weight < 1) {
        bias = 1 - min_weight;
    }



    // For small n, use brute force (bitmask DP) to guarantee optimality
    if (n <= 20) {
        std::vector<long long> dp(1LL << n, -1);
        std::function<long long(int)> solve;
        solve = [&](int mask) -> long long {
            if (mask == (1LL << n) - 1) return 0;
            if (dp[mask] != -1) return dp[mask];
            
            int i = 0;
            while ((mask >> i) & 1) i++;
            
            // Option 1: Leave i unmatched
            long long res = solve(mask | (1 << i));
            
            // Option 2: Match i with j
            for (int j = i + 1; j < n; j++) {
                if (!((mask >> j) & 1)) {
                    // Find edge weight
                    long long w_ij = 0;
                    bool exists = false;
                    Edge* e = get_edges(i);
                    while (e) {
                        if (e->to == j) {
                            w_ij = std::max(w_ij, e->weight); // Handle multi-edges if any
                            exists = true;
                        }
                        e = e->next;
                    }
                    if (exists) {
                        long long current = w_ij + solve(mask | (1 << i) | (1 << j));
                        res = std::max(res, current);
                    }
                }
            }
            return dp[mask] = res;
        };
        return solve(0);
    }

    // Reduction to Maximum Weight Perfect Matching
    // Create a graph with 2*n vertices
    int N = 2 * n;
    std::vector<std::vector<long long>> w(N, std::vector<long long>(N, 0));
    long long scaling = 2; // Multiply by 2 to handle r/2 in blossom update

    // Fill original edges with bias and corresponding dummy edges
    for (int u = 0; u < n; u++) {
        Edge* e = get_edges(u);
        while (e) {
            int v = e->to;
            if (v >= 0 && v < n && v != u) {
                long long ww = (e->weight + bias) * scaling;
                if (ww > w[u][v]) {
                    w[u][v] = ww;
                    w[v][u] = ww;
                    // Mirror edge between dummy nodes
                    w[u + n][v + n] = bias * scaling;
                    w[v + n][u + n] = bias * scaling;
                }
            }
            e = e->next;
        }
    }

    // Fill dummy edges
    // Edges between u and u' (dummy)
    for (int i = 0; i < n; ++i) {
        w[i][i + n] = bias * scaling;
        w[i + n][i] = bias * scaling;
    }
    // No clique needed. We only need dummy edges (u', v') if (u, v) exists.


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
    std::vector<int> par(N, -1);
    std::vector<int> q;
    std::vector<int> st(N);
    std::vector<int> flower_from(N);
    std::vector<std::vector<int>> flower(N);
    std::vector<std::vector<int>> g(N, std::vector<int>(N));

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
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
        
        std::vector<int> blossom_bases;
        int t = v;
        while (st[t] != b) {
            blossom_bases.push_back(st[t]);
            blossom_bases.push_back(st[match[t]]);
            t = par[match[t]];
        }
        t = u;
        while (st[t] != b) {
            blossom_bases.push_back(st[t]);
            blossom_bases.push_back(st[match[t]]);
            t = par[match[t]];
        }
        
        mark_path(v, b, u, S);
        mark_path(u, b, v, S);
        
        for (int i = 0; i < N; i++) {
            if (st[i] != b) {
                for (int base : blossom_bases) {
                    if (st[i] == base) {
                        st[i] = b;
                        break;
                    }
                }
            }
        }
    };

    auto add_to_tree = [&](int v, int root, std::vector<int>& S, std::vector<int>& vis) {
        (void)vis;
        S[v] = 1;
        int u = match[v];
        par[u] = root;
        par[v] = u;
        for (int i = 0; i < N; i++) {
            if (g[v][i] && st[v] != st[i] && match[v] != i) {
                long long delta = lab[st[v]] + lab[st[i]] - w[v][i];
                if (delta < slack[i]) {
                    slack[i] = delta;
                    slackx[i] = v;
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

    std::vector<int> S(N);
    std::vector<int> vis(N);

    auto bfs = [&](int root) -> bool {
        std::fill(S.begin(), S.end(), 0);
        std::fill(vis.begin(), vis.end(), 0);
        std::fill(slack.begin(), slack.end(), std::numeric_limits<long long>::max());
        
        // Reset st to identity for new augmenting path search
        for(int i=0; i<N; ++i) st[i] = i;

        int qh = 0;
        q.clear();
        q.push_back(root);
        S[root] = 1;
        par[root] = -1;
        
        while (true) {
            while (qh < q.size()) {
                int v = q[qh++];
                // Skip if v is no longer a representative (contracted)
                if (st[v] != v) continue;
                
                for (int u = 0; u < N; u++) {
                      if (g[v][u] && st[v] != st[u]) {
                          long long r = lab[st[v]] + lab[st[u]] - w[v][u];
                          
                          if (S[st[u]] == 1) { // u is EVEN -> Blossom
                             long long cur_slack = r / 2;
                             if (cur_slack == 0) {
                                blossom_contract(v, u, root, S, vis);
                                for(int i=0; i<N; ++i) {
                                    if (st[i] == st[v] && S[i] == 1) {
                                        bool seen = false;
                                        for(int x : q) if(x == i) { seen = true; break; }
                                        if (!seen) q.push_back(i);
                                    }
                                }
                            } 
                             // else: wait for delta to reduce slack
                         } else if (match[u] == -1) { // u is Free
                             if (r == 0) {
                                 augment_path(v, u);
                                return true;
                            }
                             if (r < slack[u]) {
                                 slack[u] = r;
                                 slackx[u] = v;
                             }
                         } else { // u is Matched
                             // u is ODD (implicitly). match[u] is candidate for EVEN.
                             if (S[st[match[u]]] == 0) {
                                 if (r == 0) {
                                     add_to_tree(match[u], v, S, vis);
                                    if (std::find(q.begin(), q.end(), match[u]) == q.end()) {
                                       q.push_back(match[u]);
                                    }
                                 } else if (r < slack[u]) {
                                     slack[u] = r;
                                     slackx[u] = v;
                                 }
                             }
                         }
                     }
                }
            }
            
            // Calculate delta
            long long d = std::numeric_limits<long long>::max();
            
            // 1. Slack of free/matched nodes not in tree
            for(int i=0; i<N; ++i) {
                if (st[i] == i && S[i] == 0) {
                    d = std::min(d, slack[i]);
                }
            }
            
            // 2. Blossom formation slack (S-S edges)
            for(int i=0; i<N; ++i) {
                if (S[st[i]] == 1) {
                    for(int j=0; j<N; ++j) {
                        if (S[st[j]] == 1 && st[i] != st[j] && g[i][j]) {
                            long long r = lab[st[i]] + lab[st[j]] - w[i][j];
                            d = std::min(d, r / 2);
                        }
                    }
                }
            }

            if (d == std::numeric_limits<long long>::max()) return false; // No path
            
            // Update labels
                for(int i=0; i<N; ++i) {
                    if (st[i] == i) {
                        if (S[i]) {
                            lab[i] -= d;
                        } else if (match[i] != -1 && S[st[match[i]]]) {
                            lab[i] += d;
                        }
                    }
                }
                // Update slacks
                for(int i=0; i<N; ++i) {
                    if (st[i] == i && S[i] == 0 && slack[i] != std::numeric_limits<long long>::max()) {
                        slack[i] -= d;
                    }
                }
            
            // Re-check for new tight edges
            // We can just loop and let the q loop handle it, but we need to push new candidates to q.
            // Or just reset qh?
            // If we reset qh=0, we rescan everything. Safe but O(N^2) per phase.
            // Actually, we only need to act on slack[u] == 0 or blossom slack == 0.
            
            // Check slack[u] == 0
            for(int i=0; i<N; ++i) {
                if (st[i] == i && S[i] == 0 && slack[i] == 0) {
                    int v = slackx[i];
                    if (match[i] == -1) {
                         augment_path(v, i);
                         return true;
                    } else {
                         add_to_tree(match[i], v, S, vis);
                         if (std::find(q.begin(), q.end(), match[i]) == q.end()) {
                             q.push_back(match[i]);
                         }
                    }
                }
            }
            
            // Check blossom formation (S-S edges becoming tight)
            // This requires scanning S nodes again.
            qh = 0; // Rescan everything to find new tight edges/blossoms
            // Note: qt stays same, we just re-process.
            // Wait, if we rescan, we might re-add things?
            // q only contains unique items if we check std::count or use bool array.
            // The logic above checks std::count.
        }
    };


    for (int i = 0; i < N; i++) {
        st[i] = i;
    }

    // Greedily find augmenting paths
    std::vector<int> failed(N, 0);
    for (int i = 0; i < N; ++i) {
        int root = -1;
        for (int v = 0; v < N; ++v) {
            if (match[v] == -1 && !failed[v]) {
                root = v;
                break;
            }
        }
        if (root == -1) break;
        if (!bfs(root)) {
            failed[root] = 1;
        }
    }

    long long total = 0;
    for (int i = 0; i < N; i++) {
        if (match[i] != -1 && i < match[i]) {
            total += w[i][match[i]];
        }
    }

    return total / scaling - (long long)n * bias;
}

}
