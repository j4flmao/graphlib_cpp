# Centroid Decomposition

## Overview

Centroid Decomposition is a divide-and-conquer technique for trees that recursively partitions a tree by its centroid. A **centroid** of a tree is a node whose removal results in no remaining subtree having more than half the nodes of the original tree.

The decomposition builds a **centroid tree** where:
- The root is the centroid of the original tree
- Each node's children are the centroids of the subtrees formed after removing ancestor centroids

This structure has **O(log N)** depth, enabling efficient solutions for path-related queries.

## Time Complexity

| Operation | Complexity |
|-----------|------------|
| Construction | O(N log N) |
| `get_root()` | O(1) |
| `get_parent()` | O(1) |
| `get_level()` | O(1) |
| `get_tree()` | O(N) |

The O(N log N) construction comes from processing each node O(log N) times (once per level of the centroid tree).

## API Reference

### Constructor

```cpp
explicit CentroidDecomposition(const Graph& graph);
```

Constructs the centroid decomposition of the given tree. The graph must be a connected tree (N-1 edges for N nodes).

### Methods

#### `int get_root() const`

Returns the root of the centroid tree.

#### `int get_parent(int u) const`

Returns the parent of node `u` in the centroid tree. Returns `-1` if `u` is the root.

#### `int get_level(int u) const`

Returns the depth of node `u` in the centroid tree. The root has level `0`.

#### `std::vector<std::vector<int>> get_tree() const`

Returns the full centroid tree as an adjacency list where `tree[u]` contains the children of `u` in the centroid tree.

#### Alias Methods

```cpp
int get_centroid_parent(int u) const;  // Same as get_parent(u)
int get_centroid_root() const;         // Same as get_root()
```

## Usage Examples

### Basic Usage

```cpp
#include <graphlib/centroid_decomposition.h>
#include <graphlib/graph_core.h>
#include <iostream>

int main() {
    graphlib::Graph tree(7, false);  // Undirected tree with 7 nodes
    
    // Build a tree:
    //       0
    //      /|\
    //     1 2 3
    //    /|   |
    //   4 5   6
    tree.add_edge(0, 1);
    tree.add_edge(0, 2);
    tree.add_edge(0, 3);
    tree.add_edge(1, 4);
    tree.add_edge(1, 5);
    tree.add_edge(3, 6);
    
    graphlib::CentroidDecomposition cd(tree);
    
    std::cout << "Centroid tree root: " << cd.get_root() << "\n";
    
    for (int i = 0; i < 7; i++) {
        std::cout << "Node " << i 
                  << " -> parent: " << cd.get_parent(i)
                  << ", level: " << cd.get_level(i) << "\n";
    }
    
    return 0;
}
```

### Traversing the Centroid Tree

```cpp
#include <graphlib/centroid_decomposition.h>
#include <graphlib/graph_core.h>

// Process all ancestors in centroid tree (useful for updates/queries)
void process_ancestors(const graphlib::CentroidDecomposition& cd, int u) {
    int current = u;
    while (current != -1) {
        // Process node 'current'
        // Distance from u to current can be computed separately
        current = cd.get_parent(current);
    }
    // This loop runs O(log N) times due to centroid tree depth
}
```

### Distance Queries Framework

```cpp
#include <graphlib/centroid_decomposition.h>
#include <graphlib/graph_core.h>
#include <vector>
#include <climits>

class DistanceQuery {
    const graphlib::Graph& tree_;
    graphlib::CentroidDecomposition cd_;
    std::vector<std::vector<int>> dist_to_centroid_;  // dist_to_centroid_[level][node]
    
public:
    DistanceQuery(const graphlib::Graph& tree) 
        : tree_(tree), cd_(tree) {
        int n = tree.node_count();
        int max_level = 0;
        for (int i = 0; i < n; i++) {
            max_level = std::max(max_level, cd_.get_level(i));
        }
        
        dist_to_centroid_.resize(max_level + 1, std::vector<int>(n, -1));
        
        // Precompute distances from each node to its centroid ancestors
        // Implementation depends on specific requirements
    }
    
    // Query minimum distance from node u to any marked node
    // Update: mark node u
    // Both operations run in O(log N) using centroid tree traversal
};
```

## Applications

### 1. Distance Queries

Find the minimum/maximum distance from a node to any node in a set. Use the centroid tree to:
- **Update**: When marking a node, traverse up the centroid tree and update distance information at each ancestor
- **Query**: Traverse up the centroid tree and check the best answer from each ancestor's data

Both operations are O(log N) per query.

### 2. Path Counting

Count paths of a specific length or satisfying certain properties:
- At each centroid, count paths passing through it
- Recursively solve for subtrees after removing the centroid
- Each path is counted exactly once (at its highest centroid)

### 3. Finding Pairs at Distance K

Count or enumerate all pairs of nodes at exactly distance K:
```
For each centroid c:
    Count paths of length K passing through c
    Subtract paths entirely within a single subtree (to avoid double counting)
```

### 4. Tree Coloring Queries

Answer queries like "closest node of color X to node U":
- Maintain data structures at each centroid level
- Query by traversing centroid ancestors

### 5. Xor Path Queries

Find paths with specific XOR values:
- Store XOR values from nodes to their centroid ancestors
- Use properties of XOR to combine path segments

## Key Properties

1. **Depth Bound**: The centroid tree has depth O(log N) because each subtree is at most half the size of its parent.

2. **Path Coverage**: Every path in the original tree passes through exactly one LCA in the centroid tree.

3. **Subtree Independence**: After removing a centroid, the remaining subtrees are independent and can be processed separately.

## See Also

- [Graph Core](DOCS_GRAPH_CORE.md) - Basic graph structure
- [Heavy-Light Decomposition](DOCS_HLD.md) - Another tree decomposition technique
