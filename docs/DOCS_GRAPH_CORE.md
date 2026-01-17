# GraphLib – Core Graph Structure (`graph_core.h`)

This document explains the core `Graph` structure: what it represents, how it stores edges, and how to use its helper functions.

---

## 1. Overview

- Adjacency-list representation of a graph.
- Supports directed or undirected edges with weights.
- Used internally by several other modules (DAG, connectivity, SCC, matching).

Header:

```cpp
#include <graphlib/graph_core.h>
```

Namespace:

```cpp
using namespace graphlib;
```

---

## 2. Graph Class

### 2.1 Construction

```cpp
Graph g(5, true);
```

- `Graph(int n, bool directed = true)`:
  - `n`: number of vertices, indexed `0..n-1`.
  - `directed = true`: edges are stored as directed arcs.
  - `directed = false`: edges are stored as undirected (two arcs).

### 2.2 Adding Edges

```cpp
g.add_edge(0, 1, 3);
g.add_edge(1, 2, 7);
```

- `void add_edge(int from, int to, long long weight = 1)`:
  - Creates an edge from `from` to `to` with the given weight.
  - In undirected graphs, this adds symmetric edges.

Typical use:

- Represent a general graph before running custom traversals (BFS/DFS).
- Prepare input for algorithms that only need adjacency information.

### 2.3 Basic Queries

```cpp
int n = g.vertex_count();
bool directed = g.is_directed();
```

- `int vertex_count() const`: number of vertices.
- `bool is_directed() const`: whether the graph is directed.

To iterate all outgoing edges of a vertex, you can use `get_edges` and the internal `Edge` structure:

```cpp
for (Edge* e = g.get_edges(u); e != nullptr; e = e->next) {
    int v = e->to;
    long long w = e->weight;
    // process edge (u -> v) with weight w
}
```

---

## 3. Helper Functions

### 3.1 Complete Graph

```cpp
Graph complete = make_complete_graph(4, false);
```

- `Graph make_complete_graph(int n, bool directed = false)`:
  - Returns a new graph with edges between all pairs of vertices.
  - For `directed = false`, you get `K_n` as an undirected graph.

Use this for:

- Testing algorithms on dense graphs.
- Synthetic benchmarks.

### 3.2 Random Graph

```cpp
Graph random_g = make_random_graph(10, 20, true, 1, 10, 42);
```

- `Graph make_random_graph(int n, int m, bool directed, long long min_weight, long long max_weight, unsigned int seed)`:
  - Creates a graph with `n` vertices and `m` edges.
  - Edge endpoints are chosen pseudo-randomly.
  - Weights are uniformly sampled from `[min_weight, max_weight]`.
  - `seed` makes the graph generation deterministic.

Use this for:

- Randomized testing.
- Quick experiments with algorithm performance.

### 3.3 Connectivity and Tree Checks

```cpp
bool connected = is_connected(complete);
bool tree = is_tree(complete);
```

- `bool is_connected(const Graph& g)`:
  - Returns `true` if the undirected graph is connected.
- `bool is_tree(const Graph& g)`:
  - Returns `true` if the graph is a tree:
    - Connected.
    - Has exactly `n - 1` edges.

Use cases:

- Validating input before applying tree-based algorithms.
- Quick sanity checks in tests and examples.

### 3.4 Build Graphs from Edge Lists

If you already have edges stored in standard containers (common in competitive programming), you can build graphs directly:

```cpp
std::vector<std::pair<int, int>> edges0 = {
    {0, 1},
    {1, 2},
    {2, 3}
};

Graph g0 = make_graph_from_edges(4, edges0, false);

std::vector<std::pair<int, int>> edges1 = {
    {1, 2},
    {2, 3},
    {3, 4}
};

Graph g1 = make_graph_from_edges_one_based(4, edges1, false);
```

- `Graph make_graph_from_edges(int n, const std::vector<std::pair<int, int>>& edges, bool directed = false)`:
  - Vertices are `0..n-1`.
  - Each pair `(u, v)` is added as an edge.
- `Graph make_graph_from_edges_one_based(int n, const std::vector<std::pair<int, int>>& edges, bool directed = false)`:
  - Input endpoints are `1..n`.
  - The function converts them internally to `0..n-1` and checks bounds.

For weighted graphs:

```cpp
std::vector<std::tuple<int, int, long long>> edges_w = {
    std::tuple<int, int, long long>(0, 1, 5),
    std::tuple<int, int, long long>(1, 2, 7)
};

Graph gw = make_weighted_graph_from_edges(3, edges_w, true);
```

- `Graph make_weighted_graph_from_edges(int n, const std::vector<std::tuple<int, int, long long>>& edges, bool directed = false)`
- `Graph make_weighted_graph_from_edges_one_based(int n, const std::vector<std::tuple<int, int, long long>>& edges, bool directed = false)`

These helpers are handy when reading testcases from standard input and storing them in vectors first.

### 3.5 BFS Distance Helper

For unweighted shortest paths from a single source you can use:

```cpp
Graph g(4, false);
g.add_edge(0, 1);
g.add_edge(1, 2);
g.add_edge(2, 3);

std::vector<int> dist = bfs_distances(g, 0);
```

- `std::vector<int> bfs_distances(const Graph& g, int source, int unreachable = -1)`:
  - Returns the distance (in edges) from `source` to each vertex.
  - Unreachable vertices take the value `unreachable` (default `-1`).

This is a convenience wrapper around a standard BFS, useful for quick checks and competitive programming.

For multiple sources, you can run a single BFS from all of them at once:

```cpp
std::vector<int> sources;
sources.push_back(0);
sources.push_back(3);

std::vector<int> dist_multi = bfs_multi_source(g, sources);
```

- `std::vector<int> bfs_multi_source(const Graph& g, const std::vector<int>& sources, int unreachable = -1)`:
  - Treats every valid vertex in `sources` as a starting point with distance `0`.
  - Returns the distance (in edges) from the nearest source to each vertex.
  - Unreachable vertices take the value `unreachable` (default `-1`).

This pattern is common in problems where you want the distance to the nearest “special” vertex.

### 3.6 Eulerian Trails and Cycles

For some problems you need to find an Eulerian trail or cycle that uses each edge exactly once.

Undirected graphs:

```cpp
Graph g(3, false);
g.add_edge(0, 1);
g.add_edge(1, 2);

bool has_trail = has_eulerian_trail_undirected(g);
bool has_cycle = has_eulerian_cycle_undirected(g);
std::vector<int> trail = eulerian_trail_undirected(g);
```

- `bool has_eulerian_trail_undirected(const Graph& g)`:
  - Returns `true` if an undirected Eulerian trail exists.
- `bool has_eulerian_cycle_undirected(const Graph& g)`:
  - Returns `true` if an undirected Eulerian cycle exists.
- `std::vector<int> eulerian_trail_undirected(const Graph& g)`:
  - Returns a vertex sequence representing an Eulerian trail if one exists.
  - Returns an empty vector if there is no Eulerian trail or the graph has no edges.

Directed graphs:

```cpp
Graph g(3, true);
g.add_edge(0, 1);
g.add_edge(1, 2);
g.add_edge(2, 0);

bool has_trail_dir = has_eulerian_trail_directed(g);
bool has_cycle_dir = has_eulerian_cycle_directed(g);
std::vector<int> trail_dir = eulerian_trail_directed(g);
```

- `bool has_eulerian_trail_directed(const Graph& g)`:
  - Checks for an Eulerian trail in a directed graph.
- `bool has_eulerian_cycle_directed(const Graph& g)`:
  - Checks for an Eulerian cycle in a directed graph.
- `std::vector<int> eulerian_trail_directed(const Graph& g)`:
  - Returns a vertex sequence for an Eulerian trail if one exists.
  - Returns an empty vector if the graph is not Eulerian or has no edges.

All these functions ignore isolated vertices with degree zero. Connectivity is checked only among vertices that are incident to at least one edge.

### 3.5 Global Minimum Cut (Undirected)

For undirected weighted graphs, you can compute the value of a global minimum cut (Stoer–Wagner style algorithm):

```cpp
Graph g(4, false);
g.add_edge(0, 1, 3);
g.add_edge(1, 2, 1);
g.add_edge(2, 3, 4);
g.add_edge(0, 3, 2);

long long cut_value = global_min_cut_undirected(g);
```

- `long long global_min_cut_undirected(const Graph& g)`:
  - Requires `g.is_directed() == false`. For directed graphs, it throws `std::invalid_argument`.
  - Returns the total weight of the minimum cut over all non-trivial partitions of the vertex set.
  - If `g` has `0` or `1` vertices, or no edges, returns `0`.

Typical use cases:

- Network reliability and partitioning.
- Finding sparsest cuts in small to medium graphs where an exact global cut is needed.

### 3.6 Gomory–Hu Tree (All-Pairs Min-Cut)

For undirected weighted graphs where you need **minimum s–t cut values for all vertex pairs**, you can build a Gomory–Hu cut tree:

```cpp
Graph g(4, false);
g.add_edge(0, 1, 3);
g.add_edge(1, 2, 1);
g.add_edge(2, 3, 4);
g.add_edge(0, 3, 2);

std::vector<int> parent;
std::vector<long long> min_cut;
gomory_hu_tree(g, parent, min_cut);
```

- `void gomory_hu_tree(const Graph& g, std::vector<int>& parent, std::vector<long long>& min_cut)`:
  - Requires `g.is_directed() == false`. For directed graphs, it throws `std::invalid_argument`.
  - Fills `parent` and `min_cut` with `n` entries each, where `n = g.vertex_count()`.
  - The undirected tree has edges between `v` and `parent[v]` with weight `min_cut[v]` for all `v` where `parent[v] != v`.
  - For any two vertices `s` and `t`, the value of the minimum `s–t` cut in `g` equals the **minimum edge weight** along the unique path between `s` and `t` in this tree.

Typical use cases:

- All-pairs min-cut queries on small to medium graphs.
- Network reliability analysis where many different source–sink pairs must be considered.
- As a preprocessing step for advanced cut-based algorithms.

---

## 4. Internal Representation (High-Level)

The core implementation of `Graph` in this library is intentionally low-level for performance:

- Vertices are stored implicitly as integers `0..n-1`.
- Adjacency lists are arrays of raw pointers (`Edge** adj_`).
- Each `Edge` contains:
  - `to`: destination vertex.
  - `weight`: edge weight.
  - `reverse_idx` (used by some algorithms needing reverse edges).
  - `next`: pointer to the next edge in the adjacency list.

This design is:

- Friendly to competitive-programming style usage.
- Cache-friendly for sequential edge traversal.
- Flexible enough to be reused by multiple algorithms.

You normally do not need to manipulate `Edge` directly unless:

- You are implementing your own algorithm on top of GraphLib.
- You want full control over adjacency traversal patterns.

---

## 5. Example: Custom BFS Using `Graph`

The library does not force you to use a specific BFS/DFS implementation. You can easily write your own on top of `Graph`.

```cpp
#include <graphlib/graph_core.h>
#include <queue>
#include <vector>

int main() {
    graphlib::Graph g(5, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(3, 4);

    std::vector<int> dist(g.vertex_count(), -1);
    std::queue<int> q;

    int start = 0;
    dist[start] = 0;
    q.push(start);

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (graphlib::Edge* e = g.get_edges(u); e != nullptr; e = e->next) {
            int v = e->to;
            if (dist[v] == -1) {
                dist[v] = dist[u] + 1;
                q.push(v);
            }
        }
    }

    return 0;
}
```

This example shows:

- How to iterate adjacency lists using `get_edges`.
- How to build standard graph algorithms on top of `Graph`.

---

## 6. Example: Building a Tree and Checking Properties

```cpp
#include <graphlib/graph_core.h>
#include <iostream>

int main() {
    int n = 5;
    graphlib::Graph tree(n, false);

    tree.add_edge(0, 1);
    tree.add_edge(1, 2);
    tree.add_edge(1, 3);
    tree.add_edge(3, 4);

    bool connected = graphlib::is_connected(tree);
    bool is_tree = graphlib::is_tree(tree);

    std::cout << "Connected: " << connected << "\\n";
    std::cout << "Is tree : " << is_tree << "\\n";
}
```

Use this pattern when:

- Validating that your input really forms a tree before running tree-only algorithms.
- Quickly checking whether extra edges have created cycles.

---

## 7. When to Use `Graph` vs Other Modules

Use `Graph` directly when:

- You only need to store a graph and perform custom traversals.
- You want a simple adjacency-list representation without higher-level algorithms.

Use specialized modules when:

- You need shortest paths → `ShortestPath`.
- You need max flow / min-cost flow → `MaxFlow`.
- You need MST → `MST` and `UnionFind`.
- You need connectivity / SCC → `Connectivity`, `SCC`.

As a rule of thumb:

- Start with `Graph` when experimenting or writing custom algorithms.
- Move to specialized modules when you see a pattern that matches a known problem (flow, shortest path, MST, etc.).
