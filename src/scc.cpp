#include "graphlib/scc.h"
#include "graphlib/dag.h"
#include <vector>
#include <stdexcept>

namespace graphlib {

SCC::SCC(int n)
    : Graph(n, true) {
}

int SCC::kosaraju(std::vector<int>& component) {
    int n = vertex_count();
    component.assign(n, -1);

    std::vector<bool> visited(n, false);
    std::vector<int> order;
    order.reserve(n);

    std::vector<int> stack;
    stack.reserve(n);

    for (int start = 0; start < n; start++) {
        if (visited[start]) {
            continue;
        }
        stack.clear();
        stack.push_back(start);

        std::vector<int> path;
        path.reserve(n);

        while (!stack.empty()) {
            int v = stack.back();
            if (v >= 0) {
                if (visited[v]) {
                    stack.pop_back();
                    continue;
                }
                visited[v] = true;
                path.push_back(v);

                Edge* e = get_edges(v);
                while (e) {
                    int to = e->to;
                    if (!visited[to]) {
                        stack.push_back(to);
                    }
                    e = e->next;
                }
            } else {
                stack.pop_back();
            }
        }

        for (int i = static_cast<int>(path.size()) - 1; i >= 0; i--) {
            order.push_back(path[i]);
        }
    }

    std::vector<Edge*> rev_adj(vertex_count(), nullptr);

    for (int u = 0; u < n; u++) {
        Edge* e = get_edges(u);
        while (e) {
            int v = e->to;
            Edge* rev_edge = new Edge(u, e->weight);
            rev_edge->next = rev_adj[v];
            rev_adj[v] = rev_edge;
            e = e->next;
        }
    }

    int comp_count = 0;
    std::vector<bool> used(n, false);

    for (int i = static_cast<int>(order.size()) - 1; i >= 0; i--) {
        int v = order[i];
        if (used[v]) {
            continue;
        }

        std::vector<int> stack2;
        stack2.reserve(n);
        stack2.push_back(v);
        used[v] = true;

        while (!stack2.empty()) {
            int u = stack2.back();
            stack2.pop_back();
            component[u] = comp_count;

            Edge* e = rev_adj[u];
            while (e) {
                int to = e->to;
                if (!used[to]) {
                    used[to] = true;
                    stack2.push_back(to);
                }
                e = e->next;
            }
        }

        comp_count++;
    }

    for (int i = 0; i < n; i++) {
        Edge* e = rev_adj[i];
        while (e) {
            Edge* next = e->next;
            delete e;
            e = next;
        }
    }

    return comp_count;
}

DynamicSCC::DynamicSCC(int n)
    : SCC(n),
      dirty_(true),
      component_(),
      comp_count_(0) {
}

void DynamicSCC::add_edge(int from, int to, long long weight) {
    int n = vertex_count();
    if (from < 0 || from >= n || to < 0 || to >= n) {
        throw std::out_of_range("Vertex index out of range in DynamicSCC::add_edge");
    }
    Graph::add_edge(from, to, weight);
    dirty_ = true;
}

void DynamicSCC::ensure_up_to_date() const {
    if (!dirty_) {
        return;
    }
    int n = vertex_count();
    component_.assign(n, -1);
    comp_count_ = const_cast<DynamicSCC*>(this)->tarjan(component_);
    dirty_ = false;
}

int DynamicSCC::component_count() const {
    ensure_up_to_date();
    return comp_count_;
}

const std::vector<int>& DynamicSCC::components() const {
    ensure_up_to_date();
    return component_;
}

int DynamicSCC::component_id(int v) const {
    int n = vertex_count();
    if (v < 0 || v >= n) {
        return -1;
    }
    ensure_up_to_date();
    if (static_cast<int>(component_.size()) != n) {
        return -1;
    }
    return component_[v];
}

bool DynamicSCC::strongly_connected(int u, int v) const {
    int n = vertex_count();
    if (u < 0 || u >= n || v < 0 || v >= n) {
        return false;
    }
    ensure_up_to_date();
    if (static_cast<int>(component_.size()) != n) {
        return false;
    }
    return component_[u] == component_[v];
}

void DynamicSCC::rebuild() const {
    dirty_ = true;
    ensure_up_to_date();
}

int SCC::tarjan(std::vector<int>& component) {
    int n = vertex_count();
    component.assign(n, -1);

    std::vector<int> index(n, -1);
    std::vector<int> lowlink(n, 0);
    std::vector<bool> on_stack(n, false);
    std::vector<int> stack;
    stack.reserve(n);

    int current_index = 0;
    int comp_count = 0;

    // pair<int, int>: {u, stage}. stage=0 (visit), stage=1 (post-visit)
    std::vector<std::pair<int, int>> call_stack;
    call_stack.reserve(n);

    for (int start = 0; start < n; start++) {
        if (index[start] != -1) {
            continue;
        }

        call_stack.clear();
        call_stack.push_back({start, 0});

        while (!call_stack.empty()) {
            std::pair<int, int> top = call_stack.back();
            call_stack.pop_back();
            int v = top.first;
            int stage = top.second;

            if (stage == 0) {
                if (index[v] != -1) {
                    continue; // Already visited
                }

                index[v] = current_index;
                lowlink[v] = current_index;
                current_index++;
                stack.push_back(v);
                on_stack[v] = true;

                call_stack.push_back({v, 1}); // Post-visit marker

                Edge* e = get_edges(v);
                while (e) {
                    int w = e->to;
                    if (index[w] == -1) {
                        call_stack.push_back({w, 0});
                    } else if (on_stack[w]) {
                        if (lowlink[v] > index[w]) {
                            lowlink[v] = index[w];
                        }
                    }
                    e = e->next;
                }
            } else {
                // Post-visit logic
                Edge* e = get_edges(v);
                while (e) {
                    int w = e->to;
                    if (on_stack[w]) {
                        if (lowlink[v] > lowlink[w]) {
                            lowlink[v] = lowlink[w];
                        }
                    }
                    e = e->next;
                }

                if (lowlink[v] == index[v]) {
                    while (true) {
                        int w = stack.back();
                        stack.pop_back();
                        on_stack[w] = false;
                        component[w] = comp_count;
                        if (w == v) {
                            break;
                        }
                    }
                    comp_count++;
                }
            }
        }
    }

    return comp_count;
}

TwoSAT::TwoSAT(int variables)
    : n_(variables),
      implications_() {
    if (n_ <= 0) {
        throw std::invalid_argument("TwoSAT requires positive number of variables");
    }
}

int TwoSAT::variable_count() const {
    return n_;
}

int TwoSAT::encode_literal(int var, bool value) const {
    return 2 * var + (value ? 0 : 1);
}

void TwoSAT::add_implication(int from, int to) {
    Implication imp;
    imp.from = from;
    imp.to = to;
    implications_.push_back(imp);
}

void TwoSAT::add_clause(int x, bool x_is_true, int y, bool y_is_true) {
    if (x < 0 || x >= n_ || y < 0 || y >= n_) {
        throw std::out_of_range("Variable index out of range in TwoSAT::add_clause");
    }
    int a = encode_literal(x, x_is_true);
    int b = encode_literal(y, y_is_true);
    int na = a ^ 1;
    int nb = b ^ 1;
    add_implication(na, b);
    add_implication(nb, a);
}

void TwoSAT::add_unit_clause(int x, bool x_is_true) {
    if (x < 0 || x >= n_) {
        throw std::out_of_range("Variable index out of range in TwoSAT::add_unit_clause");
    }
    int a = encode_literal(x, x_is_true);
    int na = a ^ 1;
    add_implication(na, a);
}

bool TwoSAT::solve(std::vector<bool>& assignment) {
    int vertices = 2 * n_;
    SCC solver(vertices);
    for (std::size_t i = 0; i < implications_.size(); i++) {
        int u = implications_[i].from;
        int v = implications_[i].to;
        if (u < 0 || v < 0 || u >= vertices || v >= vertices) {
            continue;
        }
        solver.add_edge(u, v);
    }

    std::vector<int> component;
    int comp_count = solver.tarjan(component);
    assignment.assign(n_, false);

    if (static_cast<int>(component.size()) != vertices) {
        return false;
    }

    for (int i = 0; i < n_; i++) {
        int a = encode_literal(i, true);
        int b = encode_literal(i, false);
        if (component[a] == component[b]) {
            return false;
        }
    }

    DAG dag = build_scc_condensation_dag(solver, component, comp_count);

    bool has_cycle = false;
    std::vector<int> order = dag.topological_sort_kahn(has_cycle);
    if (has_cycle || static_cast<int>(order.size()) != comp_count) {
        return false;
    }

    std::vector<int> comp_value(comp_count, -1);
    std::vector<int> comp_opposite(comp_count, -1);

    for (int i = 0; i < n_; i++) {
        int a = component[encode_literal(i, true)];
        int b = component[encode_literal(i, false)];
        comp_opposite[a] = b;
        comp_opposite[b] = a;
    }

    for (int idx = static_cast<int>(order.size()) - 1; idx >= 0; idx--) {
        int c = order[idx];
        if (c < 0 || c >= comp_count) {
            continue;
        }
        if (comp_value[c] != -1) {
            continue;
        }
        comp_value[c] = 1;
        int oc = comp_opposite[c];
        if (oc >= 0 && oc < comp_count) {
            comp_value[oc] = 0;
        }
    }

    for (int i = 0; i < n_; i++) {
        int a = component[encode_literal(i, true)];
        if (a < 0 || a >= comp_count) {
            assignment[i] = false;
            continue;
        }
        assignment[i] = comp_value[a] == 1;
    }

    return true;
}

}
