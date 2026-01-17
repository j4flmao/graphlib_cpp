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

```cpp
bg.add_edge(0, 0); // left 0 -> right 0
bg.add_edge(0, 1); // left 0 -> right 1
bg.add_edge(1, 1); // left 1 -> right 1
```

- `void add_edge(int u, int v)`:
  - `u`: index in left part.
  - `v`: index in right part.

Use cases:

- Matching workers to jobs.
- Matching students to projects.

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

```cpp
int n = 3;
graphlib::BipartiteGraph bg(n, n);

bg.add_edge(0, 0);
bg.add_edge(1, 1);
bg.add_edge(2, 2);

long long INF = 1000000000000LL;
long long min_cost = bg.hungarian_min_cost(INF);
```

- `long long hungarian_min_cost(long long inf)`:
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

