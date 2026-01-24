# Planarity Testing and Embedding

This module provides algorithms for checking if a graph is planar (can be drawn on a plane without edge crossings) and finding such an embedding.

Header:
```cpp
#include <graphlib/planarity.h>
```

## 1. Planarity Check

The `is_planar` function checks if a graph is planar. It uses **Demoucron's Algorithm**, which constructs a planar embedding incrementally or fails if one doesn't exist.

- **Time Complexity**: $O(V^2)$ in worst case (Demoucron's), but effectively linear for many graphs due to Euler's formula filtering.
- **Space Complexity**: $O(V + E)$.

### Usage

```cpp
#include <graphlib/planarity.h>
#include <graphlib/graph_core.h>

graphlib::Graph g(5, false); // Undirected graph
g.add_edge(0, 1);
g.add_edge(1, 2);
g.add_edge(2, 3);
g.add_edge(3, 4);
g.add_edge(4, 0);
g.add_edge(0, 2); // Chord

if (graphlib::is_planar(g)) {
    std::cout << "The graph is planar!" << std::endl;
} else {
    std::cout << "The graph is NOT planar." << std::endl;
}
```

### Algorithm Details

1. **Preprocessing**: Checks if $E > 3V - 6$. If so, the graph is non-planar (Euler's formula corollary).
2. **Disconnected Graphs**: The algorithm handles disconnected graphs by checking each connected component individually.
3. **Embedding Construction**:
   - Finds an initial cycle.
   - Identifies "fragments" (connected components relative to the current embedding).
   - Places fragments into faces of the current embedding.
   - If a fragment cannot be placed in any face, the graph is non-planar.

### Note on K5 and K3,3

The complete graph $K_5$ and the complete bipartite graph $K_{3,3}$ are the classic non-planar graphs (Kuratowski's theorem). This implementation correctly identifies them as non-planar.
