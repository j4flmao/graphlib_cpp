# Max Flow & Min-Cost Max-Flow

## Overview
This module provides algorithms for **Maximum Flow** and **Minimum-Cost Maximum-Flow (MCMF)** problems, along with applications like closures and density subgraphs.

## Header
```cpp
#include <graphlib/max_flow.h>
```

## Class `MaxFlow`

### Setup
```cpp
MaxFlow mf(n);
mf.add_edge(u, v, capacity, [cost]); // Add edge
// For undirected flow (capacity in both directions):
mf.add_undirected_edge(u, v, capacity, [cost]); 
```

### Algorithms

1.  **Dinic's Algorithm** (Standard Max Flow)
    ```cpp
    long long max_flow = mf.dinic(source, sink);
    ```
    - Complexity: O(V^2 E) generally, O(E sqrt(V)) for unit networks.
    - Recommended default.

2.  **Edmonds-Karp**
    ```cpp
    long long max_flow = mf.edmonds_karp(source, sink);
    ```
    - Complexity: O(V E^2). Slower, but simpler.

3.  **Push-Relabel**
    ```cpp
    long long max_flow = mf.push_relabel(source, sink);
    ```
    - Complexity: O(V^3) or O(V^2 sqrt(E)). Good for dense graphs.

4.  **Min-Cost Max-Flow**
    ```cpp
    std::pair<long long, long long> result = mf.min_cost_max_flow(source, sink);
    // result.first = Total Flow
    // result.second = Total Cost
    ```
    - Uses **Successive Shortest Path** algorithm with **Dijkstra** and potentials (handles negative costs if no negative cycles).

### Applications

- **Min-Cut**: `mf.min_cut_reachable_from_source(source, reachable_vector)` after running max flow.
- **Maximum Weight Closure**: `maximum_weight_closure(weights, dependencies)`.
- **Maximum Density Subgraph**: `maximum_density_subgraph(graph)`.
- **Min-Cost Circulation**: `min_cost_circulation(n, edges, cost)`.

### Example (MCMF)

```cpp
#include <graphlib/graphlib.h>
#include <graphlib/max_flow.h>
#include <iostream>

int main() {
    graphlib::MaxFlow mf(4);
    // S=0, T=3
    // 0->1 cap 10 cost 2
    // 0->2 cap 10 cost 1
    // 1->3 cap 10 cost 1
    // 2->3 cap 10 cost 5
    
    mf.add_edge(0, 1, 10, 2);
    mf.add_edge(0, 2, 10, 1);
    mf.add_edge(1, 3, 10, 1);
    mf.add_edge(2, 3, 10, 5);
    
    auto [flow, cost] = mf.min_cost_max_flow(0, 3);
    
    // Path 0->1->3: Cost 2+1=3. Cap 10.
    // Path 0->2->3: Cost 1+5=6. Cap 10.
    // If we want max flow (20), we use both.
    // Total cost = 10*3 + 10*6 = 30 + 60 = 90.
    
    std::cout << "Flow: " << flow << ", Cost: " << cost << "\n";
}
```
