#include "graphlib/two_sat.h"
#include "graphlib/scc.h"
#include "graphlib/graph_core.h"
#include <algorithm>

namespace graphlib {

TwoSat::TwoSat(int n) : n_(n) {}

void TwoSat::add_clause(int i, bool val_i, int j, bool val_j) {
    // (x_i == f) is node 2*i
    // (x_i == t) is node 2*i + 1
    
    int u = 2 * i + (val_i ? 1 : 0);
    int v = 2 * j + (val_j ? 1 : 0);
    
    // Clause (u OR v)
    // Equivalent: !u -> v AND !v -> u
    
    auto negate = [](int node) {
        return node ^ 1;
    };
    
    edges_.push_back({negate(u), v});
    edges_.push_back({negate(v), u});
}

void TwoSat::add_implication(int i, bool val_i) {
    // Force x_i == val_i.
    // Equivalent to (x_i == val_i OR x_i == val_i)
    add_clause(i, val_i, i, val_i);
}

bool TwoSat::solve(std::vector<bool>& assignment) {
    int num_nodes = 2 * n_;
    Graph g(num_nodes);
    
    for (const auto& e : edges_) {
        g.add_edge(e.u, e.v);
    }
    
    int count = 0;
    std::vector<int> scc = strongly_connected_components(g, count);
    
    assignment.assign(n_, false);
    
    for (int i = 0; i < n_; ++i) {
        // 2*i: False node
        // 2*i + 1: True node
        if (scc[2 * i] == scc[2 * i + 1]) {
            return false;
        }
        
        // Tarjan SCC produces reverse topological order.
        // i.e. If A -> B, scc[B] < scc[A] (usually popped earlier).
        // Wait, let's verify Tarjan's property.
        // If A -> B in condensation graph, A is processed after B in DFS finish time?
        // Tarjan output ID: usually 0 is the first popped component (sink).
        // So scc[B] < scc[A] means B is a sink, A leads to B.
        // Implication !x -> x.
        // If !x -> x, then we MUST choose x.
        // Means scc[x] < scc[!x] (x is downstream).
        // So if scc[TrueNode] < scc[FalseNode], pick True.
        
        if (scc[2 * i + 1] < scc[2 * i]) {
            assignment[i] = true;
        } else {
            assignment[i] = false;
        }
    }
    
    return true;
}

}
