# Tree Algorithms

## Overview
A collection of efficient algorithms specifically for trees.
- **Complexity**: Linear `O(N)` for most DP/Greedy algorithms.

## Header
```cpp
#include <graphlib/tree_algo.h>
```

## Functions

### `TreeDiameterResult get_tree_diameter(const Graph& tree)`
- Finds the longest path in the tree.
- Returns `{u, v, length, path}`.

### `std::vector<int> get_tree_centers(const Graph& tree)`
- Finds the 1 or 2 centers of the tree.
- Centers minimize the maximum distance to any other node.

### `long long max_weight_independent_set_tree(const Graph& tree, const std::vector<long long>& weights, std::vector<int>& selected)`
- Solves MWIS on trees using DP.
- Returns max weight.

### `int min_dominating_set_tree(const Graph& tree, std::vector<int>& selected)`
- Solves MDS on trees using Greedy approach.
- Returns size of MDS.

## Example

```cpp
#include <graphlib/graphlib.h>
#include <graphlib/tree_algo.h>
#include <iostream>

int main() {
    graphlib::Graph g(4);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 3); g.add_edge(3, 2);
    
    auto d = graphlib::get_tree_diameter(g);
    std::cout << "Diameter: " << d.length << "\n"; // 3
    
    std::vector<int> mds;
    graphlib::min_dominating_set_tree(g, mds);
    std::cout << "MDS Size: " << mds.size() << "\n"; // Likely 2 ({1,2} or {1,3}?)
}
```
