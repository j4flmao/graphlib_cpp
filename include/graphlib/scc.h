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

class GRAPHLIB_API SCC : public Graph {
public:
    explicit SCC(int n);

    int tarjan(std::vector<int>& component) const;
    int kosaraju(std::vector<int>& component) const;
};

class GRAPHLIB_API DynamicSCC : public Graph {
public:
    explicit DynamicSCC(int n);

    void add_edge(int from, int to);
    int component_count() const;
    int component_id(int vertex) const;
    bool strongly_connected(int first, int second) const;

private:
    mutable bool dirty_;
    mutable int component_count_;
    mutable std::vector<int> components_;

    void recompute() const;
};

}

#endif
