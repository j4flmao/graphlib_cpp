# Max Clique

## Overview
Finds the maximum clique (largest subset of vertices where all pairs are connected).
- **Complexity**: NP-Hard. Uses Bron-Kerbosch algorithm (efficient for dense/small graphs).

## Header
```cpp
#include <graphlib/clique.h>
```

## Functions

### `std::vector<int> maximum_clique(const Graph& g)`
- Returns *one* maximum clique.

### `std::vector<std::vector<int>> find_all_maximal_cliques(const Graph& g)`
- Returns *all* maximal cliques (cliques that cannot be extended).

## Example

```cpp
#include <graphlib/graphlib.h>
#include <graphlib/clique.h>
#include <iostream>

int main() {
    // Triangle
    graphlib::Graph g(3);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    
    auto clique = graphlib::maximum_clique(g);
    std::cout << "Max Clique Size: " << clique.size() << "\n"; // 3
}
```
