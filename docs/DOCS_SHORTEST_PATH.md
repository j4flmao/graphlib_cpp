# GraphLib – Shortest Paths (`shortest_path.h`)

This document covers shortest path algorithms:

- Dijkstra (single-source, non-negative weights).
- 0–1 BFS (single-source, edges with weights 0 or 1).
- Bellman–Ford (single-source, negative weights allowed).
- Floyd–Warshall (all-pairs).
- Johnson (all-pairs, sparse graphs).
- A* search (single pair with heuristic).
- Multi-source Dijkstra (distances from a set of sources).

---

## 1. Overview

Header:

```cpp
#include <graphlib/shortest_path.h>
```

Main class:

- `graphlib::ShortestPath`.

---

## 2. Building the Graph

```cpp
int n = 5;
graphlib::ShortestPath sp(n);

sp.add_edge(0, 1, 2);
sp.add_edge(1, 2, 3);
sp.add_edge(0, 3, 10);
```

- `ShortestPath(int n)`:
  - Directed weighted graph on vertices `0..n-1`.
- `void add_edge(int from, int to, long long weight)`:
  - Adds a directed edge with the given weight.

Design note:

- `ShortestPath` owns its own adjacency lists separate from `Graph`.
- This allows it to be specialized for shortest-path algorithms without affecting other modules.

---

## 3. Dijkstra – Non-Negative Weights

```cpp
long long INF = 1000000000000LL;
auto dist = sp.dijkstra(0, INF);
```

- `std::vector<long long> dijkstra(int source, long long inf)`:
  - `source`: start vertex.
  - `inf`: value used to represent "infinite" distance.
  - Returns a vector `dist` where `dist[v]` is shortest distance from `source` to `v`.

Requirements:

- All edge weights must be non-negative.

Implementation idea (high-level):

- Uses a priority queue to always expand the currently closest vertex.
- Once a vertex is popped with distance `d`, there is no shorter path to it.

---

## 4. 0–1 BFS – Edges with Weights 0 or 1

```cpp
long long INF = 1000000000000LL;
auto dist01 = sp.zero_one_bfs(0, INF);
```

- `std::vector<long long> zero_one_bfs(int source, long long inf)`:
  - `source`: start vertex.
  - `inf`: value used for unreachable vertices.
  - Assumes every edge weight is either `0` or `1`.

Use this when:

- All edges have weights `0` or `1`.
- You want better performance than Dijkstra on such graphs.

Complexity:

- `O(V + E)` using a deque.

If an edge has weight outside `{0, 1}`, it is ignored by this method.

---

## 5. Bellman–Ford – Negative Weights

```cpp
long long INF = 1000000000000LL;
bool has_negative_cycle = false;
auto dist_bf = sp.bellman_ford(0, INF, has_negative_cycle);
```

- `std::vector<long long> bellman_ford(int source, long long inf, bool& has_negative_cycle)`:
  - Handles negative weights.
  - Detects negative cycles reachable from `source`.

Use this when:

- Some edges can have negative weights.
- You need to know whether a negative cycle exists.

Performance considerations:

- Complexity is `O(V * E)`, slower than Dijkstra on large graphs.
- Use it mainly when negative edges are unavoidable or you need cycle detection.

---

## 6. Floyd–Warshall – All-Pairs

```cpp
long long INF = 1000000000000LL;
auto dist_fw = sp.floyd_warshall(INF);
```

- `std::vector<std::vector<long long>> floyd_warshall(long long inf)`:
  - Returns an `n x n` matrix `dist` where `dist[i][j]` is the shortest distance from `i` to `j`.

Use this for:

- Small or dense graphs.
- When you need distances between all pairs of vertices.

---

## 7. Johnson – All-Pairs on Sparse Graphs

```cpp
long long INF = 1000000000000LL;
bool has_neg_cycle = false;
auto dist_johnson = sp.johnson(INF, has_neg_cycle);
```

- `std::vector<std::vector<long long>> johnson(long long inf, bool& has_negative_cycle)`:
  - Combines Bellman–Ford and Dijkstra.
  - Supports negative weights but no negative cycles.

Use this when:

- Graph is sparse.
- Edge weights may be negative.
- You need all-pairs distances.

---

## 8. Minimum Mean Cycle

In some directed network problems, you need to find the **cycle with the minimum mean weight** (minimum average weight per edge), allowing for negative edge weights.

```cpp
long double mean = 0.0L;
bool has_cycle = false;

mean = sp.minimum_mean_cycle(has_cycle);
```

- `long double minimum_mean_cycle(bool& has_cycle)`:
  - Calculates the minimum mean weight of all simple cycles in the graph.
  - If at least one cycle exists, sets `has_cycle = true` and returns the minimum mean value.
  - If the graph has no cycles (is a DAG), sets `has_cycle = false` and returns `0.0L`.

Note:

- Allows negative edge weights, so this problem often arises when dealing with recurring "profits" or "costs".
- The algorithm is based on Karp's theorem, with complexity approximately `O(V * E)`.

Typical use cases:

- Detecting the "best" or "worst" average cycle after transforming weights.
- Analyzing long-run average costs in Markov models or graph-based DP.

---

## 9. A* Search – Single Pair with Heuristic

```cpp
std::vector<long long> heuristic(n, 0);
long long INF = 1000000000000LL;
auto dist_a_star = sp.a_star(0, 4, heuristic, INF);
```

- `std::vector<long long> a_star(int source, int target, const std::vector<long long>& heuristic, long long inf)`:
  - `source`: start vertex.
  - `target`: goal vertex.
  - `heuristic[v]`: admissible estimate of distance from `v` to `target`.

Use this for:

- Pathfinding where you care about one specific start–goal pair.
- Grid or map routing where you can design a good heuristic (e.g., Euclidean distance).

---

## 10. Multi-Source Dijkstra

Sometimes you need distances from a whole set of starting vertices at once.

```cpp
std::vector<int> sources;
sources.push_back(0);
sources.push_back(3);

long long INF = 1000000000000LL;
auto dist_multi = sp.multi_source_dijkstra(sources, INF);
```

- `std::vector<long long> multi_source_dijkstra(const std::vector<int>& sources, long long inf)`:
  - Treats every valid index in `sources` as a vertex with distance `0`.
  - Returns a single distance array with distance to the nearest source.
  - Invalid source indices are ignored.

Typical use cases:

- Multiple starting positions in grid or graph problems.
- Finding distance to the nearest facility, server, or special node.

---

## 11. K-Shortest Paths

To find not just the shortest path, but the $K$ shortest paths (simple or non-simple, depending on implementation specifics, here typically allowing cycles if standard Yen's or similar, but let's check header: `k_shortest_paths` returns vectors of vertices).

```cpp
int k = 3;
std::vector<std::vector<int>> paths = sp.k_shortest_paths(0, 5, k, INF);
```

- `std::vector<std::vector<int>> k_shortest_paths(int source, int target, int k, long long inf)`:
  - Returns up to `k` shortest paths from `source` to `target`.
  - Paths are sorted by ascending length.
  - Returns empty if no path exists.

Use cases:
- Routing alternatives.
- finding "second best" options.

---

## 12. Utility: Path Reconstruction

In many problems you not only need distances but also the actual path.  
You can reconstruct the path from a `parent` array using:

```cpp
std::vector<int> parent(n, -1);
// ... fill parent[v] during BFS/Dijkstra ...

int source = 0;
int target = 5;
std::vector<int> path = graphlib::reconstruct_path(source, target, parent);
```

- `std::vector<int> reconstruct_path(int source, int target, const std::vector<int>& parent)`:
  - Returns the vertex sequence from `source` to `target`.
  - Returns an empty vector if `target` is unreachable or indices are invalid.

This pairs naturally with algorithms where you maintain `parent[v]` whenever you relax an edge.

---

## 13. Choosing the Right Algorithm

- Only non-negative weights:
  - Edges weights 0 or 1 → `zero_one_bfs`.
  - Single-source, general non-negative → `dijkstra`.
  - All-pairs, small/dense → `floyd_warshall`.
  - All-pairs, large/sparse → `johnson`.
- Multiple sources, non-negative weights:
  - Single run → `multi_source_dijkstra`.
- Negative weights, no negative cycles:
  - Single-source → `bellman_ford`.
  - All-pairs → `johnson`.
- Negative cycles possible:
  - Use `bellman_ford` to detect and handle them explicitly.
