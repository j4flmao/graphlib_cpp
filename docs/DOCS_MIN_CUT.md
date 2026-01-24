# Minimum Cut and Gomory-Hu Tree

This module focuses on global minimum cuts and cut structures in undirected graphs.

Header:
```cpp
#include <graphlib/min_cut.h>
```

## 1. Global Minimum Cut

Finds the minimum cut in an undirected graph (without fixing source/sink).
- **Algorithm**: Stoer-Wagner Algorithm.
- **Complexity**: $O(VE + V^2 \log V)$.

```cpp
#include <graphlib/min_cut.h>

long long min_cut_val = graphlib::global_min_cut(g);
```

## 2. Gomory-Hu Tree

A **Gomory-Hu Tree** is a weighted tree that represents the minimum cut values between all pairs of vertices in the original graph.
- For any pair $(u, v)$, the min-cut in the original graph equals the minimum weight edge on the path between $u$ and $v$ in the Gomory-Hu tree.
- Uses $N-1$ max flow computations.

### Usage

```cpp
#include <graphlib/min_cut.h>

// Returns a graph representing the Gomory-Hu tree
// The weights on the edges represent the min-cut capacity
graphlib::Graph gh_tree = graphlib::gomory_hu_tree(g);

// Query min cut between 0 and 5
// In a tree, the min-cut is just the bottleneck edge on the unique path
// You can use BFS/DFS or LCA to find the path min
```

This structure effectively compresses the $N^2$ min-cut values into an $O(N)$ space structure.
