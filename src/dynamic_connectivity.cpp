#include "graphlib/dynamic_connectivity.h"
#include <algorithm>
#include <tuple>

namespace graphlib {

DynamicConnectivity::DynamicConnectivity(int n) 
    : n_(n), dsu_(n), time_steps_(0) {
}

void DynamicConnectivity::add_edge(int u, int v) {
    if (u > v) std::swap(u, v);
    if (edge_active_start_.count({u, v})) return;
    edge_active_start_[{u, v}] = time_steps_;
}

void DynamicConnectivity::remove_edge(int u, int v) {
    if (u > v) std::swap(u, v);
    auto it = edge_active_start_.find({u, v});
    if (it != edge_active_start_.end()) {
        closed_intervals_.emplace_back(u, v, it->second, time_steps_);
        edge_active_start_.erase(it);
    }
}

void DynamicConnectivity::query(int u, int v) {
    if (static_cast<int>(queries_.size()) <= time_steps_) {
        queries_.resize(time_steps_ + 1);
    }
    queries_[time_steps_].push_back({u, v, (int)queries_[time_steps_].size()}); 
    // Wait, the result vector needs to be indexed by total query count?
    // Or just append results in order?
    // "Returns results of queries in order".
    // But 'queries_' is vector of vector.
    // I need to track the global query index if I want to return a flat vector.
    // But I can just return a vector corresponding to each 'query()' call?
    // No, 'query()' is void. 'solve()' returns vector<bool>.
    // So I need to flatten the results.
    // Let's store the 'global query index' if needed, or just iterate time_steps_ in order.
    // The current 'id' is index within the time step.
    time_steps_++;
}

void DynamicConnectivity::add_edge_to_tree(int node, int l, int r, int ql, int qr, int u, int v) {
    if (ql > r || qr < l) return;
    if (ql <= l && r <= qr) {
        tree_[node].push_back({u, v});
        return;
    }
    int mid = l + (r - l) / 2;
    add_edge_to_tree(2 * node, l, mid, ql, qr, u, v);
    add_edge_to_tree(2 * node + 1, mid + 1, r, ql, qr, u, v);
}

void DynamicConnectivity::solve_recursive(int node, int l, int r, std::vector<bool>& results) {
    int snapshot = dsu_.snapshot();
    
    for (const auto& edge : tree_[node]) {
        dsu_.unite(edge.first, edge.second);
    }
    
    if (l == r) {
        if (l < (int)queries_.size()) {
            for (const auto& q : queries_[l]) {
                results.push_back(dsu_.connected(q.u, q.v));
            }
        }
    } else {
        int mid = l + (r - l) / 2;
        solve_recursive(2 * node, l, mid, results);
        solve_recursive(2 * node + 1, mid + 1, r, results);
    }
    
    dsu_.rollback_to(snapshot);
}

std::vector<bool> DynamicConnectivity::solve() {
    // 1. Close all open intervals
    for (const auto& kv : edge_active_start_) {
        closed_intervals_.emplace_back(kv.first.first, kv.first.second, kv.second, time_steps_); // extend to end
    }
    edge_active_start_.clear();
    
    // 2. Build Segment Tree
    if (time_steps_ == 0) return {};
    
    int size = time_steps_;
    tree_.assign(4 * size, {});
    
    for (const auto& interval : closed_intervals_) {
        add_edge_to_tree(1, 0, size - 1, std::get<2>(interval), std::get<3>(interval) - 1, std::get<0>(interval), std::get<1>(interval));
        // Interval is [start, end). The edge is active at time t if start <= t < end.
        // Queries are at time t.
        // So edge covers [start, end-1] in terms of query indices?
        // Wait, my 'query' increments 'time_steps_'.
        // query() at t=0. time_steps_ becomes 1.
        // remove_edge() at t=1.
        // If I added edge at t=0, and removed at t=1.
        // It should be active for query at t=0?
        // Yes.
        // So interval is [start, end-1].
        // If start == end, it was added and removed without a query in between? 
        // If start < end, valid.
        // Note: remove_edge uses current time_steps_ as end.
        // If query() was called, time_steps_ increased.
        // So [start, end-1] is correct.
    }
    
    std::vector<bool> results;
    solve_recursive(1, 0, size - 1, results);
    return results;
}

}
