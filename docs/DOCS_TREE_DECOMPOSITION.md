# Tree Decomposition

This module computes a Tree Decomposition of a graph, which is useful for solving NP-hard problems on graphs with small treewidth.

Header:
```cpp
#include <graphlib/tree_decomposition.h>
```

## 1. Min-Degree Heuristic

Finding the optimal tree decomposition (and treewidth) is NP-hard. This module implements the **Min-Degree Heuristic**, which often produces good approximations in practice.

### Data Structures

```cpp
struct TreeDecompositionResult {
    Graph tree;                         // The decomposition tree
    std::vector<std::vector<int>> bags; // bags[i] = vertices in bag i
    int width;                          // The width of this decomposition
};
```

### Usage

```cpp
#include <graphlib/tree_decomposition.h>

graphlib::Graph g(6, false);
// ... add edges ...

graphlib::TreeDecompositionResult td = graphlib::tree_decomposition_min_degree(g);

std::cout << "Treewidth (approx): " << td.width << std::endl;

// Iterate through bags
for (int i = 0; i < td.tree.vertex_count(); ++i) {
    std::cout << "Bag " << i << ": {";
    for (int v : td.bags[i]) std::cout << v << " ";
    std::cout << "}\n";
}
```

### Applications
- Dynamic programming on tree decompositions.
- Constraint satisfaction problems.
- Probabilistic inference.
