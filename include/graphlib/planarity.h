#ifndef GRAPHLIB_PLANARITY_H
#define GRAPHLIB_PLANARITY_H

#include "export.h"
#include "graphlib.h"

namespace graphlib {

// Checks if the graph is planar.
// Uses a path addition algorithm (similar to Demoucron's or Boyer-Myrvold's simplified).
// For small graphs, it is exact.
GRAPHLIB_API bool is_planar(const Graph& g);

}

#endif
