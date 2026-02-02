#ifndef GRAPHLIB_SCC_H
#define GRAPHLIB_SCC_H

#include "export.h"
#include "graph_core.h"
#include <vector>

namespace graphlib {

// Computes Strongly Connected Components using Tarjan's Algorithm.
// Returns a vector where result[u] is the Component ID of vertex u.
// Component IDs are in reverse topological order (0 is a sink component in the condensation graph).
// 'count' will be set to the number of components.
GRAPHLIB_API std::vector<int> strongly_connected_components(const Graph& g, int& count);

// Returns the condensation graph where each vertex represents a component.
// Edges exist between components if there is an edge in original graph.
// Duplicate edges are removed.
GRAPHLIB_API Graph condensation_graph(const Graph& g, const std::vector<int>& scc_ids, int scc_count);

}

#endif
