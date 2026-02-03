# GraphLib – Graph Isomorphism

This module provides algorithms to determine if two graphs are structurally identical (isomorphic) or if one graph is contained within another.

---

## 1. Overview

### Headers

```cpp
#include <graphlib/isomorphism.h>
```

### Key Functions

| Function | Description | Complexity |
|----------|-------------|------------|
| `is_isomorphic` | Checks if $G_1 \cong G_2$ generally. | $O(N! \cdot N)$ worst-case |
| `is_tree_isomorphic` | Specialized fast check for trees. | $O(N \log N)$ |
| `is_subgraph_isomorphic` | Checks if $G_{pattern} \subseteq G_{target}$. | NP-Complete |
| `find_all_subgraph_isomorphisms` | Finds all embeddings of pattern in target. | Exponential |

---

## 2. Graph Isomorphism

Determines if two graphs have identical structures, meaning there exists a one-to-one mapping of vertices preserving adjacency.

### 2.1 General Graphs (VF2 Algorithm)
Uses the **VF2 Algorithm**, a state-of-the-art backtracking method with heuristic pruning.

```cpp
graphlib::Graph g1(4), g2(4);
// ... construct graphs ...

std::vector<int> mapping;
if (graphlib::is_isomorphic(g1, g2, &mapping)) {
    // mapping[u] = v means vertex u in g1 maps to v in g2
}
```

### 2.2 Tree Isomorphism (AHU Algorithm)
Uses the **Aho-Hopcroft-Ullman** algorithm (Canonical Labeling with Centers), which is much faster than general purpose solvers.

```cpp
if (graphlib::is_tree_isomorphic(t1, t2)) {
    // Trees are structurally identical
}
```

---

## 3. Subgraph Isomorphism

Determines if a smaller "pattern" graph exists inside a larger "target" graph.

### 3.1 Existence Check

```cpp
if (graphlib::is_subgraph_isomorphic(pattern, target)) {
    std::cout << "Pattern found!" << std::endl;
}
```

### 3.2 Finding All Occurrences

Useful for motif searching or pattern matching in networks.

```cpp
auto matches = graphlib::find_all_subgraph_isomorphisms(pattern, target);

std::cout << "Found " << matches.size() << " occurrences." << std::endl;
for (const auto& map : matches) {
    // Process each valid mapping
}
```

---

## 4. Theory & Implementation Notes

### VF2 Algorithm
- **State Space**: Maintains a partial mapping $M$.
- **Pruning**: Uses feasibility rules (R_pred, R_succ, R_in, R_out) to prune branches that cannot lead to a valid isomorphism.
- **Performance**: While worst-case exponential ($N!$), it works efficiently on sparse graphs ($O(N^2)$ typical).

### Tree Canonical Labeling
- **Rooting**: Roots the tree at its center(s) (1 or 2 centers guaranteed).
- **Encoding**: Recursively encodes each subtree into a string/tuple `(sorted_child_codes)`.
- **Comparison**: Two trees are isomorphic iff their canonical strings are identical.

---

## 5. Performance Guidelines

- **Use `is_tree_isomorphic`** whenever possible; it is practically linear time compared to VF2's potential exponential time.
- **Node Invariants**: The current VF2 implementation uses node degrees and connection types for early pruning.
- **Large Graphs**: Subgraph isomorphism on graphs with $N > 1000$ can be extremely slow if the pattern is complex. Use motif-counting algorithms for generic patterns like triangles.
