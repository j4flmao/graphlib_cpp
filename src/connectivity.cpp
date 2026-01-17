#include "graphlib/connectivity.h"
#include <vector>
#include <algorithm>

namespace {

void dfs_components(graphlib::Connectivity& g, int v, int cid, std::vector<int>& comp, std::vector<bool>& visited) {
    visited[v] = true;
    comp[v] = cid;
    graphlib::Edge* e = g.get_edges(v);
    while (e) {
        int to = e->to;
        if (!visited[to]) {
            dfs_components(g, to, cid, comp, visited);
        }
        e = e->next;
    }
}

void dfs_bridges(graphlib::Connectivity& g, int v, int parent, std::vector<int>& tin, std::vector<int>& low, int& timer, std::vector<std::pair<int, int>>& res) {
    tin[v] = timer;
    low[v] = timer;
    timer++;

    graphlib::Edge* e = g.get_edges(v);
    while (e) {
        int to = e->to;
        if (to == parent) {
            e = e->next;
            continue;
        }
        if (tin[to] == -1) {
            dfs_bridges(g, to, v, tin, low, timer, res);
            if (low[to] < low[v]) {
                low[v] = low[to];
            }
            if (low[to] > tin[v]) {
                if (v < to) {
                    res.push_back(std::pair<int, int>(v, to));
                } else {
                    res.push_back(std::pair<int, int>(to, v));
                }
            }
        } else if (tin[to] < low[v]) {
            low[v] = tin[to];
        }
        e = e->next;
    }
}

void dfs_articulation(graphlib::Connectivity& g, int v, int parent, std::vector<int>& tin, std::vector<int>& low, int& timer, std::vector<bool>& cut) {
    tin[v] = timer;
    low[v] = timer;
    timer++;

    int children = 0;

    graphlib::Edge* e = g.get_edges(v);
    while (e) {
        int to = e->to;
        if (to == parent) {
            e = e->next;
            continue;
        }
        if (tin[to] == -1) {
            children++;
            dfs_articulation(g, to, v, tin, low, timer, cut);
            if (low[to] < low[v]) {
                low[v] = low[to];
            }
            if (parent != -1 && low[to] >= tin[v]) {
                cut[v] = true;
            }
        } else if (tin[to] < low[v]) {
            low[v] = tin[to];
        }
        e = e->next;
    }

    if (parent == -1 && children > 1) {
        cut[v] = true;
    }
}

void dfs_biconnected(graphlib::Connectivity& g,
                     int v,
                     int parent,
                     std::vector<int>& tin,
                     std::vector<int>& low,
                     int& timer,
                     std::vector<std::pair<int, int>>& edge_stack,
                     std::vector<std::vector<int>>& components) {
    tin[v] = timer;
    low[v] = timer;
    timer++;

    graphlib::Edge* e = g.get_edges(v);
    while (e) {
        int to = e->to;
        if (to == parent) {
            e = e->next;
            continue;
        }
        if (tin[to] == -1) {
            edge_stack.push_back(std::pair<int, int>(v, to));
            dfs_biconnected(g, to, v, tin, low, timer, edge_stack, components);
            if (low[to] < low[v]) {
                low[v] = low[to];
            }
            if (low[to] >= tin[v]) {
                std::vector<int> comp_vertices;
                while (!edge_stack.empty()) {
                    std::pair<int, int> e2 = edge_stack.back();
                    edge_stack.pop_back();
                    comp_vertices.push_back(e2.first);
                    comp_vertices.push_back(e2.second);
                    if ((e2.first == v && e2.second == to) || (e2.first == to && e2.second == v)) {
                        break;
                    }
                }
                std::sort(comp_vertices.begin(), comp_vertices.end());
                comp_vertices.erase(std::unique(comp_vertices.begin(), comp_vertices.end()), comp_vertices.end());
                components.push_back(comp_vertices);
            }
        } else if (tin[to] < tin[v]) {
            edge_stack.push_back(std::pair<int, int>(v, to));
            if (tin[to] < low[v]) {
                low[v] = tin[to];
            }
        }
        e = e->next;
    }
}

}

namespace graphlib {

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

