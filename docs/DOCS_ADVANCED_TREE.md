# GraphLib – Advanced Tree Algorithms

This document covers advanced tree algorithms:

- Centroid Decomposition.
- Dominator Tree.

---

## 1. Centroid Decomposition

Centroid Decomposition recursively divides a tree into centroids, creating a "tree of centroids" (or centroid tree) with logarithmic height.

### 1.1 Overview

Header:
```cpp
#include <graphlib/centroid_decomposition.h>
```

Class: `graphlib::CentroidDecomposition`

### 1.2 Usage

```cpp
graphlib::Graph tree(n, false);
// ... add edges to form a tree ...

graphlib::CentroidDecomposition cd(tree);

int root = cd.get_root();
int parent_of_u = cd.get_parent(u);
```

### 1.3 API

- `explicit CentroidDecomposition(const Graph& graph)`:
  - Computes the decomposition. The input `graph` must be a tree.
- `int get_root() const`:
  - Returns the root of the centroid tree.
- `int get_parent(int u) const`:
  - Returns the parent of `u` in the centroid tree (or -1 if `u` is the root).
- `int get_level(int u) const`:
  - Returns the depth of `u` in the centroid tree.
- `std::vector<std::vector<int>> get_tree() const`:
  - Returns the full adjacency list of the centroid tree.

---

## 2. Dominator Tree

A Dominator Tree represents the dominance relations in a flow graph. Node `u` dominates `v` if every path from the entry node to `v` goes through `u`.

### 2.1 Overview

Header:
```cpp
#include <graphlib/dominator_tree.h>
```

Class: `graphlib::DominatorTree`

### 2.2 Usage

```cpp
graphlib::Graph g(n, true); // Directed graph
// ... add edges ...

int entry_node = 0;
graphlib::DominatorTree dt(g, entry_node);

int idom = dt.get_idom(v); // Immediate dominator of v
```

### 2.3 API

- `DominatorTree(const Graph& graph, int root)`:
  - Builds the dominator tree for `graph` starting at `root`.
  - Uses the Lengauer-Tarjan algorithm ($O(E \log V)$ or almost linear).
- `int get_idom(int u) const`:
  - Returns the immediate dominator of `u`.
  - Returns -1 if `u` is unreachable or `u` is the root.
- `bool dominates(int u, int v) const`:
  - Returns `true` if `u` dominates `v` (i.e., `u` is an ancestor of `v` in the dominator tree).
- `std::vector<std::vector<int>> get_tree() const`:
  - Returns the adjacency list of the dominator tree (edges point from idom to node).
