#include "graphlib/dsu_rollback.h"
#include <numeric>
#include <algorithm>

namespace graphlib {

DsuRollback::DsuRollback(int n) : components_(n) {
    parent_.resize(n);
    std::iota(parent_.begin(), parent_.end(), 0);
    rank_.assign(n, 0);
}

DsuRollback::~DsuRollback() = default;

int DsuRollback::find(int x) const {
    while (x != parent_[x]) {
        x = parent_[x];
    }
    return x;
}

bool DsuRollback::unite(int x, int y) {
    int rootX = find(x);
    int rootY = find(y);
    
    if (rootX == rootY) {
        history_.push({rootX, rank_[rootX], rootY, rank_[rootY], false});
        return false;
    }
    
    // Union by rank
    if (rank_[rootX] < rank_[rootY]) {
        std::swap(rootX, rootY);
    }
    
    history_.push({rootX, rank_[rootX], rootY, rank_[rootY], true});
    
    parent_[rootY] = rootX;
    if (rank_[rootX] == rank_[rootY]) {
        rank_[rootX]++;
    }
    components_--;
    
    return true;
}

int DsuRollback::snapshot() {
    return static_cast<int>(history_.size());
}

void DsuRollback::rollback() {
    if (history_.empty()) return;
    
    State state = history_.top();
    history_.pop();
    
    if (state.merged) {
        parent_[state.v] = state.v; // Restore parent
        rank_[state.u] = state.rank_u; // Restore rank
        rank_[state.v] = state.rank_v;
        components_++;
    }
}

void DsuRollback::rollback_to(int snapshot_id) {
    while (static_cast<int>(history_.size()) > snapshot_id) {
        rollback();
    }
}

int DsuRollback::component_count() const {
    return components_;
}

bool DsuRollback::connected(int x, int y) const {
    return find(x) == find(y);
}

} // namespace graphlib
