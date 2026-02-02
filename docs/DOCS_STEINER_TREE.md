# Steiner Tree

## Overview

The **Steiner Tree Problem** asks for the minimum weight tree that connects a specified subset of vertices (called **terminals**) in a weighted graph. It generalizes the Minimum Spanning Tree (MST) problem (where all vertices are terminals) and the Shortest Path problem (where 2 vertices are terminals).

The problem is **NP-hard**. GraphLib implements the classic **Dreyfus-Wagner Algorithm**, which is fixed-parameter tractable with respect to the number of terminals $k$.

**Complexity**: $O(3^k n + 2^k n^2 + n^3)$ or better depending on implementation details. This assumes $k$ is small (e.g., $k \le 12$).

## API

### `graphlib::steiner_tree`

```cpp
long long steiner_tree(const Graph& g, const std::vector<int>& terminals);
```

Computes the minimum cost of a Steiner Tree connecting the `terminals`.

**Parameters:**
- `g`: The input weighted graph.
- `terminals`: A list of vertex indices to be connected.

**Returns:**
- `long long`: The total weight of the Steiner Tree. Returns -1 if connectivity is impossible.

## Example

```cpp
#include <graphlib/graphlib.h>
#include <iostream>
#include <vector>

using namespace graphlib;

int main() {
    Graph g(4);
    // Star graph center 0, leaves 1, 2, 3
    g.add_edge(0, 1, 5); g.add_edge(1, 0, 5);
    g.add_edge(0, 2, 5); g.add_edge(2, 0, 5);
    g.add_edge(0, 3, 5); g.add_edge(3, 0, 5);
    
    // Connect leaves
    std::vector<int> terminals = {1, 2, 3};
    long long cost = steiner_tree(g, terminals);
    
    std::cout << "Min cost: " << cost << "\n"; // Output: 15 (via center)
}
```
