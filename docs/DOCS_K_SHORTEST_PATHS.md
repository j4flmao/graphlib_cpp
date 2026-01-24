# K-Shortest Paths

This module finds the $K$ shortest loopless paths between two nodes in a graph. This is useful for routing alternatives, backup paths, and network analysis.

Header:
```cpp
#include <graphlib/k_shortest_paths.h>
```

## 1. Yen's Algorithm

The library implements **Yen's Algorithm**, which uses a series of deviations from the shortest path to find the next shortest paths.

- **Algorithm**: Yen's Algorithm with Blocked Dijkstra.
- **Time Complexity**: $O(K \cdot N \cdot (M + N \log N))$.

### Usage

```cpp
#include <graphlib/k_shortest_paths.h>
#include <graphlib/graph_core.h>
#include <iostream>

graphlib::Graph g(6, true); // Directed graph
g.add_edge(0, 1, 3);
g.add_edge(0, 2, 2);
g.add_edge(2, 1, 1); // 0->2->1 is cost 3, same as 0->1
g.add_edge(1, 3, 4);
// ... more edges

// Find top 3 shortest paths from node 0 to node 3
int k = 3;
std::vector<std::vector<int>> paths = graphlib::yen_k_shortest_paths(g, 0, 3, k);

for (size_t i = 0; i < paths.size(); ++i) {
    std::cout << "Path " << i + 1 << ": ";
    for (int node : paths[i]) {
        std::cout << node << " ";
    }
    std::cout << std::endl;
}
```

### Return Value

The function returns a `std::vector<std::vector<int>>`, where each inner vector is a sequence of node indices representing a path.
- The paths are sorted by total weight (ascending).
- If fewer than $K$ paths exist, the returned vector will have size $< K$.
- If no path exists, it returns an empty vector.

### Constraints

- Edge weights must be non-negative (because it relies on Dijkstra).
- The algorithm finds **loopless** paths.
