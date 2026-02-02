# GraphLib – General Graph Matching (`general_matching.h`)

This document covers matching in general (non-bipartite) graphs:

- Maximum cardinality matching.
- Maximum weight matching.
- Edmonds' Blossom algorithm.

---

## 1. Overview

Header:

```cpp
#include <graphlib/general_matching.h>
```

Main class:

- `graphlib::GeneralMatching`.

Concept:

- A **matching** in a graph is a set of edges without common vertices.
- A **maximum matching** has the largest possible number of edges.
- A **maximum weight matching** maximizes the sum of edge weights.

Unlike bipartite matching, general matching must handle **odd cycles** (blossoms), which require specialized algorithms.

---

## 2. Constructing a GeneralMatching Graph

```cpp
int n = 5;
graphlib::GeneralMatching gm(n);
```

- `GeneralMatching(int n)`:
  - Creates an undirected graph with `n` vertices.
  - Inherits from `Graph`, so use `add_edge(u, v)` or `add_edge(u, v, weight)` to add edges.

### 2.1 Adding Edges

```cpp
gm.add_edge(0, 1);       // Unweighted edge
gm.add_edge(1, 2, 10);   // Weighted edge with weight 10
gm.add_edge(2, 3, 5);
gm.add_edge(3, 4, 8);
gm.add_edge(4, 0, 3);
```

---

## 3. Maximum Cardinality Matching

```cpp
int match_size = gm.maximum_matching();
```

- `int maximum_matching()`:
  - Returns the number of matched pairs (size of the matching).
  - Uses Edmonds' Blossom algorithm internally.

### 3.1 Retrieving the Matching

```cpp
std::vector<int> mate = gm.get_mate();
// mate[v] = u means vertex v is matched to vertex u
// mate[v] = -1 means vertex v is unmatched
```

- `std::vector<int> get_mate() const`:
  - Returns the mate array after calling `maximum_matching()` or `maximum_weight_matching()`.

---

## 4. Maximum Weight Matching

```cpp
long long total_weight = gm.maximum_weight_matching();
```

- `long long maximum_weight_matching()`:
  - Returns the maximum total weight of a matching.
  - Uses a weighted variant of Edmonds' algorithm with dual variables.
  - Handles negative, zero, and positive edge weights.

After calling this method, use `get_mate()` to retrieve which vertices are matched.

---

## 5. Edmonds' Blossom Algorithm

### 5.1 The Challenge of Odd Cycles

In bipartite graphs, augmenting paths can be found with simple BFS/DFS. In general graphs, **odd cycles** (blossoms) complicate the search:

```
    1 --- 2
   / \   /
  0   \ /
   \   3
    \ /
     4
```

If vertices 1, 2, 3 form an odd cycle and we're searching for augmenting paths, treating them naively can miss valid paths.

### 5.2 Blossom Contraction

Edmonds' algorithm solves this by **contracting** blossoms:

1. When an odd cycle is detected during augmenting path search, contract it into a single "super-vertex".
2. Continue searching in the contracted graph.
3. When an augmenting path is found, **expand** blossoms to recover the actual path.

### 5.3 Algorithm Steps

1. Start from an unmatched vertex.
2. Build an alternating tree using BFS.
3. If a blossom is detected (two even-level vertices connected), contract it.
4. If an augmenting path is found, augment the matching.
5. Repeat until no more augmenting paths exist.

---

## 6. Weighted Matching Details

The weighted matching implementation uses:

- **Dual variables** (labels) for each vertex.
- **Equality subgraph**: edges where `label[u] + label[v] == weight(u,v)`.
- **Delta adjustments** to progressively expand the equality subgraph.

The algorithm reduces the problem to finding a maximum weight **perfect** matching on an auxiliary graph with 2n vertices, then extracts the original matching.

---

## 7. Usage Examples

### 7.1 Simple Triangle

```cpp
#include <graphlib/general_matching.h>
#include <iostream>

int main() {
    graphlib::GeneralMatching gm(3);
    gm.add_edge(0, 1);
    gm.add_edge(1, 2);
    gm.add_edge(2, 0);

    int size = gm.maximum_matching();
    std::cout << "Maximum matching size: " << size << "\n"; // Output: 1

    auto mate = gm.get_mate();
    for (int i = 0; i < 3; i++) {
        if (mate[i] > i) {
            std::cout << "Matched: " << i << " - " << mate[i] << "\n";
        }
    }
    return 0;
}
```

### 7.2 Weighted Matching

```cpp
#include <graphlib/general_matching.h>
#include <iostream>

int main() {
    graphlib::GeneralMatching gm(4);
    gm.add_edge(0, 1, 10);
    gm.add_edge(0, 2, 5);
    gm.add_edge(1, 3, 8);
    gm.add_edge(2, 3, 12);

    long long weight = gm.maximum_weight_matching();
    std::cout << "Maximum weight: " << weight << "\n"; // Output: 22 (edges 0-1 and 2-3)

    auto mate = gm.get_mate();
    for (int i = 0; i < 4; i++) {
        if (mate[i] > i) {
            std::cout << "Matched: " << i << " - " << mate[i] << "\n";
        }
    }
    return 0;
}
```

### 7.3 Pentagon (Odd Cycle)

```cpp
#include <graphlib/general_matching.h>
#include <iostream>

int main() {
    graphlib::GeneralMatching gm(5);
    // Pentagon: 0-1-2-3-4-0
    gm.add_edge(0, 1);
    gm.add_edge(1, 2);
    gm.add_edge(2, 3);
    gm.add_edge(3, 4);
    gm.add_edge(4, 0);

    int size = gm.maximum_matching();
    std::cout << "Maximum matching size: " << size << "\n"; // Output: 2

    return 0;
}
```

---

## 8. Complexity

| Method                      | Time Complexity     | Space Complexity |
|-----------------------------|---------------------|------------------|
| `maximum_matching()`        | O(V³)               | O(V + E)         |
| `maximum_weight_matching()` | O(V³)               | O(V²)            |

Where:
- V = number of vertices
- E = number of edges

The O(V³) complexity comes from:
- Up to V augmenting path searches.
- Each search involves O(V²) work in the worst case due to blossom operations.

---

## 9. When to Use GeneralMatching

Use `GeneralMatching` when:

- The graph is **not bipartite** (contains odd cycles).
- You need matching in arbitrary undirected graphs.
- You need **weighted** matching in non-bipartite graphs.

Use `BipartiteGraph` when:

- The graph naturally splits into two disjoint vertex sets.
- You want the efficiency of bipartite-specific algorithms (Hopcroft-Karp, Hungarian).

---

## 10. API Reference

### Class: `graphlib::GeneralMatching`

Inherits from `graphlib::Graph`.

| Method                        | Description                                      |
|-------------------------------|--------------------------------------------------|
| `GeneralMatching(int n)`      | Construct graph with n vertices                  |
| `int maximum_matching()`      | Compute and return maximum cardinality matching  |
| `long long maximum_weight_matching()` | Compute and return maximum weight matching |
| `std::vector<int> get_mate() const`   | Get mate array after matching computation  |

Inherited from `Graph`:
- `void add_edge(int u, int v)`
- `void add_edge(int u, int v, long long weight)`
- `int vertex_count() const`
- `Edge* get_edges(int u) const`
