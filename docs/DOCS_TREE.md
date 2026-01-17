# GraphLib – Tree and LCA (`tree.h`)

This document explains the `TreeLCA` helper built on top of the core `Graph`:

- Lightweight tree representation using adjacency lists.
- Binary lifting for lowest common ancestor (LCA) queries.
- Distance queries on trees in terms of number of edges.

---

## 1. Overview

Header:

```cpp
#include <graphlib/tree.h>
```

Namespace:

```cpp
using namespace graphlib;
```

`TreeLCA` is meant for problems where:

- The underlying structure is a rooted tree (no cycles, connected, `n - 1` edges).
- You need to answer many queries of the form:
  - `lca(u, v)` – lowest common ancestor of `u` and `v`.
  - `distance(u, v)` – distance between two vertices in edges.
  - `parent(v)` and `depth(v)` of each vertex in the rooted tree.

Internally it uses binary lifting with `O(n log n)` preprocessing and `O(log n)` per query.

---

## 2. TreeLCA Class

### 2.1 Construction

```cpp
TreeLCA tree(n);
```

- `explicit TreeLCA(int n)`:
  - Creates a tree structure with `n` vertices labeled `0..n-1`.
  - Internally uses an undirected `Graph` (edges are added in both directions).

### 2.2 Adding Edges

Use `add_edge` from the base `Graph` class to build the tree:

```cpp
TreeLCA tree(n);
for (int i = 0; i < n - 1; i++) {
    int u = ...;  // 0-based endpoints
    int v = ...;
    tree.add_edge(u, v);
}
```

Assumptions:

- The resulting structure must be a tree:
  - Connected.
  - Exactly `n - 1` edges (each undirected edge added once via `add_edge`).
- Vertex indices are `0..n-1`. If you read `1..n` from input, subtract `1`.

### 2.3 Building the Rooted Tree

Before answering queries, call `build(root)` to precompute parents and binary lifting tables:

```cpp
int root = 0;
tree.build(root);
```

- `void build(int root)`:
  - Checks that the vertex count is positive.
  - Validates that `root` is in range.
  - Runs a DFS from `root` to compute:
    - `depth[v]` – depth of `v` from `root` (with `depth[root] = 0`).
    - `up[k][v]` – the `2^k`-th ancestor of `v` (or `-1` if it does not exist).
  - Enables all subsequent LCA and distance queries.

If `build` has not been called or if inputs are out of range, the query methods return safe sentinel values (described below).

---

## 3. Query Methods

After calling `build`, you can use the following queries.

### 3.1 Lowest Common Ancestor

```cpp
int w = tree.lca(u, v);
```

- `int lca(int u, int v) const`:
  - Returns the lowest common ancestor of vertices `u` and `v`.
  - Both `u` and `v` must be within `0..n-1`.
  - If the tree was not built or inputs are invalid, returns `-1`.

The implementation:

- Lifts the deeper vertex up to the same depth as the shallower one.
- Then lifts both simultaneously from high powers of two downwards until their parents match.

### 3.2 Distance Between Two Vertices

```cpp
int d = tree.distance(u, v);
```

- `int distance(int u, int v) const`:
  - Returns the number of edges on the unique path between `u` and `v`.
  - Computed as:

    \[
      \text{distance}(u, v) = \text{depth}(u) + \text{depth}(v) - 2 \cdot \text{depth}(\text{lca}(u, v))
    \]

  - If the LCA does not exist (invalid input or `build` not called), returns `-1`.

### 3.3 Parent and Depth

```cpp
int p = tree.parent(v);
int h = tree.depth(v);
```

- `int parent(int v) const`:
  - Returns the parent of `v` in the rooted tree.
  - For the root, returns `-1`.
  - If `build` was not called or `v` is out of range, returns `-1`.

- `int depth(int v) const`:
  - Returns the depth (distance from the root) of `v`.
  - For the root, returns `0`.
  - If `build` was not called or `v` is out of range, returns `0`.

### 3.4 Ancestor Checks and k-th Ancestor

Additional helper methods:

```cpp
bool a = tree.is_ancestor(u, v);
int kpar = tree.kth_ancestor(v, k);
int sz = tree.subtree_size(v);
```

- `bool is_ancestor(int u, int v) const`:
  - Returns `true` if `u` is an ancestor of `v` (including the case `u == v`) in the rooted tree.
  - Returns `false` if the tree was not built or indices are out of range.
- `int kth_ancestor(int v, int k) const`:
  - Returns the `k`‑th ancestor of `v` (0‑th ancestor is `v` itself).
  - If the ancestor does not exist or input is invalid, returns `-1`.
- `int subtree_size(int v) const`:
  - Returns the size of the subtree rooted at `v` (number of vertices).
  - Returns `0` if the tree was not built or `v` is out of range.

These are common building blocks for tree DP, path queries, and many competitive‑programming problems.

### 3.5 k-th Vertex on a Path

Sometimes you need the k-th vertex along the unique path between two vertices:

```cpp
int x = tree.kth_on_path(u, v, k);
```

- `int kth_on_path(int u, int v, int k) const`:
  - Interprets the path from `u` to `v` as a sequence of vertices
    `p[0] = u, p[1], ..., p[L] = v`.
  - Returns `p[k]` for `0 <= k <= L`.
  - Returns `-1` if the tree was not built, indices are invalid, or `k` is outside the path length.

Typical use cases:

- Path queries where you need to jump to a specific position along the path.
- Implementing binary searches on paths in tree problems.

### 3.6 Heavy-Light Decomposition Helpers

`TreeLCA` also precomputes a heavy-light decomposition (HLD) structure that maps each vertex to a position in a base array and a path head. This is useful when you want to attach a segment tree or Fenwick tree for path queries.

```cpp
int p = tree.hld_pos(v);
int h = tree.hld_head(v);
std::vector<std::pair<int, int>> segments;
tree.hld_decompose_path(u, v, segments);
```

- `int hld_pos(int v) const`:
  - Returns the index of vertex `v` in the HLD base array (0-based).
  - Returns `-1` if the tree was not built or `v` is out of range.
- `int hld_head(int v) const`:
  - Returns the head vertex of the heavy path containing `v`.
  - Returns `-1` if the tree was not built or `v` is out of range.
- `void hld_decompose_path(int u, int v, std::vector<std::pair<int, int>>& segments) const`:
  - Decomposes the path from `u` to `v` into `O(log n)` disjoint segments in the HLD base array.
  - Each pair `(l, r)` represents a contiguous range of positions in the base array.
  - The function clears `segments` before appending new ranges.

Typical usage pattern:

- Build a segment tree or Fenwick tree over an array `value[p]` where `p = hld_pos(v)`.
- To process a path query between `u` and `v`:
  - Call `hld_decompose_path(u, v, segments)`.
    - For each `(l, r)` in `segments`, query or update the range `[l, r]` as needed.

### 3.7 TreePathSum: Path and Subtree Sums

On top of `TreeLCA` and its HLD helpers, the library provides `TreePathSum` for common path and subtree sum queries.

```cpp
TreeLCA tree(n);
// add n-1 edges
tree.build(0);

std::vector<long long> values(n);
// values[v] is value stored at vertex v

TreePathSum ps(tree, values);

long long s = ps.query_path(u, v);
long long sub = ps.query_subtree(v);
```

Main operations:

- `TreePathSum(const TreeLCA& tree, const std::vector<long long>& values)`:
  - `values.size()` must equal the number of vertices of `tree`.
  - `tree.build(root)` must have been called before constructing `TreePathSum`.
- `long long query_path(int u, int v) const`:
  - Returns the sum of `values` along the unique path from `u` to `v` (inclusive).
  - If indices are out of range, returns `0`.
- `long long query_subtree(int v) const`:
  - Returns the sum of `values` over the subtree rooted at `v`.
  - If `v` is out of range, returns `0`.
- `void set_value(int v, long long value)` / `void add_value(int v, long long delta)`:
  - Point updates on a single vertex, in `O(log n)` time.
- `long long get_value(int v) const`:
  - Reads back the current value associated with vertex `v`.

Internally `TreePathSum`:

- Uses `hld_pos(v)` to map vertices to positions of a base array.
- Uses `hld_decompose_path(u, v, segments)` to decompose any path into a small number of contiguous ranges.
- Maintains a segment tree over the base array, so both path and subtree sums run in `O(log^2 n)` and `O(log n)` respectively.

---

## 4. Example Usage

### 4.1 Simple Setup

```cpp
#include <graphlib/tree.h>
#include <iostream>

int main() {
    int n = 5;
    graphlib::TreeLCA tree(n);

    tree.add_edge(0, 1);
    tree.add_edge(1, 2);
    tree.add_edge(1, 3);
    tree.add_edge(3, 4);

    tree.build(0);

    int u = 2;
    int v = 4;
    int w = tree.lca(u, v);
    int d = tree.distance(u, v);

    std::cout << "LCA(" << u << ", " << v << ") = " << w << "\\n";
    std::cout << "dist(" << u << ", " << v << ") = " << d << "\\n";

    return 0;
}
```

### 4.2 Contest-Style Input

To see a full competitive-programming style solution:

- Read `n` and `q`.
- Read `n - 1` edges.
- Build `TreeLCA`.
- Answer `q` queries of the form `(u, v)` by printing `lca(u, v)` and `distance(u, v)`.

Refer to `examples/example_lca.cpp` for a complete program.

---

## 5. Complexity and Notes

- Preprocessing:
  - `build(root)` runs in `O(n log n)` time and `O(n log n)` memory (LCA + HLD).
- Queries:
  - `lca(u, v)` runs in `O(log n)` time.
  - `distance(u, v)` runs in `O(log n)` time (dominated by the LCA call).
  - `parent(v)` and `depth(v)` run in `O(1)` time.
  - `hld_decompose_path(u, v, segments)` runs in `O(log n)` time.

Guidelines:

- Ensure the input really forms a tree; otherwise, the semantics of LCA are undefined.
- Always call `build(root)` before using LCA or distance queries.
- Use zero-based indices inside `TreeLCA`. Convert from one-based input by subtracting one.
