# Min Cut Algorithms

## Overview

Min cut algorithms find the minimum set of edges (by total weight) that, when removed, disconnect a graph. There are two main variants:

| Variant | Description |
|---------|-------------|
| **Global Min Cut** | Finds the minimum cut that separates the graph into any two non-empty parts |
| **S-T Min Cut** | Finds the minimum cut that separates two specific vertices s and t |

This library provides:
- **Stoer-Wagner Algorithm** — Computes the global minimum cut in O(V·E + V² log V)
- **Gomory-Hu Tree** — Precomputes all pairwise s-t min cuts efficiently

---

## Stoer-Wagner Algorithm

The Stoer-Wagner algorithm computes the **global minimum cut** of an undirected weighted graph. It works by repeatedly finding minimum s-t cuts and contracting vertices until only two vertices remain.

### How It Works

1. Start with all vertices in the graph
2. Perform a "maximum adjacency search" to find the two most connected vertices (s, t)
3. Record the cut-of-the-phase (edges incident to t)
4. Contract s and t into a single vertex
5. Repeat until 2 vertices remain
6. Return the minimum cut found across all phases

### API Reference

```cpp
namespace graphlib {

// Computes the global minimum cut using Stoer-Wagner algorithm.
// Returns the total weight of the minimum cut.
// Throws std::invalid_argument if the graph is directed.
GRAPHLIB_API long long global_min_cut_undirected(const Graph& g);

}
```

### Usage Example

```cpp
#include <graphlib/min_cut.h>
#include <graphlib/graph_core.h>
#include <iostream>

int main() {
    graphlib::Graph g(4, false); // 4 vertices, undirected
    
    // Build a simple graph
    g.add_edge(0, 1, 2);
    g.add_edge(0, 2, 3);
    g.add_edge(1, 2, 1);
    g.add_edge(1, 3, 3);
    g.add_edge(2, 3, 2);
    
    long long min_cut = graphlib::global_min_cut_undirected(g);
    std::cout << "Global minimum cut: " << min_cut << std::endl;
    
    return 0;
}
```

### Complexity

| Operation | Time Complexity | Space Complexity |
|-----------|-----------------|------------------|
| `global_min_cut_undirected` | O(V·E + V² log V) | O(V + E) |

---

## Gomory-Hu Tree

The **Gomory-Hu Tree** is a weighted tree that encodes the minimum s-t cut values for all pairs of vertices in an undirected graph. Instead of computing O(V²) individual min cuts, the Gomory-Hu tree computes all of them with only O(V) max-flow computations.

### Key Properties

- The tree has the same vertices as the original graph
- Each edge in the tree represents the min cut value between its endpoints
- The minimum s-t cut for any pair (s, t) equals the **minimum edge weight** on the path from s to t in the tree

### API Reference

```cpp
namespace graphlib {

// Computes the Gomory-Hu Tree for an undirected weighted graph.
// Throws std::invalid_argument if the graph is directed.
//
// Output parameters:
//   parent:  parent[i] is the parent of vertex i in the tree (root has parent = root)
//   min_cut: min_cut[i] is the weight of edge (i, parent[i]), representing
//            the minimum cut value between vertex i and its parent
GRAPHLIB_API void gomory_hu_tree(
    const Graph& g,
    std::vector<int>& parent,
    std::vector<long long>& min_cut
);

}
```

### Usage Example

```cpp
#include <graphlib/min_cut.h>
#include <graphlib/graph_core.h>
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    graphlib::Graph g(5, false); // 5 vertices, undirected
    
    g.add_edge(0, 1, 3);
    g.add_edge(0, 2, 2);
    g.add_edge(1, 2, 2);
    g.add_edge(1, 3, 4);
    g.add_edge(2, 4, 3);
    g.add_edge(3, 4, 1);
    
    std::vector<int> parent;
    std::vector<long long> min_cut;
    
    graphlib::gomory_hu_tree(g, parent, min_cut);
    
    // Print the Gomory-Hu tree structure
    std::cout << "Gomory-Hu Tree:" << std::endl;
    for (int i = 0; i < 5; ++i) {
        std::cout << "  Vertex " << i << " -> Parent " << parent[i]
                  << " (min cut = " << min_cut[i] << ")" << std::endl;
    }
    
    return 0;
}

// Query minimum s-t cut using the Gomory-Hu tree:
long long query_min_cut(int s, int t,
                        const std::vector<int>& parent,
                        const std::vector<long long>& min_cut) {
    // Find minimum edge weight on path from s to t
    // (Implementation depends on tree traversal)
    long long result = LLONG_MAX;
    
    // Trace path from s to root, marking visited
    std::vector<bool> visited(parent.size(), false);
    for (int v = s; !visited[v]; v = parent[v]) {
        visited[v] = true;
        if (v == parent[v]) break; // reached root
    }
    
    // Trace from t to first visited node, tracking min
    for (int v = t; !visited[v]; v = parent[v]) {
        result = std::min(result, min_cut[v]);
    }
    
    // Continue from s towards that meeting point
    for (int v = s; !visited[v] || v == s; v = parent[v]) {
        if (visited[v] && v != s) break;
        result = std::min(result, min_cut[v]);
        if (v == parent[v]) break;
    }
    
    return result;
}
```

### Complexity

| Operation | Time Complexity | Space Complexity |
|-----------|-----------------|------------------|
| `gomory_hu_tree` construction | O(V × MaxFlow) | O(V + E) |
| Query min s-t cut from tree | O(V) per query | O(1) |

Where MaxFlow is the complexity of the underlying max-flow algorithm (typically O(V²·E) for Dinic's algorithm).

---

## Comparison

| Algorithm | Use Case | Computes |
|-----------|----------|----------|
| Stoer-Wagner | Find the single global minimum cut | One cut value |
| Gomory-Hu Tree | Query many pairwise min cuts | All V² pairwise min cuts |

**When to use which:**
- Use **Stoer-Wagner** when you only need the global minimum cut
- Use **Gomory-Hu Tree** when you need to answer multiple s-t min cut queries

---

## Requirements

- Graph must be **undirected** (both functions throw `std::invalid_argument` for directed graphs)
- Edge weights should be non-negative
