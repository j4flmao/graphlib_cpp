# GraphLib – K-Shortest Paths (`k_shortest_paths.h`)

This document covers the K-Shortest Paths algorithm for finding multiple shortest paths between two vertices.

---

## 1. Overview

Header:

```cpp
#include <graphlib/k_shortest_paths.h>
```

Main function:

- `graphlib::yen_k_shortest_paths` – Yen's algorithm for finding K shortest loopless paths.

---

## 2. API Reference

### yen_k_shortest_paths

```cpp
std::vector<std::vector<int>> yen_k_shortest_paths(
    const Graph& g,
    int start,
    int end,
    int k
);
```

Parameters:

- `g`: The input graph (weighted, directed).
- `start`: Source vertex index.
- `end`: Target vertex index.
- `k`: Maximum number of paths to find.

Returns:

- A vector of paths, where each path is a vector of vertex indices.
- Paths are sorted by total weight in ascending order.
- Returns fewer than `k` paths if fewer exist.
- Returns an empty vector if no path exists.

---

## 3. Yen's Algorithm

Yen's algorithm finds the K shortest loopless paths from a source to a target vertex. It was developed by Jin Y. Yen in 1971.

### How It Works

1. **Find the shortest path** using Dijkstra's algorithm. This becomes the first of the K paths.

2. **Generate candidate paths** by systematically deviating from previously found paths:
   - For each path already found, consider each vertex as a potential "spur node".
   - Temporarily remove edges that would lead to previously found paths.
   - Find the shortest path from the spur node to the target.
   - Combine the root path (from source to spur node) with this new spur path.

3. **Select the next shortest path** from the candidate pool.

4. **Repeat** until K paths are found or no more candidates exist.

### Key Properties

- **Loopless paths**: Each path visits every vertex at most once.
- **Optimal ordering**: Paths are guaranteed to be sorted by total weight.
- **Completeness**: Finds all K shortest paths if they exist.

---

## 4. Complexity

| Operation | Time Complexity |
|-----------|-----------------|
| `yen_k_shortest_paths` | O(K × N × (M + N log N)) |

Where:
- K = number of paths to find
- N = number of vertices
- M = number of edges

The algorithm runs Dijkstra's algorithm O(K × N) times in the worst case.

Space complexity: O(K × N) for storing the K paths.

---

## 5. Usage Examples

### Basic Usage

```cpp
#include <graphlib/graphlib.h>
#include <graphlib/k_shortest_paths.h>
#include <iostream>

int main() {
    graphlib::Graph g(5, true);  // 5 vertices, directed
    
    g.add_edge(0, 1, 1);
    g.add_edge(0, 2, 2);
    g.add_edge(1, 3, 2);
    g.add_edge(2, 3, 1);
    g.add_edge(1, 4, 4);
    g.add_edge(3, 4, 1);
    
    // Find 3 shortest paths from vertex 0 to vertex 4
    auto paths = graphlib::yen_k_shortest_paths(g, 0, 4, 3);
    
    for (size_t i = 0; i < paths.size(); ++i) {
        std::cout << "Path " << (i + 1) << ": ";
        for (int v : paths[i]) {
            std::cout << v << " ";
        }
        std::cout << std::endl;
    }
    
    return 0;
}
```

### Finding Alternative Routes

```cpp
#include <graphlib/graphlib.h>
#include <graphlib/k_shortest_paths.h>

// Find backup routes in a network
std::vector<std::vector<int>> find_backup_routes(
    const graphlib::Graph& network,
    int source,
    int destination,
    int num_alternatives
) {
    return graphlib::yen_k_shortest_paths(
        network, source, destination, num_alternatives
    );
}
```

### Calculating Path Weights

```cpp
#include <graphlib/graphlib.h>
#include <graphlib/k_shortest_paths.h>

long long calculate_path_weight(
    const graphlib::Graph& g,
    const std::vector<int>& path
) {
    long long total = 0;
    for (size_t i = 0; i + 1 < path.size(); ++i) {
        // Get edge weight between consecutive vertices
        for (const auto& [neighbor, weight] : g.neighbors(path[i])) {
            if (neighbor == path[i + 1]) {
                total += weight;
                break;
            }
        }
    }
    return total;
}

int main() {
    graphlib::Graph g(4, true);
    g.add_edge(0, 1, 5);
    g.add_edge(0, 2, 3);
    g.add_edge(1, 3, 2);
    g.add_edge(2, 3, 4);
    
    auto paths = graphlib::yen_k_shortest_paths(g, 0, 3, 2);
    
    for (const auto& path : paths) {
        std::cout << "Weight: " << calculate_path_weight(g, path) << std::endl;
    }
    
    return 0;
}
```

---

## 6. Use Cases

- **Network routing**: Finding primary and backup paths in communication networks.
- **Transportation**: Offering alternative routes in navigation systems.
- **Supply chain**: Identifying multiple shipping routes for redundancy.
- **Game AI**: Providing varied movement options for NPCs.
- **Resource allocation**: Finding multiple feasible solutions ranked by cost.

---

## 7. Limitations and Considerations

- **No negative weights**: The algorithm relies on Dijkstra internally, so all edge weights must be non-negative.
- **Directed graphs**: Designed for directed graphs. For undirected graphs, each undirected edge should be represented as two directed edges.
- **Large K values**: Performance degrades with large K; consider if you truly need many alternative paths.
- **Dense graphs**: On very dense graphs, the algorithm may be slow due to repeated Dijkstra calls.

---

## 8. Related Algorithms

- **Dijkstra**: For finding the single shortest path (`shortest_path.h`).
- **Bellman-Ford**: For graphs with negative weights (`shortest_path.h`).
- **A***: For heuristic-guided single-pair shortest path (`shortest_path.h`).
