#ifndef GRAPHLIB_DYNAMIC_CONNECTIVITY_H
#define GRAPHLIB_DYNAMIC_CONNECTIVITY_H

#include "export.h"
#include "dsu_rollback.h"
#include <vector>
#include <map>
#include <utility>
#include <tuple>

namespace graphlib {

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

class GRAPHLIB_API DynamicConnectivity {
private:
    int n_;
    DsuRollback dsu_;
    
    struct Query {
        int u, v;
        int id; // Original query index
    };
    
    // Map from edge (min(u,v), max(u,v)) to start time
    std::map<std::pair<int, int>, int> edge_active_start_;
    
    // Closed intervals: u, v, start_time, end_time
    std::vector<std::tuple<int, int, int, int>> closed_intervals_;

    // Segment tree: tree_[node] contains list of edges active in that interval
    std::vector<std::vector<std::pair<int, int>>> tree_;
    
    // Queries at each time step
    std::vector<std::vector<Query>> queries_;
    
    int time_steps_;
    
    void add_edge_to_tree(int node, int l, int r, int ql, int qr, int u, int v);
    void solve_recursive(int node, int l, int r, std::vector<bool>& results);

public:
    explicit DynamicConnectivity(int n);
    
    // Add edge between u and v
    void add_edge(int u, int v);
    
    // Remove edge between u and v
    void remove_edge(int u, int v);
    
    // Query if u and v are connected
    void query(int u, int v);
    
    // Solve all queries
    std::vector<bool> solve();
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

}

#endif
