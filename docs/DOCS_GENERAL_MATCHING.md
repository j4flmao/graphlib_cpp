# GraphLib – General Graph Matching (`general_matching.h`)

This document explains general (non-bipartite) graph matching using Edmonds’ Blossom algorithm.

---

## 1. Overview

Header:

```cpp
#include <graphlib/general_matching.h>
```

Main class:

- `graphlib::GeneralMatching`.

Use cases:

- Pairing vertices in arbitrary undirected graphs.
- Maximum cardinality matching.
- Maximum weight matching.

---

## 2. Constructing the Matching Graph

```cpp
graphlib::GeneralMatching gm(4);
```

- `GeneralMatching(int n)`:
  - Undirected graph with vertices `0..n-1`.

### 2.1 Adding Edges

```cpp
gm.add_edge(0, 1, 5);
gm.add_edge(0, 2, 2);
gm.add_edge(1, 2, 4);
gm.add_edge(1, 3, 3);
gm.add_edge(2, 3, 1);
```

- `void add_edge(int u, int v, long long weight = 1)`:
  - Adds an undirected edge between `u` and `v`.
  - `weight` defaults to `1` for unweighted matching.

---

## 3. Maximum Cardinality Matching

```cpp
int size = gm.maximum_matching();
```

- `int maximum_matching()`:
  - Returns the number of edges in a maximum cardinality matching.

Meaning:

- A matching is a set of edges with no shared endpoints.
- Maximum cardinality means the matching contains as many edges as possible, regardless of weights.

---

## 4. Maximum Weight Matching

```cpp
long long weight = gm.maximum_weight_matching();
```

- `long long maximum_weight_matching()`:
  - Returns the sum of weights of edges in a maximum-weight matching.
  - After calling this, you can retrieve the matching using `get_mate()`.

```cpp
std::vector<int> mate = gm.get_mate();
for (int i = 0; i < n; ++i) {
    if (mate[i] != -1 && i < mate[i]) {
        std::cout << "Matched: " << i << " - " << mate[i] << std::endl;
    }
}
```

Differences vs maximum cardinality:

- Maximum cardinality ignores weights, maximizing number of pairs.
- Maximum weight considers weights, maximizing the total sum.

> **Note:** The Weighted Matching implementation in v1.0.4 is fully functional and verified. It uses an O(N³) primal-dual approach based on Edmonds' Blossom algorithm. It correctly handles odd cycles (blossoms) and complex weight structures. The implementation reduces the Maximum Weight Matching problem to a Perfect Matching problem on an expanded graph (2N nodes) with specific edge weights (bias and scaling) to ensure optimality. This method is robust but computationally intensive for very large dense graphs.

---

## 5. When to Use General Matching

Use `GeneralMatching` when:

- The graph is not bipartite.
- You cannot or do not want to partition vertices into "left" and "right".
- You need to handle odd cycles and blossoms correctly.

Examples:

- Matching people into pairs where edges encode compatibility scores.
- Selecting non-overlapping edges in arbitrary networks with weights.
