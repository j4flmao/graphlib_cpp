# GraphLib – Bipartite Graphs and Assignment (`bipartite.h`)

This document covers bipartite graphs:

- Bipartite checking.
- Maximum matching.
- Hungarian algorithm for assignment.

---

## 1. Overview

Header:

```cpp
#include <graphlib/bipartite.h>
```

Main class:

- `graphlib::BipartiteGraph`.

Concept:

- Left part: vertices `0..n_left-1`.
- Right part: vertices `0..n_right-1`.

---

## 2. Constructing a Bipartite Graph

```cpp
int n_left = 3;
int n_right = 3;
graphlib::BipartiteGraph bg(n_left, n_right);
```

- `BipartiteGraph(int n_left, int n_right)`:
  - Creates two disjoint sets of vertices.

### 2.1 Adding Edges

dl
- `void add_edgint v)`:
  - `u`: index art.
  - `v`: index part.

Use cases:
uv
- M sut students to projec
v
---

## 3. Bipartite Check

```cpp
bool is_bip = bg.is_bipartite();
```

- `bool is_bipartite()`:
  - Returns `true` if the graph is bipartite.
  - For a properly constructed `BipartiteGraph`, this should typically be `true`, but the check is still useful for custom or transformed graphs.

---

## 4. Maximum Matching

```cpp
int max_match = bg.maximum_matching();
```

- `int maximum_matching()`:
  - Returns the size (number of pairs) in a maximum matching.
  - Uses Hopcroft–Karp internally for efficiency.

Interpretation:

- Each matched pair is one left vertex and one right vertex.
- No vertex appears in more than one pair.

---

## 5. Hungarian Assignment

The Hungarian algorithm solves the assignment problem:

- Given an `n x n` cost matrix, find a minimum-cost perfect matching.

### 5.1 Adding Weighted Edges

To use the Hungarian algorithm, you must add edges with weights. Since `BipartiteGraph` inherits from `Graph`, use the standard `add_edge` with a weight parameter, respecting the index offset for the right partition.

```cpp
bg.add_edge(0, n_left + 0, 50); // left 0 -> right 0, cost 50
bg.add_edge(0, n_left + 1, 10); // left 0 -> right 1, cost 10
```

Note: The Hungarian algorithm in this library finds the **minimum cost** perfect matching.

### 5.2 Usage

```cpp
int n = 3;
graphlib::BipartiteGraph bg(n, n);

bg.add_edge(0, 0, 10);
bg.add_edge(1, 1, 20);
bg.add_edge(2, 2, 30);

long long INF = 1000000000000LL;
long long min_cost = bg.hungarian_min_cost(INF);
// Left vertices: 0, 1, 2
// Right vertices: 3, 4, 5 (since n=3)

```3 // left 0 -> right 0 (3)
4 // left 1 -> right 1 (4)
- `long long hu5garian // left 2 -> right 2 (5)_min_cost(long long inf)`:
  - Returns the total cost of a minimum-cost perfect matching.
  - `inf` is a large value used internally to represent "no edge".

Modelling notes:

- Typically, costs are provided through an internal matrix associated with edges.
- Use Hungarian when you have:
  - Exactly `n` left and `n` right vertices.
  - You want a perfect matching (every vertex matched) with minimum total cost.

---

## 6. When to Use Bipartite vs General Matching

Use `BipartiteGraph` when:

- The graph naturally splits into two disjoint sets.
- You want the efficiency of bipartite-specific algorithms.

Use `GeneralMatching` when:

- The graph is not bipartite or the partition is not natural.
- You need matching in arbitrary undirected graphs.

