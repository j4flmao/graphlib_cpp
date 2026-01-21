#ifndef GRAPHLIB_H
#define GRAPHLIB_H

#define GRAPHLIB_VERSION_MAJOR 1
#define GRAPHLIB_VERSION_MINOR 0
#define GRAPHLIB_VERSION_PATCH 2

#include "graphlib/export.h"
#include "graphlib/graph_core.h"
#include "graphlib/max_flow.h"
#include "graphlib/bipartite.h"
#include "graphlib/dag.h"
#include "graphlib/shortest_path.h"
#include "graphlib/mst.h"
#include "graphlib/connectivity.h"
#include "graphlib/scc.h"
#include "graphlib/general_matching.h"
#include "graphlib/splay_tree.h"
#include "graphlib/tree.h"

namespace graphlib {

GRAPHLIB_API const char* get_version();

}

#endif
