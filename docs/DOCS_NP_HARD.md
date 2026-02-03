# GraphLib – NP-Hard Problems

This module provides solvers for computationally hard problems, utilizing both exact exponential algorithms for small inputs and polynomial-time approximations for larger datasets.

---

## 1. Overview

### Headers

```cpp
#include <graphlib/np_hard.h>
```

### Problem scope

- **TSP**: Traveling Salesperson Problem (Exact & Approx).
- **Max Clique / Independent Set**: Maximum subgraph problems.
- **Graph Coloring**: Vertex coloring optimization.
- **Cover & Cut**: Vertex Cover, Max Cut, Feedback Vertex Set.

---

## 2. Traveling Salesperson Problem (TSP)

Finds the minimum cost Hamiltonian Cycle visiting every city exactly once.

### 2.1 Exact Solution (Bitmask DP)
Solves instances up to $N \approx 20$ optimally using dynamic programming.

```cpp
long long cost = graphlib::tsp_bitmask(g);
```
**Complexity**: $O(N^2 2^N)$

### 2.2 Christofides Algorithm (Metric TSP)
Best standard approximation for metric graphs.

- **Guarantee**: Within 1.5x of optimal.
- **Requirement**: Graph must satisfy triangle inequality.

```cpp
// Returns {cost, path}
auto [cost, path] = graphlib::tsp_christofides(g);
```
**Complexity**: $O(N^3)$ (dominated by Matching)

### 2.3 MST Approximation (Metric TSP)
Faster but less accurate than Christofides.

- **Guarantee**: Within 2x of optimal.

```cpp
auto [cost, path] = graphlib::tsp_metric_approx(g);
```

---

## 3. Clique & Independent Set

### 3.1 Maximum Clique
Finds the largest subset of vertices where every pair is adjacent. Uses the **Bron-Kerbosch** algorithm with pivoting.

```cpp
std::vector<int> clique = graphlib::max_clique(g);
```
**Complexity**: Exponential worst-case ($O(3^{N/3})$).

### 3.2 Maximum Independent Set
Finds the largest subset of vertices with NO edges between them. Equivalent to Max Clique on the complement graph.

```cpp
int size = graphlib::max_independent_set(g);
```

---

## 4. Graph Coloring

Assigns colors to vertices such that no adjacent vertices share the same color. Goal: Minimize $k$ (chromatic number).

### 4.1 Greedy Coloring (Welsh-Powell)
Heuristic that colors high-degree nodes first. Fast but not always optimal.

```cpp
// Returns mapping: vertex -> color_id
std::vector<int> colors = graphlib::greedy_coloring(g);
int chromatic_num = graphlib::count_colors(colors);
```

### 4.2 Exact Coloring (DSATUR)
"Degree of Saturation" algorithm. More accurate than simple greedy.

```cpp
std::vector<int> colors = graphlib::dsatur_coloring(g);
```

---

## 5. Other Solvers

### 5.1 Chinese Postman Problem
Finds a minimum cost closed walk traversing every **edge** at least once.

```cpp
long long cost = graphlib::chinese_postman(g);
```
**Complexity**: Polynomial (uses Matching + Eulerian path).
Note: Currently supports graphs requiring only even-degree augmentation.

### 5.2 Maximum Cut Approximation
Finds a partition of vertices $(S, V \setminus S)$ maximizing the weight of edges crossing the cut.

```cpp
// 0.5-approximation using local search
auto [cut_weight, partition] = graphlib::max_cut_approx(g);
```

### 5.3 Feedback Vertex Set
Finds a small set of vertices whose removal makes the graph acyclic (a forest).

```cpp
std::vector<int> fvs = graphlib::feedback_vertex_set_approx(g);
```

---

## 6. Summary Table

| Problem | Algorithm | Type | Limit/Approx |
|---------|-----------|------|--------------|
| **TSP** | Bitmask DP | Exact | $N \le 20$ |
| **TSP** | Christofides | Approx | 1.5x (Metric) |
| **Max Clique** | Bron-Kerbosch | Exact | $N \le 60-80$ |
| **Coloring** | DSATUR / Greedy | Heuristic | Fast |
| **Max Cut** | Local Search | Approx | 0.5x |
| **Set Cover** | Greedy | Approx | $\ln N$ |
