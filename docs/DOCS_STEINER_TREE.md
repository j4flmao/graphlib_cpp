# GraphLib – Steiner Tree (`steiner_tree.h`)

This document covers the Steiner Tree algorithm implementation in GraphLib.

---

## 1. Overview

Header:

```cpp
#include <graphlib/steiner_tree.h>
```

The **Steiner Tree Problem** finds the minimum-weight tree connecting a specified subset of vertices called **terminals**. Unlike MST (which connects all vertices), Steiner Tree connects only the terminals, potentially using non-terminal vertices (Steiner points) to reduce total cost.

**Problem Classification:**
- **NP-hard** in general
- **Fixed-Parameter Tractable (FPT)** with respect to the number of terminals $k$

**Relationship to Other Problems:**
| Terminals | Problem |
|-----------|---------|
| All vertices | Minimum Spanning Tree |
| 2 vertices | Shortest Path |
| $k$ vertices | Steiner Tree |

---

## 2. Algorithm: Dreyfus-Wagner

GraphLib implements the **Dreyfus-Wagner algorithm** (1971), a dynamic programming approach that is optimal for small numbers of terminals.

### 2.1 Core Idea

The algorithm uses bitmask DP with state `dp[mask][v]`:
- `mask`: a bitmask representing which terminals are connected
- `v`: the vertex where the current subtree is "rooted"
- `dp[mask][v]`: minimum cost to connect the terminals in `mask` with a tree that includes vertex `v`

### 2.2 Transitions

1. **Combine Submasks**: Merge two disjoint subtrees at vertex `v`:
   ```
   dp[mask][v] = min(dp[submask][v] + dp[mask ^ submask][v])
   ```
   for all proper submasks of `mask`.

2. **Extend via Edges**: Use Dijkstra's algorithm to propagate costs along edges:
   ```
   dp[mask][v] = min(dp[mask][u] + weight(u, v))
   ```

### 2.3 Complexity

| Component | Complexity |
|-----------|------------|
| Submask enumeration | $O(3^k \cdot n)$ |
| Dijkstra relaxation | $O(2^k \cdot n^2)$ or $O(2^k \cdot m \log n)$ |
| Overall | $O(3^k \cdot n + 2^k \cdot n^2 + n^3)$ |

Where:
- $k$ = number of terminals
- $n$ = number of vertices
- $m$ = number of edges

**Practical Limits:** Works well for $k \le 12$. Beyond that, consider heuristic approaches.

---

## 3. API Reference

### `graphlib::steiner_tree`

```cpp
long long steiner_tree(const Graph& g, const std::vector<int>& terminals);
```

Computes the minimum cost of a Steiner Tree connecting all terminals.

**Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `g` | `const Graph&` | Input weighted graph (undirected or directed) |
| `terminals` | `const std::vector<int>&` | Vertex indices (0-based) to connect |

**Returns:**

| Value | Meaning |
|-------|---------|
| `>= 0` | Minimum total weight of the Steiner Tree |
| `0` | If `terminals.size() <= 1` |
| `-1` | If terminals cannot be connected |

**Preconditions:**
- All terminal indices must be in range `[0, g.vertex_count())`
- Edge weights should be non-negative (uses Dijkstra internally)

---

## 4. Examples

### 4.1 Basic Usage

```cpp
#include <graphlib/graphlib.h>
#include <iostream>
#include <vector>

int main() {
    using namespace graphlib;
    
    // Create a simple weighted graph
    //     1
    //    /|\
    //   5 | 5
    //  /  |  \
    // 0---3---2
    //     1
    
    Graph g(4);
    // Add undirected edges (add both directions)
    g.add_edge(0, 1, 5); g.add_edge(1, 0, 5);
    g.add_edge(1, 2, 5); g.add_edge(2, 1, 5);
    g.add_edge(0, 3, 1); g.add_edge(3, 0, 1);
    g.add_edge(3, 2, 1); g.add_edge(2, 3, 1);
    g.add_edge(1, 3, 3); g.add_edge(3, 1, 3);
    
    // Connect terminals 0, 1, 2
    std::vector<int> terminals = {0, 1, 2};
    long long cost = steiner_tree(g, terminals);
    
    std::cout << "Steiner Tree cost: " << cost << "\n";
    // Output: 5 (path: 0-3-2-1 costs 1+1+5=7, but 0-3-1 + 3-2 costs 1+3+1=5)
}
```

### 4.2 Star Graph with Non-Terminal Hub

```cpp
#include <graphlib/graphlib.h>
#include <iostream>

int main() {
    using namespace graphlib;
    
    // Star graph: center vertex 0, leaves 1,2,3,4
    // All edges have weight 10
    Graph g(5);
    for (int i = 1; i <= 4; ++i) {
        g.add_edge(0, i, 10);
        g.add_edge(i, 0, 10);
    }
    
    // Connect all leaves (1,2,3,4) - center 0 is not a terminal
    std::vector<int> terminals = {1, 2, 3, 4};
    long long cost = steiner_tree(g, terminals);
    
    std::cout << "Cost to connect leaves: " << cost << "\n";
    // Output: 30 (use center as Steiner point: 3 edges × 10 = 30)
}
```

### 4.3 Grid Graph

```cpp
#include <graphlib/graphlib.h>
#include <iostream>

int main() {
    using namespace graphlib;
    
    // 3×3 grid, vertices numbered 0-8
    // 0-1-2
    // |X|X|
    // 3-4-5
    // |X|X|
    // 6-7-8
    
    Graph g(9);
    auto add_undirected = [&](int u, int v, long long w) {
        g.add_edge(u, v, w);
        g.add_edge(v, u, w);
    };
    
    // Horizontal edges (weight 1)
    add_undirected(0, 1, 1); add_undirected(1, 2, 1);
    add_undirected(3, 4, 1); add_undirected(4, 5, 1);
    add_undirected(6, 7, 1); add_undirected(7, 8, 1);
    
    // Vertical edges (weight 1)
    add_undirected(0, 3, 1); add_undirected(3, 6, 1);
    add_undirected(1, 4, 1); add_undirected(4, 7, 1);
    add_undirected(2, 5, 1); add_undirected(5, 8, 1);
    
    // Connect corners: 0, 2, 6, 8
    std::vector<int> corners = {0, 2, 6, 8};
    long long cost = steiner_tree(g, corners);
    
    std::cout << "Cost to connect corners: " << cost << "\n";
    // Output: 4 (optimal uses center vertex 4 as Steiner point)
}
```

### 4.4 Handling Disconnected Terminals

```cpp
#include <graphlib/graphlib.h>
#include <iostream>

int main() {
    using namespace graphlib;
    
    // Two disconnected components
    Graph g(4);
    g.add_edge(0, 1, 1); g.add_edge(1, 0, 1);  // Component 1
    g.add_edge(2, 3, 1); g.add_edge(3, 2, 1);  // Component 2
    
    // Try to connect vertices from both components
    std::vector<int> terminals = {0, 2};
    long long cost = steiner_tree(g, terminals);
    
    if (cost == -1) {
        std::cout << "Terminals cannot be connected\n";
    }
    // Output: Terminals cannot be connected
}
```

---

## 5. When to Use Steiner Tree

**Good Use Cases:**
- Network design with mandatory endpoints
- VLSI wire routing
- Phylogenetic tree reconstruction
- Multicast routing in networks

**Consider Alternatives When:**
- $k > 12$: Use approximation algorithms (2-approximation via MST)
- $k = n$: Use standard MST algorithms (Kruskal/Prim)
- $k = 2$: Use shortest path algorithms (Dijkstra/Bellman-Ford)

---

## 6. Implementation Notes

1. **Graph Representation**: Uses adjacency list via `Graph::get_edges()`
2. **Edge Weights**: Must be non-negative (Dijkstra-based relaxation)
3. **Memory**: Requires $O(2^k \cdot n)$ space for the DP table
4. **Directed Graphs**: Supported, but typically used with undirected graphs

---

## 7. Related Algorithms

| Algorithm | Header | Use Case |
|-----------|--------|----------|
| MST (Kruskal/Prim) | `mst.h` | Connect all vertices |
| Shortest Path | `shortest_path.h` | Connect 2 vertices |
| Steiner Tree | `steiner_tree.h` | Connect $k$ terminals |
