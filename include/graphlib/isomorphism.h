#ifndef GRAPHLIB_ISOMORPHISM_H
#define GRAPHLIB_ISOMORPHISM_H

#include "export.h"
#include "graph_core.h"
#include <vector>
#include <map>

namespace graphlib {

/**
 * @brief Checks if 'pattern' is isomorphic to 'target'.
 * Returns true if an isomorphism exists.
 * Optionally returns the mapping in 'mapping' (pattern_node -> target_node).
 */
GRAPHLIB_API bool is_isomorphic(const Graph& pattern, const Graph& target, std::vector<int>* mapping = nullptr);

/**
 * @brief Checks if 'pattern' is a subgraph of 'target' (subgraph isomorphism).
 * Returns true if a subgraph isomorphism exists.
 * Optionally returns the mapping in 'mapping' (pattern_node -> target_node).
 */
GRAPHLIB_API bool is_subgraph_isomorphic(const Graph& pattern, const Graph& target, std::vector<int>* mapping = nullptr);

/**
 * @brief Finds all occurrences of 'pattern' in 'target'.
 * Returns a list of mappings.
 */
GRAPHLIB_API std::vector<std::vector<int>> find_all_subgraph_isomorphisms(const Graph& pattern, const Graph& target);

}

#endif // GRAPHLIB_ISOMORPHISM_H
