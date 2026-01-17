# GraphLib – Connectivity and Strongly Connected Components

This document explains connectivity-related functionality:

- Connected components in undirected graphs.
- Bridges and articulation points.
- Biconnected components in undirected graphs.
- Strongly connected components (SCCs) in directed graphs.

---

## 1. Overview

Headers:

```cpp
#include <graphlib/connectivity.h>
#include <graphlib/scc.h>
#include <graphlib/dag.h>
```

Main classes and helpers:

- `graphlib::Connectivity`.
- `graphlib::SCC`.
- `graphlib::TwoSAT`.
- `graphlib::build_scc_condensation_dag`.

---

## 2. Connectivity (Undirected Graphs)

### 2.1 Building the Graph

```cpp
graphlib::Connectivity conn(5);
conn.add_edge(0, 1);
conn.add_edge(1, 2);
conn.add_edge(3, 4);
```

- `Connectivity(int n)`:
  - Undirected graph with vertices `0..n-1`.

### 2.2 Connected Components

```cpp
std::vector<int> comp(5);
int components = conn.connected_components(comp);
```

- `int connected_components(std::vector<int>& component)`:
  - Returns the number of connected components.
  - Fills `component[v]` with the component id of vertex `v`.

Use this for:

- Partitioning the graph into independent subgraphs.
- Preprocessing before more expensive algorithms.

### 2.3 Bridges and Articulation Points

```cpp
std::vector<std::pair<int, int>> bridges;
conn.bridges(bridges);

std::vector<int> articulation;
conn.articulation_points(articulation);
```

- `void bridges(std::vector<std::pair<int, int>>& result)`:
  - Fills `result` with pairs `(u, v)` representing bridge edges.
- `void articulation_points(std::vector<int>& result)`:
  - Fills `result` with articulation vertices.

Interpretation:

- Bridge: an edge whose removal increases the number of connected components.
- Articulation point: a vertex whose removal increases the number of connected components.

Use cases:

- Network reliability and vulnerability analysis.
- Finding critical connections or routers.

### 2.4 Biconnected Components

```cpp
std::vector<std::vector<int>> bcc;
conn.biconnected_components(bcc);
```

- `void biconnected_components(std::vector<std::vector<int>>& components)`:
  - Decomposes the graph into vertex sets, each representing one biconnected component.
  - Each component is stored as a vector of vertex indices.
  - Articulation points may appear in multiple components.

Interpretation:

- A biconnected component (block) is a maximal subgraph where:
  - Removing any single vertex does not disconnect the component.
  - Equivalently, it has no articulation points inside it.
- Bridges form biconnected components of size 2 (each side of the bridge).

Use cases:

- Block‑cut tree construction.
- Analyzing robust substructures in undirected graphs.

---

## 3. Strongly Connected Components (Directed Graphs)

### 3.1 Building the Graph

```cpp
graphlib::SCC scc(5);
scc.add_edge(0, 1);
scc.add_edge(1, 2);
scc.add_edge(2, 0);
scc.add_edge(3, 4);
```

- `SCC(int n)`:
  - Directed graph with vertices `0..n-1`.

### 3.2 Computing SCCs

```cpp
std::vector<int> comp(5);
int cnt1 = scc.kosaraju(comp);
int cnt2 = scc.tarjan(comp);
```

- `int kosaraju(std::vector<int>& component)`:
  - Kosaraju’s algorithm for SCCs.
- `int tarjan(std::vector<int>& component)`:
  - Tarjan’s algorithm for SCCs.

Both functions:

- Return the number of strongly connected components.
- Fill `component[v]` with the SCC id of vertex `v`.

Use cases:

- Decomposing a directed graph into SCCs.
- Collapsing SCCs into a DAG for higher-level algorithms.

### 3.3 SCC Condensation DAG Helper

After computing SCCs, you can build a condensation DAG where each SCC is a node using the helper:

```cpp
#include <graphlib/scc.h>
#include <graphlib/dag.h>
#include <vector>

int main() {
    graphlib::SCC scc(5);
    scc.add_edge(0, 1);
    scc.add_edge(1, 2);
    scc.add_edge(2, 0);
    scc.add_edge(2, 3);
    scc.add_edge(3, 4);

    std::vector<int> comp(5);
    int scc_count = scc.tarjan(comp);

    graphlib::DAG dag = graphlib::build_scc_condensation_dag(scc, comp, scc_count);

    bool has_cycle = false;
    std::vector<int> topo = dag.topological_sort_kahn(has_cycle);

    return 0;
}
```

- `DAG build_scc_condensation_dag(const Graph& g, const std::vector<int>& component, int component_count)`:
  - `g`: original directed graph (typically `graphlib::SCC` instance).
  - `component`: `component[v]` is the SCC id of vertex `v`.
  - `component_count`: total number of SCCs.
  - Returns a DAG where each vertex is one SCC and edges represent connections between different SCCs.

This pattern is useful when:

- You want to run DAG algorithms on the SCC condensation graph.
- You need to reason about strongly connected regions as single units.

---

## 4. Two-SAT Solver

`graphlib::TwoSAT` solves 2-SAT formulas over `n` boolean variables using SCC on the implication graph.

### 4.1 Basic Usage

Variables are indexed `0..n-1`. Each clause has the form `(x_is_true ? x : ¬x) ∨ (y_is_true ? y : ¬y)`.

```cpp
int n = 3;
graphlib::TwoSAT sat(n);

sat.add_clause(0, true, 1, true);   // (x0 ∨ x1)
sat.add_clause(0, false, 2, true);  // (¬x0 ∨ x2)
sat.add_unit_clause(1, false);      // (¬x1)

std::vector<bool> assignment;
bool ok = sat.solve(assignment);

if (ok) {
    // assignment[i] holds the value of variable i in a satisfying assignment
}
```

- `TwoSAT(int variables)`:
  - Creates a solver for `variables` boolean variables.
- `int variable_count() const`:
  - Returns the number of variables.
- `void add_clause(int x, bool x_is_true, int y, bool y_is_true)`:
  - Adds clause `(x_is_true ? x : ¬x) ∨ (y_is_true ? y : ¬y)`.
- `void add_unit_clause(int x, bool x_is_true)`:
  - Adds unit clause forcing variable `x` to the specified value.
- `bool solve(std::vector<bool>& assignment)`:
  - Returns `true` if the formula is satisfiable.
  - On success, fills `assignment[i]` with the value of variable `i`.

---

## 5. Dynamic SCC

`graphlib::DynamicSCC` maintains strongly connected components of a directed graph under incremental edge insertions.

Internally it reuses the `SCC` Tarjan implementation and caches the last decomposition. Whenever you add new edges, the cache is marked dirty and recomputed on the next query.

### 5.1 Basic Usage

```cpp
#include <graphlib/scc.h>

int n = 4;
graphlib::DynamicSCC ds(n);

ds.add_edge(0, 1);
ds.add_edge(1, 0); // {0,1} becomes one SCC

int comp_count = ds.component_count();      // 3 SCCs: {0,1}, {2}, {3}
bool same = ds.strongly_connected(0, 1);   // true
int cid = ds.component_id(0);              // component id of vertex 0

const std::vector<int>& comp = ds.components();
```

### 5.2 API Summary

- `DynamicSCC(int n)`:
  - Creates a dynamic SCC structure on `n` vertices `0..n-1`.
  - The underlying graph is directed.

- `void add_edge(int from, int to, long long weight = 1)`:
  - Adds a directed edge `from -> to` with optional weight.
  - Marks the internal SCC decomposition as dirty; SCCs are recomputed lazily on the next query.

- `int component_count() const`:
  - Returns the number of strongly connected components.
  - Triggers recomputation if the structure is dirty.

- `const std::vector<int>& components() const`:
  - Returns a reference to the internal component array.
  - After recomputation, `components()[v]` is the component id of vertex `v`.

- `int component_id(int v) const`:
  - Returns the component id of vertex `v`, or `-1` if `v` is out of range.

- `bool strongly_connected(int u, int v) const`:
  - Returns `true` if `u` and `v` are currently in the same component.
  - Returns `false` if indices are out of range.

- `void rebuild() const`:
  - Forces the decomposition to be recomputed immediately.
  - Typically you do not need this, since other queries recompute lazily.

### 5.3 Complexity and Use Cases

- `add_edge` runs in `O(1)` time (amortized, ignoring memory allocation).
- When SCCs are recomputed, Tarjan’s algorithm runs in `O(V + E)`.
- Queries such as `component_count`, `component_id`, `components`, and `strongly_connected` are `O(1)` after the last recomputation.

This design is useful when:

- You add edges over time but only occasionally need the SCC decomposition.
- You want a simple incremental interface without manually re-running SCC algorithms.
