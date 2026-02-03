# GraphLib – Shortest Paths

This module provides a comprehensive suite of shortest path algorithms, ranging from standard single-source solvers to all-pairs computations and specialized K-shortest path variants.

---

## 1. Overview

### Headers

```cpp
#include <graphlib/shortest_path.h>
```

### Key Classes

| Class | Description |
|-------|-------------|
| `graphlib::ShortestPath` | Core engine for all shortest path algorithms (Dijkstra, Bellman-Ford, etc.). |

---

## 2. Initialization

The `ShortestPath` class maintains its own internal graph representation optimized for pathfinding.

```cpp
// Initialize shortset path solver for 5 vertices
graphlib::ShortestPath sp(5);

// add_edge(u, v, weight)
sp.add_edge(0, 1, 2);
sp.add_edge(1, 2, 3);
sp.add_edge(0, 3, 10);
```

---

## 3. Single-Source Shortest Paths (SSSP)

### 3.1 Dijkstra's Algorithm (Non-negative weights)

The gold standard for graphs with non-negative edge weights.

```cpp
long long INF = 1e18;
// Compute shortest paths from source 0
std::vector<long long> dist = sp.dijkstra(0, INF);

if (dist[2] != INF) {
    std::cout << "Distance to 2: " << dist[2] << std::endl;
}
```

**Complexity**: $O(E \log V)$ or $O(E + V \log V)$ depending on PQ.

### 3.2 0–1 BFS (Weights 0 or 1)

Optimized specific case where edges have weights 0 or 1.

```cpp
auto dist = sp.zero_one_bfs(0, INF);
```

**Complexity**: $O(V + E)$ (using `std::deque`)

### 3.3 Bellman-Ford (General Weights)

Handles negative edge weights and detects negative cycles.

```cpp
bool has_negative_cycle = false;
auto dist = sp.bellman_ford(0, INF, has_negative_cycle);

if (has_negative_cycle) {
    std::cout << "Negative cycle detected!" << std::endl;
}
```

**Complexity**: $O(V \times E)$

### 3.4 Multi-Source Dijkstra

Finds the shortest distance from **any** vertex in a source set to all other nodes. Effectively creates a supersource connected to all seed nodes with weight 0.

```cpp
std::vector<int> sources = {0, 3, 7};
auto dist = sp.multi_source_dijkstra(sources, INF);
```

---

## 4. All-Pairs Shortest Paths (APSP)

### 4.1 Floyd-Warshall

Best for small or dense graphs. Computes distances between every pair of vertices.

```cpp
// Returns n x n matrix of distances
auto dist_matrix = sp.floyd_warshall(INF);
// dist_matrix[i][j] is distance from i to j
```

**Complexity**: $O(V^3)$

### 4.2 Johnson's Algorithm

Best for large, sparse graphs. Can handle negative weights (but no negative cycles). Internally reweights the graph to use Dijkstra.

```cpp
bool has_cycle = false;
auto dist_matrix = sp.johnson(INF, has_cycle);
```

**Complexity**: $O(V \times E \log V)$

---

## 5. Specialized Algorithms

### 5.1 A* Search (Heuristic)

Optimized search for a specific target node using a heuristic function (e.g., Euclidean distance).

```cpp
std::vector<long long> heuristic(5);
// ... fill heuristic table based on geometry ...

auto dist = sp.a_star(0, 4, heuristic, INF);
```

### 5.2 K-Shortest Paths (Yen's Algorithm)

Finds the 1st, 2nd, ..., K-th shortest loopless paths between two nodes.

```cpp
int K = 3;
// Returns vector of paths (where each path is vector<int> of vertices)
auto paths = sp.k_shortest_paths(0, 4, K, INF);

for (const auto& path : paths) {
    // print path vertices
}
```

### 5.3 Minimum Mean Cycle

Finds the cycle in the graph with the minimum average edge weight.

```cpp
bool has_cycle = false;
double min_mean = sp.minimum_mean_cycle(has_cycle);
```

**Applications**: Discrete Event Systems, spectral analysis.

---

## 6. Utility: Path Reconstruction

To get the actual path (sequence of vertices) after running an algorithm:

```cpp
std::vector<int> parent(n, -1);
// Note: Dijkstra/BFS usually need modification to output parent array
// Or you can use specific helper:

int source = 0, target = 4;
auto path = graphlib::reconstruct_path(source, target, parent);
```

---

## 7. Algorithm Selection Guide

| Graph Properties | Recommended Algorithm | Complexity |
|------------------|-----------------------|------------|
| **SSSP**, Weights $\ge 0$ | Dijkstra | $O(E \log V)$ |
| **SSSP**, Weights $\in \{0,1\}$ | 0-1 BFS | $O(V + E)$ |
| **SSSP**, Weights $< 0$ | Bellman-Ford | $O(VE)$ |
| **APSP**, Dense / Small | Floyd-Warshall | $O(V^3)$ |
| **APSP**, Sparse | Johnson | $O(VE \log V)$ |
| **Point-to-Point**, Heuristic | A* | Heuristic-dependent |
| **Top K Paths** | Yen's Algorithm | $O(K \cdot V(E + V \log V))$ |
