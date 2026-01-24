# Advanced Graph Algorithms

This document covers "extreme" or advanced graph algorithms available in GraphLib.

## 1. Tree Isomorphism (`tree_algo.h`)

Determine if two trees are structurally identical.

### 1.1 Rooted Tree Isomorphism

Checks if two trees are isomorphic when rooted at specific vertices.

```cpp
#include <graphlib/tree_algo.h>

// t1 rooted at 0, t2 rooted at 1
if (graphlib::are_trees_isomorphic(t1, 0, t2, 1)) {
    // ...
}
```

### 1.2 Unrooted Tree Isomorphism

Checks if two free trees are isomorphic (regardless of root). This algorithm automatically finds the centers of the trees and compares their canonical forms.

```cpp
if (graphlib::are_unrooted_trees_isomorphic(t1, t2)) {
    // ...
}
```

### 1.3 Tree Center

Finds the center(s) of a tree. A tree has 1 or 2 centers.

```cpp
std::vector<int> centers = graphlib::tree_center(tree);
```

---

## 2. Steiner Tree (`steiner_tree.h`)

The Steiner Tree problem asks for the minimum weight tree that connects a given subset of "terminal" vertices. This is an NP-Hard problem.

GraphLib uses the **Dreyfus-Wagner algorithm**.

- **Complexity**: $O(3^k \cdot n + 2^k \cdot n^2 + n^3)$ where $k$ is the number of terminals.
- **Constraint**: $k$ should be small (e.g., $k \le 12$). $n$ can be moderate (e.g., $n \le 100$).

```cpp
#include <graphlib/steiner_tree.h>

std::vector<int> terminals = {0, 5, 10};
long long cost = graphlib::steiner_tree(graph, terminals);

if (cost != -1) {
    std::cout << "Minimum Steiner Tree Cost: " << cost << std::endl;
}
```
