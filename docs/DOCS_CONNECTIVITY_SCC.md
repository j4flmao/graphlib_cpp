# GraphLib – Connectivity & Strongly Connected Components (SCC)

This module provides comprehensive algorithms for analyzing graph connectivity, including finding connected components, strongly connected components, bridges, articulation points, and solving 2-SAT problems.

---

## 1. Overview

### Headers

```cpp
#include <graphlib/connectivity.h>
#include <graphlib/scc.h>
#include <graphlib/dag.h>
```

### Key Classes

| Class | Description |
|-------|-------------|
| `graphlib::Connectivity` | Handles undirected connectivity, bridges, articulation points, and biconnected components. |
| `graphlib::SCC` | Handles directed strongly connected components (Tarjan's, Kosaraju's). |
| `graphlib::DynamicSCC` | Incremental SCC maintenance (recomputes lazily). |
| `graphlib::TwoSAT` | Solves the 2-Satisfiability problem using SCCs. |

---

## 2. Undirected Connectivity

### 2.1 Initialization

```cpp
// Create an undirected graph with 5 vertices
graphlib::Connectivity conn(5);
conn.add_edge(0, 1);
conn.add_edge(1, 2);
conn.add_edge(3, 4);
```

### 2.2 Connected Components

Decomposes the graph into disjoint sets of connected vertices.

```cpp
std::vector<int> comp(5);
int num_components = conn.connected_components(comp);

// comp[v] now contains the component ID for vertex v
// num_components is the total number of unique components
```

**Complexity**: $O(V + E)$

### 2.3 Bridges & Articulation Points

Identifies critical edges and nodes whose removal increases the number of connected components.

```cpp
std::vector<std::pair<int, int>> bridges;
conn.bridges(bridges);
// bridges contains edges (u, v) that are bridges

std::vector<int> cut_vertices;
conn.articulation_points(cut_vertices);
// cut_vertices contains IDs of articulation points
```

**Complexity**: $O(V + E)$ (using DFS tree properties)

### 2.4 Biconnected Components (Block-Cut Tree)

Decomposes the graph into maximal biconnected subgraphs (blocks).

```cpp
std::vector<std::vector<int>> bcc;
conn.biconnected_components(bcc);

// bcc is a list of components, where each component is a list of vertex IDs
```

**Applications**: Network reliability, fault tolerance analysis.

---

## 3. Strongly Connected Components (Directed)

### 3.1 Algorithms

Supports both **Tarjan's** and **Kosaraju's** algorithms.

```cpp
graphlib::SCC scc(5);
scc.add_edge(0, 1);
scc.add_edge(1, 2);
scc.add_edge(2, 0); // 0-1-2 forms a cycle (SCC)
scc.add_edge(2, 3);

std::vector<int> comp(5);
int num_sccs = scc.tarjan(comp); 
// Alternatively: scc.kosaraju(comp);
```

**Complexity**: $O(V + E)$

### 3.2 Condensation Graph (DAG)

Converts the SCCs into a DAG (Directed Acyclic Graph) where each node represents an SCC.

```cpp
// Build the condensation graph
auto dag = graphlib::build_scc_condensation_dag(scc, comp, num_sccs);

// Now you can run DAG algorithms (e.g., topological sort, longest path)
std::vector<int> topo_order = dag.topological_sort();
```

---

## 4. 2-Satisfiability (2-SAT)

Solves boolean logic formulas where each clause has at most 2 literals, e.g., $(x_1 \lor \neg x_2) \land (\neg x_1 \lor x_3)$.

### Example Usage

```cpp
int num_vars = 3;
graphlib::TwoSAT sat(num_vars);

// Add clauses: (x0 OR x1) AND (!x0 OR x2) AND (!x1)
sat.add_clause(0, true, 1, true);   // (x0 v x1)
sat.add_clause(0, false, 2, true);  // (~x0 v x2)
sat.add_unit_clause(1, false);      // (~x1)

std::vector<bool> assignment;
bool solvable = sat.solve(assignment);

if (solvable) {
    // assignment[i] contains the truth value for variable i
}
```

**Complexity**: $O(V + E)$ (linear time)

---

## 5. Dynamic SCC

Maintains SCCs as edges are added. Recomputes lazily on query.

```cpp
graphlib::DynamicSCC ds(4);
ds.add_edge(0, 1);
ds.add_edge(1, 0); 

// Querying triggers recomputation if dirty
if (ds.strongly_connected(0, 1)) {
    // 0 and 1 are in the same SCC
}
```

---

## 6. Performance Notes

- **Memory**: Both connectivity and SCC algorithms use $O(V)$ auxiliary space for DFS stacks and metadata arrays.
- **Recursion**: Algorithms are implemented iteratively or with careful recursion depth management to avoid stack overflow on deep graphs.
- **Large Graphs**: For very large graphs (millions of nodes), consider using the iterative implementations (if available) or ensuring sufficient stack size.
