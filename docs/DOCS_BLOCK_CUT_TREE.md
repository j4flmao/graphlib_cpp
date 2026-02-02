# Block-Cut Tree & Biconnected Components

## 1. Overview

This module provides algorithms for analyzing graph connectivity structure through:

- **Articulation Points (Cut Vertices)**: Vertices whose removal disconnects the graph.
- **Bridges (Cut Edges)**: Edges whose removal disconnects the graph.
- **Biconnected Components (Blocks)**: Maximal subgraphs with no articulation points.
- **Block-Cut Tree**: A tree decomposition representing the relationship between blocks and articulation points.

### What is a Biconnected Component?

A **biconnected component** (or **block**) is a maximal subgraph such that:

1. It remains connected after removing any single vertex.
2. Between any two vertices, there exist at least two vertex-disjoint paths.

Key properties:

- Every edge belongs to exactly one block.
- Articulation points can belong to multiple blocks.
- A graph with no articulation points is itself biconnected.

### What is a Block-Cut Tree?

The **Block-Cut Tree** is a tree structure that represents the decomposition of a connected graph:

```
Original Graph:                    Block-Cut Tree:

    0---1                              [Block 0]
     \ /                               {0,1,2}
      2 (articulation)                    |
     / \                              [Art. 2]
    3---4                                 |
                                       [Block 1]
                                       {2,3,4}
```

The tree contains two types of nodes:

1. **Block nodes**: Represent biconnected components, containing the vertices of that block.
2. **Articulation nodes**: Represent cut vertices from the original graph.

An edge exists between a block node and an articulation node if the articulation point belongs to that block.

---

## 2. Header

```cpp
#include <graphlib/block_cut_tree.h>
```

---

## 3. API Reference

### 3.1 Data Structures

#### BlockCutTreeResult

```cpp
struct BlockCutTreeResult {
    Graph tree;                              // The block-cut tree
    int num_blocks;                          // Number of block nodes
    int num_articulations;                   // Number of articulation point nodes
    std::vector<int> node_type;              // 0 = block, 1 = articulation point
    std::vector<int> original_id;            // Maps articulation nodes to original vertex IDs
    std::vector<std::vector<int>> block_nodes; // Vertices in each block
};
```

**Tree node layout:**

- Nodes `0` to `num_blocks - 1` are **block nodes**.
- Nodes `num_blocks` to `num_blocks + num_articulations - 1` are **articulation nodes**.

### 3.2 Functions

#### find_articulation_points

```cpp
std::vector<int> find_articulation_points(const Graph& g);
```

Finds all articulation points in an undirected graph.

| Parameter | Description |
|-----------|-------------|
| `g` | Input undirected graph |

| Returns | Description |
|---------|-------------|
| `std::vector<int>` | Indices of articulation points |

**Complexity:** O(V + E)

---

#### find_bridges

```cpp
std::vector<std::pair<int, int>> find_bridges(const Graph& g);
```

Finds all bridges in an undirected graph.

| Parameter | Description |
|-----------|-------------|
| `g` | Input undirected graph |

| Returns | Description |
|---------|-------------|
| `std::vector<std::pair<int, int>>` | Bridge edges as pairs (u, v) with u < v |

**Complexity:** O(V + E)

---

#### find_biconnected_components

```cpp
std::vector<std::vector<int>> find_biconnected_components(const Graph& g);
```

Finds all biconnected components (blocks) of the graph.

| Parameter | Description |
|-----------|-------------|
| `g` | Input undirected graph |

| Returns | Description |
|---------|-------------|
| `std::vector<std::vector<int>>` | List of blocks; each block is a list of vertex indices |

**Note:** Articulation points appear in multiple blocks.

**Complexity:** O(V + E)

---

#### build_block_cut_tree

```cpp
BlockCutTreeResult build_block_cut_tree(const Graph& g);
```

Constructs the Block-Cut Tree of the graph.

| Parameter | Description |
|-----------|-------------|
| `g` | Input undirected graph |

| Returns | Description |
|---------|-------------|
| `BlockCutTreeResult` | The block-cut tree with metadata |

**Complexity:** O(V + E)

---

## 4. Usage Examples

### 4.1 Finding Articulation Points

```cpp
#include <graphlib/graphlib.h>
#include <graphlib/block_cut_tree.h>
#include <iostream>

int main() {
    graphlib::Graph g(7);
    
    // Create a graph with articulation points
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);  // Triangle: 0-1-2
    g.add_edge(2, 3);  // Bridge to next component
    g.add_edge(3, 4);
    g.add_edge(4, 5);
    g.add_edge(5, 3);  // Triangle: 3-4-5
    g.add_edge(5, 6);  // Pendant vertex
    
    auto articulations = graphlib::find_articulation_points(g);
    
    std::cout << "Articulation points: ";
    for (int v : articulations) {
        std::cout << v << " ";
    }
    std::cout << "\n";
    // Output: Articulation points: 2 3 5
    
    return 0;
}
```

### 4.2 Finding Bridges

```cpp
#include <graphlib/graphlib.h>
#include <graphlib/block_cut_tree.h>
#include <iostream>

int main() {
    graphlib::Graph g(6);
    
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);  // Cycle
    g.add_edge(2, 3);  // Bridge
    g.add_edge(3, 4);
    g.add_edge(4, 5);
    g.add_edge(5, 3);  // Cycle
    
    auto bridges = graphlib::find_bridges(g);
    
    std::cout << "Bridges:\n";
    for (const auto& [u, v] : bridges) {
        std::cout << "  " << u << " -- " << v << "\n";
    }
    // Output: Bridges:
    //   2 -- 3
    
    return 0;
}
```

### 4.3 Finding Biconnected Components

```cpp
#include <graphlib/graphlib.h>
#include <graphlib/block_cut_tree.h>
#include <iostream>

int main() {
    graphlib::Graph g(5);
    
    // Two triangles sharing vertex 2
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);
    g.add_edge(2, 3);
    g.add_edge(3, 4);
    g.add_edge(4, 2);
    
    auto blocks = graphlib::find_biconnected_components(g);
    
    std::cout << "Biconnected components:\n";
    for (size_t i = 0; i < blocks.size(); ++i) {
        std::cout << "  Block " << i << ": ";
        for (int v : blocks[i]) {
            std::cout << v << " ";
        }
        std::cout << "\n";
    }
    // Block 0: 0 1 2
    // Block 1: 2 3 4
    
    return 0;
}
```

### 4.4 Building and Using the Block-Cut Tree

```cpp
#include <graphlib/graphlib.h>
#include <graphlib/block_cut_tree.h>
#include <iostream>

int main() {
    graphlib::Graph g(7);
    
    // Build a graph with multiple blocks
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);  // Block 0
    g.add_edge(2, 3);
    g.add_edge(3, 4);
    g.add_edge(4, 2);  // Block 1
    g.add_edge(4, 5);
    g.add_edge(5, 6);
    g.add_edge(6, 4);  // Block 2
    
    auto result = graphlib::build_block_cut_tree(g);
    
    std::cout << "Block-Cut Tree:\n";
    std::cout << "  Blocks: " << result.num_blocks << "\n";
    std::cout << "  Articulation points: " << result.num_articulations << "\n";
    std::cout << "  Tree nodes: " << result.tree.vertex_count() << "\n";
    
    // Print block contents
    for (int i = 0; i < result.num_blocks; ++i) {
        std::cout << "  Block " << i << " vertices: ";
        for (int v : result.block_nodes[i]) {
            std::cout << v << " ";
        }
        std::cout << "\n";
    }
    
    // Print articulation point mappings
    for (int i = 0; i < result.num_articulations; ++i) {
        int tree_node = result.num_blocks + i;
        std::cout << "  Tree node " << tree_node 
                  << " -> original vertex " << result.original_id[tree_node] << "\n";
    }
    
    // Traverse the tree edges
    std::cout << "  Tree edges:\n";
    for (int u = 0; u < result.tree.vertex_count(); ++u) {
        for (const auto& edge : result.tree.neighbors(u)) {
            int v = edge.to;
            if (u < v) {
                std::string u_type = result.node_type[u] == 0 ? "Block" : "Art";
                std::string v_type = result.node_type[v] == 0 ? "Block" : "Art";
                std::cout << "    " << u_type << "(" << u << ") -- " 
                          << v_type << "(" << v << ")\n";
            }
        }
    }
    
    return 0;
}
```

### 4.5 Checking Vertex Connectivity

Use the block-cut tree to determine if two vertices would be disconnected by removing a third:

```cpp
#include <graphlib/graphlib.h>
#include <graphlib/block_cut_tree.h>
#include <queue>
#include <iostream>

// Check if removing vertex 'cut' disconnects 'u' from 'v'
bool disconnects(const graphlib::BlockCutTreeResult& bct,
                 int u, int v, int cut) {
    // Find which tree nodes contain u, v, and cut
    int cut_tree_node = -1;
    for (int i = bct.num_blocks; i < bct.tree.vertex_count(); ++i) {
        if (bct.original_id[i] == cut) {
            cut_tree_node = i;
            break;
        }
    }
    
    if (cut_tree_node == -1) {
        return false;  // Not an articulation point
    }
    
    // Find blocks containing u and v
    int u_block = -1, v_block = -1;
    for (int i = 0; i < bct.num_blocks; ++i) {
        for (int vertex : bct.block_nodes[i]) {
            if (vertex == u) u_block = i;
            if (vertex == v) v_block = i;
        }
    }
    
    if (u_block == v_block) {
        return false;  // Same block, cannot be disconnected by single vertex
    }
    
    // BFS on tree avoiding cut_tree_node
    std::vector<bool> visited(bct.tree.vertex_count(), false);
    std::queue<int> q;
    q.push(u_block);
    visited[u_block] = true;
    visited[cut_tree_node] = true;  // Block this node
    
    while (!q.empty()) {
        int node = q.front();
        q.pop();
        
        if (node == v_block) return false;  // Can reach v without going through cut
        
        for (const auto& edge : bct.tree.neighbors(node)) {
            if (!visited[edge.to]) {
                visited[edge.to] = true;
                q.push(edge.to);
            }
        }
    }
    
    return true;  // Cannot reach v
}

int main() {
    graphlib::Graph g(5);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);
    g.add_edge(2, 3);
    g.add_edge(3, 4);
    g.add_edge(4, 2);
    
    auto bct = graphlib::build_block_cut_tree(g);
    
    std::cout << "Removing 2 disconnects 0 from 3: " 
              << (disconnects(bct, 0, 3, 2) ? "yes" : "no") << "\n";
    // Output: yes (vertex 2 is the only articulation point)
    
    return 0;
}
```

---

## 5. Applications

### 5.1 Network Reliability Analysis

Identify critical points of failure in network infrastructure:

```cpp
auto articulations = graphlib::find_articulation_points(network);
auto bridges = graphlib::find_bridges(network);

// Articulation points = single points of failure (routers, switches)
// Bridges = single links whose failure partitions the network
```

### 5.2 Graph Decomposition for Parallel Processing

Partition a graph into independent biconnected components for parallel algorithms:

```cpp
auto blocks = graphlib::find_biconnected_components(g);

// Process each block independently in parallel
#pragma omp parallel for
for (size_t i = 0; i < blocks.size(); ++i) {
    process_block(blocks[i]);
}
```

### 5.3 Path Queries on Articulation Points

The block-cut tree enables efficient queries about paths that must pass through certain vertices:

- **Counting articulation points on path**: Use LCA on the block-cut tree.
- **Finding unavoidable vertices**: Articulation points between two blocks.

### 5.4 Social Network Analysis

- **Bridges**: Weak ties connecting different communities.
- **Articulation points**: Key individuals bridging groups.
- **Blocks**: Tightly connected communities.

### 5.5 Circuit Design

- Identify critical components whose failure breaks circuit connectivity.
- Find redundant paths for fault-tolerant designs.

### 5.6 Solving 2-Edge-Connected Subgraph Problems

Use bridges to identify where redundant edges are needed:

```cpp
auto bridges = graphlib::find_bridges(g);
// Add edges to eliminate bridges and make graph 2-edge-connected
```

---

## 6. Algorithm Details

### Time Complexity

| Function | Complexity |
|----------|------------|
| `find_articulation_points` | O(V + E) |
| `find_bridges` | O(V + E) |
| `find_biconnected_components` | O(V + E) |
| `build_block_cut_tree` | O(V + E) |

### Space Complexity

| Function | Complexity |
|----------|------------|
| `find_articulation_points` | O(V) |
| `find_bridges` | O(V) |
| `find_biconnected_components` | O(V + E) |
| `build_block_cut_tree` | O(V + E) |

### Implementation Notes

All algorithms use Tarjan's DFS-based approach with:

- Discovery times (`disc`)
- Low-link values (`low`)
- A stack for tracking edges/vertices in the current DFS path

---

## 7. Related Modules

- [Connectivity & SCC](DOCS_CONNECTIVITY_SCC.md) – Connected components and strongly connected components.
- [Tree Algorithms](DOCS_TREE_ALGO.md) – Algorithms on tree structures.
- [Graph Core](DOCS_GRAPH_CORE.md) – Base graph data structures.
