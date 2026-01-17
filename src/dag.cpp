#include "graphlib/dag.h"
#include <queue>
#include <limits>
#include <algorithm>
#include <stdexcept>

namespace {

void dfs_visit(graphlib::DAG& dag, int v, std::vector<int>& state, std::vector<int>& order, bool& has_cycle) {
    if (has_cycle) {
        return;
    }
    state[v] = 1;
    graphlib::Edge* e = dag.get_edges(v);
    while (e) {
        int to = e->to;
        if (state[to] == 0) {
            dfs_visit(dag, to, state, order, has_cycle);
            if (has_cycle) {
                return;
            }
        } else if (state[to] == 1) {
            has_cycle = true;
            return;
        }
        e = e->next;
    }
    state[v] = 2;
    order.push_back(v);
}

}

namespace graphlib {

DAG::DAG(int n)
    : Graph(n, true) {
}

std::vector<int> DAG::topological_sort_kahn(bool& has_cycle) {
    int n = vertex_count();
    std::vector<int> in_degree(n, 0);

    for (int u = 0; u < n; u++) {
        Edge* e = get_edges(u);
        while (e) {
            in_degree[e->to]++;
            e = e->next;
        }
    }

    std::queue<int> q;
    for (int i = 0; i < n; i++) {
        if (in_degree[i] == 0) {
            q.push(i);
        }
    }

    std::vector<int> order;
    order.reserve(n);

    while (!q.empty()) {
        int v = q.front();
        q.pop();
        order.push_back(v);

        Edge* e = get_edges(v);
        while (e) {
            int to = e->to;
            in_degree[to]--;
            if (in_degree[to] == 0) {
                q.push(to);
            }
            e = e->next;
        }
    }

    has_cycle = static_cast<int>(order.size()) != n;
    return order;
}

std::vector<int> DAG::topological_sort_dfs(bool& has_cycle) {
    int n = vertex_count();
    std::vector<int> order;
    order.reserve(n);
    std::vector<int> state(n, 0);
    has_cycle = false;

    for (int i = 0; i < n; i++) {
        if (state[i] == 0) {
            dfs_visit(*this, i, state, order, has_cycle);
            if (has_cycle) {
                break;
            }
        }
    }

    if (has_cycle) {
        order.clear();
        return order;
    }

    std::reverse(order.begin(), order.end());
    return order;
}

std::vector<long long> DAG::longest_path(int source, long long minus_inf) {
    bool has_cycle = false;
    std::vector<int> topo = topological_sort_kahn(has_cycle);
    int n = vertex_count();
    std::vector<long long> dist(n, minus_inf);

    if (has_cycle || source < 0 || source >= n) {
        return dist;
    }

    dist[source] = 0;

    for (int idx = 0; idx < static_cast<int>(topo.size()); idx++) {
        int u = topo[idx];
        if (dist[u] == minus_inf) {
            continue;
        }
        Edge* e = get_edges(u);
        while (e) {
            int v = e->to;
            long long w = e->weight;
            if (dist[v] < dist[u] + w) {
                dist[v] = dist[u] + w;
            }
            e = e->next;
        }
    }

    return dist;
}

std::vector<long long> DAG::shortest_path(int source, long long inf) {
    bool has_cycle = false;
    std::vector<int> topo = topological_sort_kahn(has_cycle);
    int n = vertex_count();
    std::vector<long long> dist(n, inf);

    if (has_cycle || source < 0 || source >= n) {
        return dist;
    }

    dist[source] = 0;

    for (int idx = 0; idx < static_cast<int>(topo.size()); idx++) {
        int u = topo[idx];
        if (dist[u] == inf) {
            continue;
        }
        Edge* e = get_edges(u);
        while (e) {
            int v = e->to;
            long long w = e->weight;
            if (dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
            }
            e = e->next;
        }
    }

    return dist;
}

long long DAG::count_paths(int source, int target) {
    bool has_cycle = false;
    std::vector<int> topo = topological_sort_kahn(has_cycle);
    int n = vertex_count();
    std::vector<long long> ways(n, 0);

    if (has_cycle || source < 0 || source >= n || target < 0 || target >= n) {
        return 0;
    }

    ways[source] = 1;

    for (int idx = 0; idx < static_cast<int>(topo.size()); idx++) {
        int u = topo[idx];
        if (ways[u] == 0) {
            continue;
        }
        Edge* e = get_edges(u);
        while (e) {
            int v = e->to;
            ways[v] += ways[u];
            e = e->next;
        }
    }

    return ways[target];
}

bool DAG::would_create_cycle(int from, int to) const {
    int n = vertex_count();
    if (from < 0 || from >= n || to < 0 || to >= n) {
        throw std::out_of_range("Vertex index out of range in DAG::would_create_cycle");
    }

    std::vector<char> visited(n, 0);
    std::vector<int> stack;
    stack.push_back(to);
    visited[to] = 1;

    while (!stack.empty()) {
        int v = stack.back();
        stack.pop_back();
        if (v == from) {
            return true;
        }
        Edge* e = get_edges(v);
        while (e) {
            int nxt = e->to;
            if (!visited[nxt]) {
                visited[nxt] = 1;
                stack.push_back(nxt);
            }
            e = e->next;
        }
    }

    return false;
}

bool DAG::add_edge_checked(int from, int to) {
    if (would_create_cycle(from, to)) {
        return false;
    }
    add_edge(from, to);
    return true;
}

DAG build_scc_condensation_dag(const Graph& g, const std::vector<int>& component, int component_count) {
    int n = g.vertex_count();
    if (component_count < 0) {
        throw std::invalid_argument("Component count must be non-negative in build_scc_condensation_dag");
    }
    if (n == 0 || component_count == 0) {
        DAG dag(component_count);
        return dag;
    }
    if (static_cast<int>(component.size()) != n) {
        throw std::invalid_argument("Component vector size must match graph vertex count in build_scc_condensation_dag");
    }

    DAG dag(component_count);
    std::vector<std::vector<int>> adjacency(component_count);

    for (int u = 0; u < n; u++) {
        int cu = component[u];
        if (cu < 0 || cu >= component_count) {
            throw std::out_of_range("Component id out of range in build_scc_condensation_dag");
        }
        Edge* e = g.get_edges(u);
        while (e) {
            int v = e->to;
            if (v >= 0 && v < n) {
                int cv = component[v];
                if (cv < 0 || cv >= component_count) {
                    throw std::out_of_range("Component id out of range in build_scc_condensation_dag");
                }
                if (cu != cv) {
                    adjacency[cu].push_back(cv);
                }
            }
            e = e->next;
        }
    }

    for (int u = 0; u < component_count; u++) {
        std::vector<int>& row = adjacency[u];
        if (row.empty()) {
            continue;
        }
        std::sort(row.begin(), row.end());
        row.erase(std::unique(row.begin(), row.end()), row.end());
        for (std::size_t i = 0; i < row.size(); i++) {
            int v = row[i];
            dag.add_edge(u, v);
        }
    }

    return dag;
}

}
