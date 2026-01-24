#ifndef GRAPHLIB_BIPARTITE_H
#define GRAPHLIB_BIPARTITE_H

#include "export.h"
#include "graph_core.h"
#include <vector>
#include <utility>

namespace graphlib {

class GRAPHLIB_API BipartiteGraph : public Graph {
public:
    BipartiteGraph(int n_left, int n_right);

    // Returns true if the graph is bipartite (always true for this class if usage is correct, 
    // but this checks the structural property explicitly).
    bool is_bipartite() const;

    // Solves the assignment problem (min cost perfect matching) or min cost max matching.
    // Returns the minimum cost.
    long long hungarian_min_cost(long long max_cost_limit = -1);

    // Returns the size of the maximum matching.
    int maximum_matching();

private:
    int n_left_;
    int n_right_;
};

// Checks if a graph is bipartite and returns the partition (color 0 or 1 for each vertex).
// Returns empty vector if not bipartite.
GRAPHLIB_API std::vector<int> is_bipartite(const Graph& g);

// Hopcroft-Karp algorithm for Maximum Bipartite Matching.
// Time Complexity: O(E * sqrt(V))
// Returns a vector where result[u] = v means u is matched with v.
// result[u] = -1 if u is unmatched.
// Note: The graph must be bipartite. If not sure, run is_bipartite first.
// The algorithm handles the bipartition internally or can accept it if needed (simplest is just to run on the graph structure).
// However, typically Hopcroft-Karp requires knowing the two sets U and V.
// Our implementation will infer partitions or work on the general graph if it is bipartite.
GRAPHLIB_API std::vector<int> hopcroft_karp_matching(const Graph& g);

// Simple Maximum Bipartite Matching using DFS (Kuhn's Algorithm)
// Simpler to implement but O(VE). Good for small dense graphs or as a baseline.
GRAPHLIB_API std::vector<int> max_bipartite_matching_dfs(const Graph& g);

}

#endif // GRAPHLIB_BIPARTITE_H
