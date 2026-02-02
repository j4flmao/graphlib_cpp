# Graph Coloring

## Overview
Assigns colors to vertices such that no adjacent vertices share the same color.
- **Problem**: Minimize number of colors (Chromatic Number).
- **Status**: NP-Hard.
- **Algorithms**: 
    - `greedy_coloring`: Fast heuristic (DSATUR). Good approximation.
    - `chromatic_number`: Exact backtracking. Slow for large N.

## Header
```cpp
#include <graphlib/coloring.h>
```

## Functions

### `std::vector<int> greedy_coloring(const Graph& g)`
- Returns vector of color IDs (0, 1, 2...).

### `int chromatic_number(const Graph& g)`
- Returns the exact chromatic number.

## Example

```cpp
#include <graphlib/graphlib.h>
#include <graphlib/coloring.h>
#include <iostream>

int main() {
    // Square graph (Bipartite)
    graphlib::Graph g(4);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 3); g.add_edge(3, 2);
    g.add_edge(3, 0); g.add_edge(0, 3);
    
    std::vector<int> colors = graphlib::greedy_coloring(g);
    std::cout << "Exact Chromatic Number: " << graphlib::chromatic_number(g) << "\n"; // 2
}
```
