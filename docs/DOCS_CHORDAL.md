# Chordal Graph Algorithms

## Overview

A **chordal graph** (also called a triangulated graph) is an undirected graph in which every cycle of length four or more has a *chord* — an edge connecting two non-adjacent vertices in the cycle. Equivalently, chordal graphs contain no induced cycles of length greater than 3.

Chordal graphs are a subclass of **perfect graphs**, meaning:
- The chromatic number equals the size of the maximum clique: `χ(G) = ω(G)`
- Many NP-hard problems on general graphs become polynomial-time solvable

### Key Properties

- Every chordal graph has a **Perfect Elimination Ordering (PEO)**
- Interval graphs are a subset of chordal graphs
- Trees are chordal graphs (no cycles at all)
- Complete graphs are chordal (all possible chords exist)

---

## API Reference

### `is_chordal`

Checks if a graph is chordal using the Maximum Cardinality Search (MCS) algorithm.

```cpp
bool is_chordal(const Graph& g, std::vector<int>* peo = nullptr);
```

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `g` | `const Graph&` | The graph to check |
| `peo` | `std::vector<int>*` | Optional output for Perfect Elimination Ordering |

**Returns:** `true` if the graph is chordal, `false` otherwise.

**PEO Output:** If provided, `peo[i]` gives the vertex at position `i` in the elimination ordering. For any vertex `v = peo[i]`, its neighbors appearing later in the ordering form a clique.

---

### `maximum_clique_chordal`

Finds the maximum clique in a chordal graph.

```cpp
std::vector<int> maximum_clique_chordal(const Graph& g);
```

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `g` | `const Graph&` | The chordal graph |

**Returns:** Vector of vertices forming the maximum clique. Empty if graph is not chordal.

---

### `chromatic_number_chordal`

Computes the chromatic number and optionally a valid coloring.

```cpp
int chromatic_number_chordal(const Graph& g, std::vector<int>* coloring = nullptr);
```

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `g` | `const Graph&` | The chordal graph |
| `coloring` | `std::vector<int>*` | Optional output for vertex colors (0-indexed) |

**Returns:** The chromatic number (minimum colors needed).

---

### `maximum_independent_set_chordal`

Finds the maximum independent set in a chordal graph.

```cpp
std::vector<int> maximum_independent_set_chordal(const Graph& g);
```

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `g` | `const Graph&` | The chordal graph |

**Returns:** Vector of vertices forming the maximum independent set.

---

## Complexity

| Function | Time Complexity | Space Complexity |
|----------|-----------------|------------------|
| `is_chordal` | O(V + E) | O(V) |
| `maximum_clique_chordal` | O(V + E) | O(V) |
| `chromatic_number_chordal` | O(V + E) | O(V) |
| `maximum_independent_set_chordal` | O(V + E) | O(V) |

All algorithms leverage the Perfect Elimination Ordering computed via Maximum Cardinality Search (MCS), which runs in linear time.

---

## Usage Examples

### Basic Chordality Check

```cpp
#include <graphlib/chordal.h>
#include <iostream>

int main() {
    // Create a chordal graph (a 4-clique)
    graphlib::Graph g(4, false);  // 4 vertices, undirected
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(0, 3);
    g.add_edge(1, 2);
    g.add_edge(1, 3);
    g.add_edge(2, 3);

    std::vector<int> peo;
    if (graphlib::is_chordal(g, &peo)) {
        std::cout << "Graph is chordal!\n";
        std::cout << "PEO: ";
        for (int v : peo) std::cout << v << " ";
        std::cout << "\n";
    }
    return 0;
}
```

### Finding Maximum Clique

```cpp
#include <graphlib/chordal.h>
#include <iostream>

int main() {
    graphlib::Graph g(5, false);
    // Build a graph with a triangle (0-1-2) and additional vertices
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(0, 2);
    g.add_edge(2, 3);
    g.add_edge(3, 4);

    auto clique = graphlib::maximum_clique_chordal(g);
    std::cout << "Maximum clique size: " << clique.size() << "\n";
    std::cout << "Vertices: ";
    for (int v : clique) std::cout << v << " ";
    // Output: Vertices: 0 1 2
    return 0;
}
```

### Graph Coloring

```cpp
#include <graphlib/chordal.h>
#include <iostream>

int main() {
    graphlib::Graph g(4, false);
    g.add_edge(0, 1);
    g.add_edge(0, 2);
    g.add_edge(1, 2);
    g.add_edge(2, 3);

    std::vector<int> coloring;
    int chi = graphlib::chromatic_number_chordal(g, &coloring);

    std::cout << "Chromatic number: " << chi << "\n";
    for (int i = 0; i < 4; ++i) {
        std::cout << "Vertex " << i << " -> Color " << coloring[i] << "\n";
    }
    return 0;
}
```

### Maximum Independent Set

```cpp
#include <graphlib/chordal.h>
#include <iostream>

int main() {
    graphlib::Graph g(5, false);
    // Path graph: 0-1-2-3-4 (chordal, since it's a tree)
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(3, 4);

    auto mis = graphlib::maximum_independent_set_chordal(g);
    std::cout << "Maximum independent set size: " << mis.size() << "\n";
    // Output: 3 (e.g., vertices 0, 2, 4)
    return 0;
}
```

---

## Applications

### Interval Scheduling

Interval graphs (graphs where vertices represent intervals and edges connect overlapping intervals) are always chordal. This makes chordal graph algorithms useful for:

- **Resource allocation**: Finding maximum non-overlapping intervals
- **Job scheduling**: Determining minimum resources needed for concurrent jobs
- **Register allocation**: In compilers, live variable intervals form an interval graph

```cpp
// Example: Given intervals, find max non-overlapping set
// Model as interval graph, then use maximum_independent_set_chordal
```

### Sparse Matrix Computation

Chordal graphs arise naturally in sparse matrix factorization:

- **Cholesky decomposition**: The fill-in pattern of sparse Cholesky factorization corresponds to chordal graph completion
- **Minimum fill-in**: Finding optimal variable orderings for Gaussian elimination
- **Parallel computation**: PEO provides an ordering for efficient parallel sparse matrix operations

The elimination tree of a sparse matrix corresponds directly to the structure captured by the Perfect Elimination Ordering.

### Database Query Optimization

- **Acyclic hypergraphs**: Query hypergraphs that are α-acyclic have chordal primal graphs
- **Join ordering**: Optimal join orders for acyclic queries can be found using PEO

### Probabilistic Graphical Models

- **Bayesian networks**: Moralized graphs of Bayesian networks are often made chordal for junction tree inference
- **Junction tree algorithm**: Requires triangulating the moral graph, producing a chordal graph
- **Variable elimination**: Ordering provided by PEO ensures efficient inference

---

## Algorithm Details

### Maximum Cardinality Search (MCS)

The recognition algorithm uses MCS to compute a candidate PEO:

1. Initialize all vertices as unvisited with cardinality 0
2. Repeat for each position (n-1 down to 0):
   - Select the unvisited vertex with maximum cardinality
   - Place it at the current position in PEO
   - Increment cardinality of all its unvisited neighbors
3. Verify the ordering is a valid PEO

### Perfect Elimination Ordering (PEO)

An ordering `v₁, v₂, ..., vₙ` is a PEO if for each `vᵢ`, the set of neighbors of `vᵢ` that appear after it in the ordering forms a clique.

**Property:** A graph is chordal if and only if it has a PEO.

This property enables linear-time algorithms for problems that are NP-hard on general graphs.

---

## See Also

- [Graph Core Documentation](DOCS_CORE.md) - Basic graph operations
- [Tree Algorithms](DOCS_TREE.md) - Specialized algorithms for trees (a subset of chordal graphs)
