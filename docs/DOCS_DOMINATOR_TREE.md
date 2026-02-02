# Dominator Tree

## Overview

A **Dominator Tree** is a structure representing the dominance relationships in a directed graph with a designated root node. Node `u` *dominates* node `v` if every path from the root to `v` goes through `u`.

The **Immediate Dominator** (`idom(v)`) of a node `v` is the unique node that strictly dominates `v` but does not dominate any other node that strictly dominates `v`. The immediate dominance relationship forms a tree rooted at the graph's root.

## Header

```cpp
#include <graphlib/dominator_tree.h>
```

## Class `DominatorTree`

Constructs and queries the dominator tree using the **Lengauer-Tarjan algorithm**, which runs in **O(M log N)** or **O(M alpha(N))** time.

### Constructor

```cpp
/**
 * @brief Builds the dominator tree for the given graph and root.
 * @param graph The input directed graph.
 * @param root The starting root vertex.
 */
DominatorTree(const Graph& graph, int root);
```

### Methods

- `int get_idom(int u) const`: Returns the immediate dominator of `u`. Returns `-1` if `u` is the root or unreachable.
- `bool dominates(int u, int v) const`: Returns `true` if `u` dominates `v` (i.e., `u` is an ancestor of `v` in the dominator tree).
- `std::vector<std::vector<int>> get_tree() const`: Returns the dominator tree structure as an adjacency list.

### Example

```cpp
#include <graphlib/graphlib.h>
#include <graphlib/dominator_tree.h>
#include <iostream>

int main() {
    graphlib::Graph g(6, true); // Directed
    g.add_edge(5, 4);
    g.add_edge(5, 3);
    g.add_edge(4, 2);
    g.add_edge(3, 2);
    g.add_edge(2, 1);
    g.add_edge(1, 0);

    // Root at 5
    graphlib::DominatorTree dt(g, 5);

    // 5 dominates everyone.
    // Paths to 2: 5->4->2 and 5->3->2.
    // 4 does NOT dominate 2 (path via 3 avoids 4).
    // 3 does NOT dominate 2.
    // 5 dominates 2.
    // 2 dominates 1.
    // 1 dominates 0.
    
    std::cout << "Idom(2): " << dt.get_idom(2) << "\n"; // Should be 5
    std::cout << "Idom(1): " << dt.get_idom(1) << "\n"; // Should be 2
}
```
