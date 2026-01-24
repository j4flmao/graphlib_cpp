#ifndef GRAPHLIB_MIN_CUT_H
#define GRAPHLIB_MIN_CUT_H

#include "export.h"
#include "graph_core.h"
#include <vector>

namespace graphlib {

// Implementation of Stoer-Wagner Algorithm for Global Minimum Cut.
// Returns the total weight of the minimum cut in an undirected weighted graph.
// Throws std::invalid_argument if the graph is directed.
GRAPHLIB_API long long global_min_cut_undirected(const Graph& g);

// Computes the Gomory-Hu Tree for an undirected weighted graph.
// The Gomory-Hu tree represents the minimum s-t cuts for all pairs (s, t).
// Throws std::invalid_argument if the graph is directed.
// Output:
//   parent: parent[i] is the parent of vertex i in the Gomory-Hu tree.
//   min_cut: min_cut[i] is the weight of the edge (i, parent[i]) in the Gomory-Hu tree,
//            which corresponds to the minimum cut value between i and parent[i].
GRAPHLIB_API void gomory_hu_tree(const Graph& g, std::vector<int>& parent, std::vector<long long>& min_cut);

} // namespace graphlib

#endif // GRAPHLIB_MIN_CUT_H
