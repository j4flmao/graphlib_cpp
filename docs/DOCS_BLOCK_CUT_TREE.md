# Block-Cut Tree & Connectivity

## Overview

This module provides algorithms for finding **Articulation Points**, **Bridges**, **Biconnected Components (Blocks)**, and constructing the **Block-Cut Tree**.

The Block-Cut Tree is a tree structure that decomposes a connected graph into its biconnected components. It has two types of nodes: "block" nodes and "cut" (articulation) nodes.

## Header

```cpp
#include <graphlib/block_cut_tree.h>
```

## Functions

### Articulation Points & Bridges

```cpp
std::vector<int> find_articulation_points(const Graph& g);
std::vector<std::pair<int, int>> find_bridges(const Graph& g);
```

### Biconnected Components

```cpp
// Returns list of blocks, where each block is a list of vertex IDs
std::vector<std::vector<int>> find_biconnected_components(const Graph& g);
```

### Block-Cut Tree

```cpp
struct BlockCutTreeResult {
    Graph tree;                 // The tree structure
    int num_blocks;             // Number of block nodes
    int num_articulations;      // Number of articulation nodes
    std::vector<int> node_type; // 0 for block, 1 for articulation
    
    // For articulation nodes, this maps tree_node_id -> original_graph_vertex_id
    std::vector<int> original_id; 
    
    // For block nodes, this contains the list of original vertices
    std::vector<std::vector<int>> block_nodes; 
};

BlockCutTreeResult build_block_cut_tree(const Graph& g);
```

The resulting `tree` has `num_blocks + num_articulations` vertices.
- Vertices `0` to `num_blocks - 1` represent blocks.
- Vertices `num_blocks` to `end` represent articulation points.
An edge exists between a block-node and an articulation-node if the articulation point belongs to that block.

### Example

```cpp
#include <graphlib/graphlib.h>
#include <graphlib/block_cut_tree.h>
#include <iostream>

int main() {
    graphlib::Graph g(5);
    // 0-1-2 and 2-3-4. 2 is cut vertex.
    g.add_edge(0, 1); g.add_edge(1, 2); g.add_edge(2, 0); // Block 1
    g.add_edge(2, 3); g.add_edge(3, 4); g.add_edge(4, 2); // Block 2
    
    auto result = graphlib::build_block_cut_tree(g);
    
    std::cout << "Tree size: " << result.tree.vertex_count() << "\n";
    // Should be 3 nodes: Block1, Block2, and CutVertex(2).
}
```
