#ifndef GRAPHLIB_H
#define GRAPHLIB_H

#define GRAPHLIB_VERSION_MAJOR 1
#define GRAPHLIB_VERSION_MINOR 0
#define GRAPHLIB_VERSION_PATCH 4

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
#include "graphlib/dominator_tree.h"
#include "graphlib/centroid_decomposition.h"
#include "graphlib/np_hard.h"
#include "graphlib/tree_algo.h"
#include "graphlib/steiner_tree.h"
#include "graphlib/block_cut_tree.h"
#include "graphlib/chordal.h"
#include "graphlib/cycle_algo.h"
#include "graphlib/directed_mst.h"
#include "graphlib/dsu_rollback.h"
#include "graphlib/dynamic_connectivity.h"
#include "graphlib/graph_generator.h"
#include "graphlib/graph_measures.h"
#include "graphlib/isomorphism.h"
#include "graphlib/k_shortest_paths.h"
#include "graphlib/link_cut_tree.h"
#include "graphlib/min_cut.h"
#include "graphlib/planarity.h"
#include "graphlib/tree_decomposition.h"

namespace graphlib {

GRAPHLIB_API const char* get_version();

}

#endif
