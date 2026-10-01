#ifndef GRAPHLIB_WEIGHTED_DSU_ROLLBACK_H
#define GRAPHLIB_WEIGHTED_DSU_ROLLBACK_H
#include "export.h"
#include <vector>
#include <stack>
#include <utility>
namespace graphlib {
// Rollback DSU for constraints value[y] - value[x] = weight.
class GRAPHLIB_API WeightedDsuRollback {
    std::vector<int> parent_, rank_;
    std::vector<long long> delta_;
    struct State { int child, root, root_rank; long long child_delta; bool merged; };
    std::stack<State> history_;
    std::pair<int,long long> root_and_offset(int x) const;
public:
    explicit WeightedDsuRollback(int n);
    bool unite(int x, int y, long long weight);
    int snapshot() const;
    void rollback();
    void rollback_to(int snapshot_id);
    bool connected(int x, int y) const;
    long long difference(int x, int y) const;
};
}
#endif
