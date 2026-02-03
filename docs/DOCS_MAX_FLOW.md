# GraphLib – Maximum Flow & Min-Cost Max-Flow

This module provides high-performance algorithms for network flow problems, including Dinic's algorithm, Push-Relabel, and Minimum-Cost Maximum-Flow reducers.

---

## 1. Overview

### Headers

```cpp
#include <graphlib/max_flow.h>
```

### Key Classes

| Class | Description |
|-------|-------------|
| `graphlib::MaxFlow` | Unified solver for Max-Flow and Min-Cost Max-Flow problems. |

---

## 2. Usage

### 2.1 Initialization & Graph Construction

```cpp
int num_nodes = 6;
graphlib::MaxFlow mf(num_nodes);

// Add directed edge: u -> v, capacity, [cost]
mf.add_edge(0, 1, 10);      // Capacity 10, Cost 0 (default)
mf.add_edge(1, 2, 5, 3);    // Capacity 5, Cost 3

// Add undirected edge (capacity in both directions)
mf.add_undirected_edge(3, 4, 15);
```

### 2.2 Maximum Flow Algorithms

The library supports multiple algorithms for standard Max-Flow.

#### Dinic's Algorithm (Recommended)
Best general-purpose algorithm. Fast on typical graphs and especially bipartite matching networks.

```cpp
long long flow = mf.dinic(source, sink);
```

**Complexity**: $O(V^2 E)$ generally; $O(E\sqrt{V})$ for unit networks.

#### Push-Relabel (Preflow-Push)
Efficient for dense graphs.

```cpp
long long flow = mf.push_relabel(source, sink);
```

**Complexity**: $O(V^3)$ or $O(V^2 \sqrt{E})$.

#### Edmonds-Karp
Simpler, uses BFS. Useful for small graphs or teaching.

```cpp
long long flow = mf.edmonds_karp(source, sink);
```

**Complexity**: $O(V E^2)$.

---

## 3. Minimum-Cost Maximum-Flow (MCMF)

Computes the maximum flow possible that has the minimal cost. Uses the **Successive Shortest Path** algorithm with potentials (SPFA/Dijkstra) to handle costs correctly.

```cpp
// Returns pair {Total Flow, Total Cost}
auto [flow, cost] = mf.min_cost_max_flow(source, sink);

std::cout << "Max Flow: " << flow << std::endl;
std::cout << "Min Cost: " << cost << std::endl;
```

---

## 4. Advanced Applications

### 4.1 Minimum Cut

After running a max-flow algorithm, you can retrieve the S-T cut.

```cpp
mf.dinic(source, sink); // Run flow first

std::vector<bool> reachable(n);
mf.min_cut_reachable_from_source(source, reachable);

// reachable[v] is true if v is on the Source side of the cut
```

### 4.2 Maximum Weight Closure

Finds a closed set of vertices with maximum total weight in a graph with dependencies.

```cpp
// weights[i] can be positive or negative
// dependencies is vector of pairs {u, v} meaning u implies v
long long closure_weight = mf.maximum_weight_closure(weights, dependencies);
```

### 4.3 Maximum Density Subgraph

Finds the subgraph with the highest ratio of edges to vertices.

```cpp
auto [density, subgraph_nodes] = graphlib::maximum_density_subgraph(graph);
```

### 4.4 Min-Cost Circulation

Finds a flow circulation satisfying demand constraints with minimum cost.

```cpp
// Returns min cost or -1 if infeasible
long long cost = graphlib::min_cost_circulation(n, edges, demands);
```

---

## 5. Summary Table

| Algorithm | Ideal Use Case | Complexity |
|-----------|----------------|------------|
| **Dinic** | General purpose, Bipartite Matching | $O(V^2 E)$ |
| **Push-Relabel** | Dense graphs | $O(V^3)$ |
| **Edmonds-Karp** | Small graphs, educational | $O(VE^2)$ |
| **MCMF** | Flow with edge costs | $O(F \cdot E \log V)$ |
