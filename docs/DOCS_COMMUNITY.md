# Community Detection

## Overview

Community detection involves identifying groups of vertices (communities) that are more densely connected internally than with the rest of the graph.

## Header

```cpp
#include <graphlib/community.h>
```

## Louvain Method

The Louvain Method is a heuristic algorithm for modularity optimization. It extracts communities from large networks and is known for its speed and quality.

### Algorithm Description

The algorithm proceeds in two alternating phases:
1.  **Modularity Optimization**: Each node is moved to the community of one of its neighbors that yields the maximum increase in modularity. This repeats until no move improves modularity.
2.  **Aggregation**: A new graph is built where nodes are the communities found in the first phase. Internal edges become self-loops, and edges between communities are aggregated.

These phases repeat until the graph cannot be aggregated further.

### Function

```cpp
/**
 * @brief Detects communities using the Louvain Method.
 * 
 * @param g The input graph (undirected).
 * @return std::vector<int> Result where result[u] is the community ID of vertex u.
 */
std::vector<int> louvain_communities(const Graph& g);
```

### Complexity

- Empirical complexity is roughly **O(N log N)** or **O(M)** on sparse graphs.
- Memory usage is **O(N + M)**.

## K-Truss Decomposition

A **k-truss** is a subgraph where every edge participates in at least `k-2` triangles within the subgraph. It is a stricter definition of cohesiveness than k-core.

### Function

```cpp
/**
 * @brief Computes the K-Truss Decomposition.
 * @return Map from edge (u, v) [u < v] to its trussness value (max k such that edge is in k-truss).
 */
std::map<std::pair<int, int>, int> k_truss_decomposition(const Graph& g);
```

### Complexity
- Approximately **O(m^1.5)**.


### Example

```cpp
#include <graphlib/graphlib.h>
#include <graphlib/community.h>
#include <iostream>

int main() {
    // Create a graph with two cliques connected by a single edge
    graphlib::Graph g(6);
    // Clique 1: 0, 1, 2
    g.add_edge(0, 1); g.add_edge(1, 2); g.add_edge(2, 0);
    
    // Clique 2: 3, 4, 5
    g.add_edge(3, 4); g.add_edge(4, 5); g.add_edge(5, 3);
    
    // Bridge
    g.add_edge(2, 3);
    
    auto communities = graphlib::louvain_communities(g);
    
    for (int i = 0; i < 6; ++i) {
        std::cout << "Node " << i << " -> Community " << communities[i] << "\n";
    }
    // Expected: {0,1,2} in one comm, {3,4,5} in another.
}
```
