# Graph Coloring Algorithms

## Overview

Graph coloring assigns colors (represented as integers) to vertices such that no two adjacent vertices share the same color. This is a fundamental problem in graph theory with applications in:

- **Scheduling**: Assign time slots to tasks where conflicting tasks cannot share a slot
- **Register allocation**: Assign CPU registers to variables in compilers
- **Map coloring**: Color regions so adjacent regions have different colors
- **Frequency assignment**: Assign radio frequencies to avoid interference

The library provides two main functions:
1. **`greedy_coloring`**: Fast heuristic using the DSATUR algorithm
2. **`chromatic_number`**: Exact computation of the minimum colors needed (NP-Hard)

## API Reference

### Header

```cpp
#include "graphlib/coloring.h"
```

### Functions

#### `greedy_coloring`

```cpp
std::vector<int> greedy_coloring(const Graph& g);
```

Colors the graph using the DSATUR (Degree of Saturation) heuristic.

**Parameters:**
- `g` — An undirected graph

**Returns:**
- A vector `colors` where `colors[i]` is the color ID (0-indexed) assigned to vertex `i`

**Complexity:**
- **Time**: O(V² + E) where V is the number of vertices and E is the number of edges
- **Space**: O(V + E) for adjacency color sets

---

#### `chromatic_number`

```cpp
int chromatic_number(const Graph& g);
```

Computes the exact chromatic number (minimum number of colors needed to properly color the graph).

**Parameters:**
- `g` — An undirected graph

**Returns:**
- The chromatic number χ(G)

**Complexity:**
- **Time**: O(k^V) worst case, where k is the chromatic number (exponential)
- **Space**: O(V) for recursion stack and color assignments

> **Warning**: This is an NP-Hard problem. Only use on small graphs (V < 20-30).

---

## Usage Examples

### Basic Graph Coloring

```cpp
#include "graphlib/graph_core.h"
#include "graphlib/coloring.h"
#include <iostream>

int main() {
    // Create a simple triangle graph (requires 3 colors)
    graphlib::Graph g(3, false);  // 3 vertices, undirected
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);

    // Get a valid coloring
    std::vector<int> colors = graphlib::greedy_coloring(g);

    for (int i = 0; i < 3; ++i) {
        std::cout << "Vertex " << i << " -> Color " << colors[i] << std::endl;
    }
    // Output:
    // Vertex 0 -> Color 0
    // Vertex 1 -> Color 1
    // Vertex 2 -> Color 2

    return 0;
}
```

### Finding the Chromatic Number

```cpp
#include "graphlib/graph_core.h"
#include "graphlib/coloring.h"
#include <iostream>

int main() {
    // Create a bipartite graph (chromatic number = 2)
    graphlib::Graph g(4, false);
    g.add_edge(0, 2);
    g.add_edge(0, 3);
    g.add_edge(1, 2);
    g.add_edge(1, 3);

    int chi = graphlib::chromatic_number(g);
    std::cout << "Chromatic number: " << chi << std::endl;
    // Output: Chromatic number: 2

    return 0;
}
```

### Coloring a Petersen Graph

```cpp
#include "graphlib/graph_core.h"
#include "graphlib/coloring.h"
#include <iostream>

int main() {
    // Petersen graph has chromatic number 3
    std::vector<std::pair<int, int>> edges = {
        {0,1}, {1,2}, {2,3}, {3,4}, {4,0},  // Outer pentagon
        {5,7}, {7,9}, {9,6}, {6,8}, {8,5},  // Inner pentagram
        {0,5}, {1,6}, {2,7}, {3,8}, {4,9}   // Connections
    };
    
    graphlib::Graph g = graphlib::make_graph_from_edges(10, edges, false);
    
    std::vector<int> colors = graphlib::greedy_coloring(g);
    int num_colors = *std::max_element(colors.begin(), colors.end()) + 1;
    
    std::cout << "Colors used by DSATUR: " << num_colors << std::endl;
    std::cout << "Exact chromatic number: " << graphlib::chromatic_number(g) << std::endl;
    
    return 0;
}
```

---

## Algorithm Descriptions

### DSATUR (Degree of Saturation)

The `greedy_coloring` function implements the **DSATUR** algorithm, introduced by Daniel Brélaz in 1979.

**Key Concept**: The *saturation degree* of a vertex is the number of distinct colors already assigned to its neighbors.

**Algorithm:**
1. Initialize all vertices as uncolored
2. Compute the degree of each vertex
3. While uncolored vertices remain:
   - Select the uncolored vertex with the **highest saturation degree**
   - Break ties by choosing the vertex with the **highest degree**
   - Assign the **smallest color** not used by any neighbor
   - Update saturation degrees of uncolored neighbors
4. Return the color assignment

**Properties:**
- Produces optimal coloring for bipartite graphs
- Often produces near-optimal results in practice
- Guaranteed to use at most Δ+1 colors (where Δ is the maximum degree)

**Comparison with Welsh-Powell:**

| Algorithm | Selection Criterion | Typical Quality |
|-----------|---------------------|-----------------|
| Welsh-Powell | Highest degree (static) | Good |
| DSATUR | Highest saturation, then degree (dynamic) | Better |

### Chromatic Number (Exact Algorithm)

The `chromatic_number` function uses **backtracking with pruning** to find the exact minimum number of colors.

**Algorithm:**
1. Compute an initial upper bound using `greedy_coloring`
2. Start backtracking from vertex 0 with color 0
3. For each vertex:
   - Try all existing colors (0 to current_max)
   - Try introducing a new color if it could improve the solution
   - Prune branches that cannot beat the current best
4. Return the minimum number of colors found

**Pruning Strategies:**
- If `current_max >= min_colors`, abandon this branch
- Only introduce a new color if `new_color < min_colors`

**When to Use:**
- Small graphs (V < 20-30)
- When exact results are required
- Research and verification purposes

---

## Complexity Summary

| Function | Time Complexity | Space Complexity |
|----------|-----------------|------------------|
| `greedy_coloring` | O(V² + E) | O(V + E) |
| `chromatic_number` | O(k^V) exponential | O(V) |

Where:
- V = number of vertices
- E = number of edges
- k = chromatic number

---

## Special Cases

| Graph Type | Chromatic Number |
|------------|------------------|
| Empty graph (no edges) | 1 |
| Complete graph Kₙ | n |
| Bipartite graph | 2 |
| Odd cycle | 3 |
| Even cycle | 2 |
| Tree | 2 |
| Planar graph | ≤ 4 (Four Color Theorem) |

---

## See Also

- [Graph Core Documentation](DOCS_GRAPH_CORE.md) — Basic graph operations
- [Bipartite Documentation](DOCS_BIPARTITE.md) — Bipartite graph detection (χ = 2)
