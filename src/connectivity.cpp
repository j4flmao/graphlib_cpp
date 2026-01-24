#include "graphlib/connectivity.h"
#include <vector>
#include <algorithm>
#include <numeric>
#include <limits>

namespace graphlib {

namespace {
    void dfs_components(const Graph& g, int u, int cid, std::vector<int>& component, std::vector<bool>& visited) {
        visited[u] = true;
        component[u] = cid;
        Edge* e = g.get_edges(u);
        while (e) {
            if (!visited[e->to]) {
                dfs_components(g, e->to, cid, component, visited);
            }
            e = e->next;
        }
    }

    void dfs_bridges(const Graph& g, int u, int p, std::vector<int>& tin, std::vector<int>& low, int& timer, std::vector<std::pair<int, int>>& result) {
        tin[u] = low[u] = timer++;
        Edge* e = g.get_edges(u);
        while (e) {
            int v = e->to;
            if (v == p) {
                e = e->next;
                continue;
            }
            if (tin[v] != -1) {
                low[u] = std::min(low[u], tin[v]);
            } else {
                dfs_bridges(g, v, u, tin, low, timer, result);
                low[u] = std::min(low[u], low[v]);
                if (low[v] > tin[u]) {
                    result.push_back({u, v});
                }
            }
            e = e->next;
        }
    }

    void dfs_articulation(const Graph& g, int u, int p, std::vector<int>& tin, std::vector<int>& low, int& timer, std::vector<bool>& cut) {
        tin[u] = low[u] = timer++;
        int children = 0;
        Edge* e = g.get_edges(u);
        while (e) {
            int v = e->to;
            if (v == p) {
                e = e->next;
                continue;
            }
            if (tin[v] != -1) {
                low[u] = std::min(low[u], tin[v]);
            } else {
                dfs_articulation(g, v, u, tin, low, timer, cut);
                low[u] = std::min(low[u], low[v]);
                if (low[v] >= tin[u] && p != -1) {
                    cut[u] = true;
                }
                children++;
            }
            e = e->next;
        }
        if (p == -1 && children > 1) {
            cut[u] = true;
        }
    }

    void dfs_biconnected(const Graph& g, int u, int p, std::vector<int>& tin, std::vector<int>& low, int& timer, std::vector<std::pair<int, int>>& st, std::vector<std::vector<int>>& comps) {
        tin[u] = low[u] = timer++;
        int children = 0;
        Edge* e = g.get_edges(u);
        while (e) {
            int v = e->to;
            if (v == p) {
                e = e->next;
                continue;
            }
            if (tin[v] != -1) {
                low[u] = std::min(low[u], tin[v]);
                if (tin[v] < tin[u]) {
                    st.push_back({u, v});
                }
            } else {
                st.push_back({u, v});
                dfs_biconnected(g, v, u, tin, low, timer, st, comps);
                low[u] = std::min(low[u], low[v]);
                if (low[v] >= tin[u]) {
                    std::vector<int> component;
                    while (true) {
                        std::pair<int, int> edge = st.back();
                        st.pop_back();
                        component.push_back(edge.first);
                        component.push_back(edge.second);
                        if (edge.first == u && edge.second == v) break;
                    }
                    // Unique vertices
                    std::sort(component.begin(), component.end());
                    component.erase(std::unique(component.begin(), component.end()), component.end());
                    comps.push_back(component);
                }
                children++;
            }
            e = e->next;
        }
    }
}

Connectivity::Connectivity(int n)
    : Graph(n, false) {
}

int Connectivity::connected_components(std::vector<int>& component) {
    int n = vertex_count();
    component.assign(n, -1);
    std::vector<bool> visited(n, false);
    int cid = 0;

    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            dfs_components(*this, i, cid, component, visited);
            cid++;
        }
    }

    return cid;
}

void Connectivity::bridges(std::vector<std::pair<int, int>>& result) {
    int n = vertex_count();
    result.clear();
    std::vector<int> tin(n, -1);
    std::vector<int> low(n, -1);
    int timer = 0;

    for (int i = 0; i < n; i++) {
        if (tin[i] == -1) {
            dfs_bridges(*this, i, -1, tin, low, timer, result);
        }
    }
}

void Connectivity::articulation_points(std::vector<int>& result) {
    int n = vertex_count();
    result.clear();
    std::vector<int> tin(n, -1);
    std::vector<int> low(n, -1);
    std::vector<bool> cut(n, false);
    int timer = 0;

    for (int i = 0; i < n; i++) {
        if (tin[i] == -1) {
            dfs_articulation(*this, i, -1, tin, low, timer, cut);
        }
    }

    for (int i = 0; i < n; i++) {
        if (cut[i]) {
            result.push_back(i);
        }
    }
}

void Connectivity::biconnected_components(std::vector<std::vector<int>>& components) {
    int n = vertex_count();
    components.clear();
    std::vector<int> tin(n, -1);
    std::vector<int> low(n, -1);
    std::vector<std::pair<int, int>> edge_stack;
    int timer = 0;

    for (int i = 0; i < n; i++) {
        if (tin[i] == -1) {
            dfs_biconnected(*this, i, -1, tin, low, timer, edge_stack, components);
            if (!edge_stack.empty()) {
                std::vector<int> comp_vertices;
                while (!edge_stack.empty()) {
                    std::pair<int, int> e = edge_stack.back();
                    edge_stack.pop_back();
                    comp_vertices.push_back(e.first);
                    comp_vertices.push_back(e.second);
                }
                std::sort(comp_vertices.begin(), comp_vertices.end());
                comp_vertices.erase(std::unique(comp_vertices.begin(), comp_vertices.end()), comp_vertices.end());
                components.push_back(comp_vertices);
            }
        }
    }
}

}

