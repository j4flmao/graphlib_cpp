# GraphLib – DAG Algorithms (`dag.h`)

This document covers algorithms specialized for Directed Acyclic Graphs (DAGs):

- Topological sorting.
- Longest and shortest paths.
- Path counting.
- Incremental cycle checks for edge insertions.

---

## 1. Overview

Header:

```cpp
#include <graphlib/dag.h>
```

Main class:

- `graphlib::DAG`.

Assumption:

- The graph should have no directed cycles (a DAG).

---

## 2. Building a DAG

```cpp
graphlib::DAG dag(4);

dag.add_edge(0, 1);
dag.add_edge(1, 2);
dag.add_edge(0, 3);
```

- The underlying structure is similar to `Graph` but optimized for DAG algorithms.

---

## 3. Topological Sort

### 3.1 Kahn’s Algorithm

```cpp
bool has_cycle = false;
auto order = dag.topological_sort_kahn(has_cycle);
```

- `std::vector<int> topological_sort_kahn(bool& has_cycle)`:
  - Returns a topological order if no cycle exists.
  - Sets `has_cycle = true` if a cycle is detected.

### 3.2 DFS-Based Topological Sort

```cpp
bool has_cycle_dfs = false;
auto order_dfs = dag.topological_sort_dfs(has_cycle_dfs);
```

- `std::vector<int> topological_sort_dfs(bool& has_cycle)`:
  - Alternative DFS-based implementation.

Use cases:

- Scheduling tasks with dependencies.
- Determining a valid processing order in pipelines.

---

## 4. Incremental Cycle Checks for New Edges

When you are building a DAG dynamically, you often need to know whether adding a new directed edge would create a cycle.

```cpp
graphlib::DAG dag(3);

dag.add_edge(0, 1);
dag.add_edge(1, 2);

bool c1 = dag.would_create_cycle(0, 2); // false
bool c2 = dag.would_create_cycle(2, 0); // true
```

- `bool would_create_cycle(int from, int to) const`:
  - Returns `true` if adding an edge `from -> to` would introduce a directed cycle.
  - Returns `false` if the graph remains acyclic after such an edge.

For convenience, you can also add edges with a built-in cycle check:

```cpp
bool ok1 = dag.add_edge_checked(0, 2); // edge added, returns true
bool ok2 = dag.add_edge_checked(2, 0); // edge rejected, returns false
```

- `bool add_edge_checked(int from, int to)`:
  - If adding `from -> to` would create a cycle, does nothing and returns `false`.
  - Otherwise adds the edge and returns `true`.

Typical use cases:

- Online construction of dependency graphs where cycles are invalid.
- Interactive tools that must reject invalid user operations that create cycles.
- Maintaining DAG invariants before running topological or path-based algorithms.

---

## 5. Longest and Shortest Paths in DAG

### 4.1 Longest Path

```cpp
long long MINUS_INF = -1000000000000LL;
auto longest = dag.longest_path(0, MINUS_INF);
```

- `std::vector<long long> longest_path(int source, long long minus_inf)`:
  - Computes longest distance from `source` to all vertices.
  - `minus_inf` is a sentinel value for unreachable nodes.

### 4.2 Shortest Path

```cpp
long long INF = 1000000000000LL;
auto shortest = dag.shortest_path(0, INF);
```

- `std::vector<long long> shortest_path(int source, long long inf)`:
  - Computes shortest distance from `source` to all vertices.

Why DAG-specific implementations?

- In a DAG, these algorithms run in linear time `O(V + E)` after topological sorting, which is faster than general shortest path algorithms for the same graph.

Use cases:

- Critical path analysis.
- Project planning where edges represent task durations.

---

## 6. Path Counting

```cpp
long long count = dag.count_paths(0, 3);
```

- `long long count_paths(int source, int target)`:
  - Returns the number of distinct paths from `source` to `target`.

Use this for:

- Counting possible sequences of tasks or states.
- Analysing reachability patterns in DAGs.
