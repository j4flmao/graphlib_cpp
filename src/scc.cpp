#include "graphlib/scc.h"
#include <algorithm>
#include <stack>
#include <vector>
#include <set>

namespace graphlib {

std::vector<int> strongly_connected_components(const Graph& g, int& count) {
    int n = g.vertex_count();
    std::vector<int> disc(n, -1);
    std::vector<int> low(n, -1);
    std::vector<bool> on_stack(n, false);
    std::stack<int> st;
    std::vector<int> scc(n, -1);
    
    int timer = 0;
    count = 0;
    
    // Tarjan's implementation with explicit recursion stack or just recursion
    // Using recursion for simplicity as N is typically < 10^5 or stack size is sufficient.
    
    // Helper lambda
    struct TarjanSolver {
        const Graph& g;
        std::vector<int>& disc;
        std::vector<int>& low;
        std::vector<bool>& on_stack;
        std::stack<int>& st;
        std::vector<int>& scc;
        int& timer;
        int& count;
        
        void dfs(int u) {
            disc[u] = low[u] = ++timer;
            st.push(u);
            on_stack[u] = true;
            
            for (Edge* e = g.get_edges(u); e != nullptr; e = e->next) {
                int v = e->to;
                if (disc[v] == -1) {
                    dfs(v);
                    low[u] = std::min(low[u], low[v]);
                } else if (on_stack[v]) {
                    low[u] = std::min(low[u], disc[v]);
                }
            }
            
            if (low[u] == disc[u]) {
                while (true) {
                    int v = st.top();
                    st.pop();
                    on_stack[v] = false;
                    scc[v] = count;
                    if (u == v) break;
                }
                count++;
            }
        }
    };
    
    TarjanSolver solver{g, disc, low, on_stack, st, scc, timer, count};
    
    for (int i = 0; i < n; ++i) {
        if (disc[i] == -1) {
            solver.dfs(i);
        }
    }
    
    return scc;
}

Graph condensation_graph(const Graph& g, const std::vector<int>& scc_ids, int scc_count) {
    Graph cg(scc_count, true);
    int n = g.vertex_count();
    
    // Use a set to avoid duplicate edges between components
    std::set<std::pair<int, int>> edges;
    
    for (int u = 0; u < n; ++u) {
        for (Edge* e = g.get_edges(u); e != nullptr; e = e->next) {
            int v = e->to;
            if (scc_ids[u] != scc_ids[v]) {
                if (edges.find({scc_ids[u], scc_ids[v]}) == edges.end()) {
                    edges.insert({scc_ids[u], scc_ids[v]});
                    cg.add_edge(scc_ids[u], scc_ids[v]);
                }
            }
        }
    }
    return cg;
}

}
