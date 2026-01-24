#ifndef GRAPHLIB_DSU_ROLLBACK_H
#define GRAPHLIB_DSU_ROLLBACK_H

#include "export.h"
#include <vector>
#include <stack>

namespace graphlib {

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

class GRAPHLIB_API DsuRollback {
private:
    std::vector<int> parent_;
    std::vector<int> rank_;
    int components_;
    
    struct State {
        int u;
        int rank_u;
        int v;
        int rank_v;
        bool merged;
    };
    std::stack<State> history_;

public:
    explicit DsuRollback(int n);
    ~DsuRollback();
    
    // Finds representative of set containing x (no path compression)
    int find(int x) const;
    
    // Unites sets containing x and y
    // Returns true if merged, false if already in same set
    bool unite(int x, int y);
    
    // Saves current state and returns snapshot ID
    int snapshot();
    
    // Rolls back to the state after the last operation
    void rollback();
    
    // Rolls back to a specific snapshot
    void rollback_to(int snapshot_id);
    
    // Returns number of connected components
    int component_count() const;
    
    // Check if connected
    bool connected(int x, int y) const;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

} // namespace graphlib

#endif
