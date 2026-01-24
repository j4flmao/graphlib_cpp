#ifndef GRAPHLIB_DIRECTED_MST_H
#define GRAPHLIB_DIRECTED_MST_H

#include "export.h"
#include <vector>

namespace graphlib {

struct GRAPHLIB_API DirectedEdge {
    int u, v;
    long long weight;
    int id; // For reconstructing the solution
};

// Computes the Minimum Spanning Arborescence (Directed MST) rooted at 'root'.
// Returns the total weight of the MST.
// If the root cannot reach all vertices, returns -1.
// 'result_edges' will contain the IDs of edges in the MST.
GRAPHLIB_API long long directed_mst(int n, int root, const std::vector<DirectedEdge>& edges, std::vector<int>& result_edges);

}

#endif
