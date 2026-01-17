# GraphLib – Minimum Spanning Tree and Union-Find (`mst.h`)

This document explains:

- The `UnionFind` (Disjoint Set Union) data structure.
- Minimum Spanning Tree algorithms: Kruskal and Prim.

---

## 1. Overview

Header:

```cpp
#include <graphlib/mst.h>
```

Main types:

- `graphlib::UnionFind`.
- `graphlib::MstEdge`.
- `graphlib::MST`.

---

## 2. UnionFind

### 2.1 Construction

```cpp
graphlib::UnionFind uf(5);
```

- `UnionFind(int n)`:
  - Creates a disjoint-set structure for elements `0..n-1`.

### 2.2 Operations

```cpp
uf.unite(0, 1);
uf.unite(1, 2);
bool same = (uf.find(0) == uf.find(2));
```

- `int find(int x)`:
  - Returns the representative of the set containing `x`.
- `bool unite(int x, int y)`:
  - Merges the sets containing `x` and `y`.
  - Returns `true` if a merge actually occurred, `false` if they were already in the same set.

Use cases:

- Tracking connected components.
- Supporting Kruskal’s MST.

---

## 3. Kruskal’s MST

### 3.1 Edge Representation

```cpp
std::vector<graphlib::MstEdge> edges;
edges.push_back({0, 1, 5});
edges.push_back({1, 2, 3});
edges.push_back({0, 2, 10});
```

- `struct MstEdge { int u; int v; long long weight; };`
  - Undirected edge between `u` and `v` with given weight.

### 3.2 Computing MST

```cpp
long long mst_weight = graphlib::MST::kruskal(3, edges);
```

- `static long long kruskal(int n, std::vector<MstEdge>& edges)`:
  - `n`: number of vertices.
  - `edges`: list of edges.
  - Returns the total weight of a minimum spanning tree.

Use this when:

- The graph is sparse.
- You already have a list of edges available.

---

## 4. Prim’s MST

### 4.1 Adjacency Representation

```cpp
int n = 3;
std::vector<std::vector<std::pair<int, long long>>> adj(n);
adj[0].push_back({1, 5});
adj[1].push_back({0, 5});
adj[1].push_back({2, 3});
adj[2].push_back({1, 3});
```

### 4.2 Computing MST

```cpp
long long INF = 1000000000000LL;
long long prim_weight = graphlib::MST::prim(n, 0, adj, INF);
```

- `static long long prim(int n, int start, const std::vector<std::vector<std::pair<int, long long>>>& adj, long long inf)`:
  - `n`: number of vertices.
  - `start`: starting vertex.
  - `adj`: adjacency list with `(neighbor, weight)` pairs.
  - `inf`: sentinel for "infinite" distance.

Use this when:

- You naturally store graph as adjacency lists.
- The graph is relatively dense or you want an alternative to Kruskal.

---

## 5. Choosing Between Kruskal and Prim

- Kruskal:
  - Easier when starting from an edge list.
  - Uses sorting + Union-Find.
- Prim:
  - Convenient when you already have an adjacency list.
  - Works well on dense graphs.

