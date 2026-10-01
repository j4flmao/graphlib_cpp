#ifndef GRAPHLIB_CONNECTIVITY_EXTRA_H
#define GRAPHLIB_CONNECTIVITY_EXTRA_H
#include "export.h"
#include "graph_core.h"
namespace graphlib {
GRAPHLIB_API int edge_connectivity(const Graph& g);
GRAPHLIB_API int vertex_connectivity(const Graph& g);
}
#endif
