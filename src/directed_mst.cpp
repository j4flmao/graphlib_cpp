#include "graphlib/directed_mst.h"
#include <limits>
#include <stdexcept>
#include <vector>

namespace graphlib {
namespace {
struct Edge { int u, v; long long w; int id; };
struct Result { long long cost = -1; std::vector<int> chosen; };

Result solve(int n, int root, const std::vector<Edge>& edges) {
    std::vector<int> in(n, -1), seen(n, -1), comp(n, -1);
    std::vector<long long> in_cost(n, 0);
    for (int i = 0; i < static_cast<int>(edges.size()); ++i) {
        const auto& e = edges[i];
        if (e.u == e.v || e.v == root) continue;
        if (in[e.v] == -1 || e.w < in_cost[e.v]) { in[e.v] = i; in_cost[e.v] = e.w; }
    }
    for (int v = 0; v < n; ++v) if (v != root && in[v] == -1) return {};

    long long base = 0;
    for (int v = 0; v < n; ++v) if (v != root) base += in_cost[v];
    int cycles = 0;
    for (int start = 0; start < n; ++start) {
        int v = start;
        while (v != root && comp[v] == -1 && seen[v] != start) { seen[v] = start; v = edges[in[v]].u; }
        if (v != root && comp[v] == -1 && seen[v] == start) {
            int x = v;
            do { comp[x] = cycles; x = edges[in[x]].u; } while (x != v);
            ++cycles;
        }
    }
    if (cycles == 0) {
        Result r; r.cost = base;
        for (int v = 0; v < n; ++v) if (v != root) r.chosen.push_back(in[v]);
        return r;
    }
    for (int v = 0; v < n; ++v) if (comp[v] == -1) comp[v] = cycles++;

    struct Map { int original; int entered; };
    std::vector<Edge> next;
    std::vector<Map> maps;
    for (int i = 0; i < static_cast<int>(edges.size()); ++i) {
        const auto& e = edges[i];
        int a = comp[e.u], b = comp[e.v];
        if (a == b) continue;
        int entered = -1;
        long long w = e.w;
        if (e.v != root && in[e.v] != -1 &&
            comp[edges[in[e.v]].u] == comp[e.v]) {
            w -= in_cost[e.v];
            entered = e.v;
        }
        next.push_back({a, b, w, i});
        maps.push_back({i, entered});
    }
    Result upper = solve(cycles, comp[root], next);
    if (upper.cost < 0) return {};
    Result r; r.cost = base + upper.cost;
    std::vector<char> removed(n, 0);
    for (int k : upper.chosen) {
        if (maps[k].entered != -1) removed[maps[k].entered] = 1;
        r.chosen.push_back(maps[k].original);
    }
    for (int v = 0; v < n; ++v) if (v != root && !removed[v]) r.chosen.push_back(in[v]);
    return r;
}
}

long long directed_mst(int n, int root, const std::vector<DirectedEdge>& input,
                       std::vector<int>& result_edges) {
    result_edges.clear();
    if (n < 0 || (n > 0 && (root < 0 || root >= n)) || (n == 0 && root != 0))
        throw std::invalid_argument("Invalid directed MST vertex count or root");
    if (n == 0) return 0;
    std::vector<Edge> edges;
    for (const auto& e : input) {
        if (e.u < 0 || e.u >= n || e.v < 0 || e.v >= n)
            throw std::invalid_argument("Directed edge endpoint is out of range");
        edges.push_back({e.u, e.v, e.weight, e.id});
    }
    Result r = solve(n, root, edges);
    if (r.cost < 0) return -1;
    for (int i : r.chosen) result_edges.push_back(edges[i].id);
    return r.cost;
}
}
