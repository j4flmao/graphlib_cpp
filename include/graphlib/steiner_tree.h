#ifndef GRAPHLIB_STEINER_TREE_H
#define GRAPHLIB_STEINER_TREE_H

#include "export.h"
#include "graph_core.h"
#include <vector>

namespace graphlib {

// Computes the minimum weight of a Steiner Tree connecting the given terminals.
// Uses Dreyfus-Wagner algorithm.
// Complexity: O(3^k * n + 2^k * n^2 + n^3) where k is number of terminals.
// Note: k should be small (e.g., <= 10).
GRAPHLIB_API long long steiner_tree(const Graph& g, const std::vector<int>& terminals);

}

#endif
