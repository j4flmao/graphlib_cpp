#ifndef GRAPHLIB_BLOCK_CUT_TREE_H
#define GRAPHLIB_BLOCK_CUT_TREE_H

#include "export.h"
#include "graphlib.h"
#include <vector>
#include <set>

namespace graphlib {

struct BlockCutTreeResult {
    Graph tree;                 // The block-cut tree
    int num_blocks;             // Number of blocks
    int num_articulations;      // Number of articulation points
    std::vector<int> node_type; // 0 for block, 1 for articulation point
    std::vector<int> original_id; // For articulation points, maps to original graph ID
    std::vector<std::vector<int>> block_nodes; // For blocks, list of original vertices in the block
};

/**
 * @brief Finds articulation points (cut vertices) in the graph.
 * @param g The input graph (undirected).
 * @return A vector of articulation point indices.
 */
GRAPHLIB_API std::vector<int> find_articulation_points(const Graph& g);

/**
 * @brief Finds bridges in the graph.
 * @param g The input graph (undirected).
 * @return A vector of pairs representing bridge edges (u, v) with u < v.
 */
GRAPHLIB_API std::vector<std::pair<int, int>> find_bridges(const Graph& g);

/**
 * @brief Finds biconnected components (blocks) of the graph.
 * @param g The input graph (undirected).
 * @return A vector of vectors, where each inner vector contains the vertices of a block.
 *         Note: Vertices (articulation points) can belong to multiple blocks.
 */
GRAPHLIB_API std::vector<std::vector<int>> find_biconnected_components(const Graph& g);

/**
 * @brief Builds the Block-Cut Tree of the graph.
 * 
 * The Block-Cut Tree has two types of nodes:
 * 1. Articulation points from the original graph.
 * 2. Blocks (biconnected components).
 * 
 * An edge exists between an articulation point node and a block node if the
 * articulation point belongs to that block.
 * 
 * @param g The input graph (undirected).
 * @return The BlockCutTreeResult structure containing the tree and mapping info.
 */
GRAPHLIB_API BlockCutTreeResult build_block_cut_tree(const Graph& g);

} // namespace graphlib

#endif // GRAPHLIB_BLOCK_CUT_TREE_H
