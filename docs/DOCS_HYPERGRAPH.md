# GraphLib – Hypergraph Algorithms (`hypergraph.h`)

This document covers the **Hypergraph** module, which allows modeling complex relationships where an edge (hyperedge) can connect any number of vertices.

---

## 1. Overview

Header:

```cpp
#include <graphlib/hypergraph.h>
```

Main class:

- `graphlib::Hypergraph`.

Features:

- Create hypergraphs with arbitrary sets of vertices per edge.
- Compute vertex and edge degrees.
- Incidence list representation.
- Dual Graph transformation (Vertices ↔ Edges).

---

## 2. Basic Usage

### 2.1 Creating a Hypergraph

```cpp
graphlib::Hypergraph h(5); // 5 vertices: 0, 1, 2, 3, 4

// Add a hyperedge connecting {0, 1, 2} with weight 1.0
int e1 = h.add_hyperedge({0, 1, 2});

// Add a hyperedge connecting {2, 3} with weight 5.0
int e2 = h.add_hyperedge({2, 3}, 5.0);

// Add a hyperedge connecting {4} (unary edge)
int e3 = h.add_hyperedge({4});
```

### 2.2 Querying

```cpp
int v_deg = h.vertex_degree(2); // Returns 2 (connected to e1 and e2)
int e_size = h.edge_size(e1);   // Returns 3 ({0, 1, 2})

// Get all edges containing vertex 2
const auto& incident = h.get_incident_edges(2); // {e1, e2}

// Get all vertices in edge e1
const auto& nodes = h.get_edge_nodes(e1); // {0, 1, 2}
```

---

## 3. Dual Hypergraph

The dual of a hypergraph $H = (V, E)$ is a hypergraph $H^* = (E, V)$ such that the vertices of $H^*$ correspond to the hyperedges of $H$, and the hyperedges of $H^*$ correspond to the vertices of $H$.

```cpp
graphlib::Hypergraph dual_h = h.get_dual();

// dual_h has 'edge_count()' vertices and 'vertex_count()' edges.
```

- If vertex $v$ was contained in edges $e_1, e_2$ in $H$, then in $H^*$, the hyperedge corresponding to $v$ connects vertices $e_1$ and $e_2$.

Applications:
- Switching perspectives (e.g., in database schema design, or converting Authors-Papers to Papers-Authors).

---

## 4. Algorithms

### 4.1 Minimal Transversal (Hitting Set)

A **transversal** (or hitting set) is a subset of vertices $T \subseteq V$ such that every hyperedge contains at least one vertex from $T$. Finding the minimum transversal is NP-Hard (equivalent to the Set Cover problem).

```cpp
std::vector<int> t = h.greedy_transversal();
```

- **Algorithm**: Greedy heuristic. Repeatedly selects the vertex that covers the highest number of currently uncovered edges.
- **Complexity**: $O(V \cdot \sum|E_i|)$.

### 4.2 Maximum Matching

A **matching** in a hypergraph is a set of disjoint hyperedges (no two edges share a vertex). Finding the maximum matching is NP-Hard.

```cpp
std::vector<int> m = h.greedy_matching();
```

- **Algorithm**: Greedy heuristic. Sorts edges by size (accumulating small edges first) and picks them if disjoint from selected edges.
- **Return**: A list of Edge IDs.

### 4.3 2-Coloring (Property B)

A hypergraph is **2-colorable** (has Property B) if vertices can be colored with 2 colors such that no edge is monochromatic (contains vertices of only one color).

```cpp
std::vector<int> colors;
bool is_bipartite_like = h.is_2_colorable(colors);
```

- **Algorithm**: Recursive backtracking.
- **Complexity**: Exponential $O(2^n)$. Feasible for small hypergraphs ($n < 30$).
