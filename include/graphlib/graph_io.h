#ifndef GRAPHLIB_GRAPH_IO_H
#define GRAPHLIB_GRAPH_IO_H

#include "export.h"
#include "graph_core.h"
#include <string>
#include <vector>

namespace graphlib {

/**
 * @brief Saves the graph to a file in Graphviz DOT format.
 * @param g The graph to save.
 * @param filename The output filename.
 * @param directed If false, uses "graph" and "--" (undirected), else "digraph" and "->" (directed).
 *                 If not specified, tries to infer from Graph::is_directed().
 * @return True on success, false on failure.
 */
GRAPHLIB_API bool save_to_dot(const Graph& g, const std::string& filename);

/**
 * @brief Saves the graph to a DOT file with custom vertex labels.
 * @param g The graph.
 * @param filename The output filename.
 * @param labels Vector of labels for vertices 0..n-1.
 * @return True on success.
 */
GRAPHLIB_API bool save_to_dot_labeled(const Graph& g, const std::string& filename, const std::vector<std::string>& labels);

}

#endif
