# Directed Minimum Spanning Tree (Arborescence)

## Overview
The **Directed MST** problem (also known as Minimum Spanning Arborescence) finds a subgraph of a directed graph that:
1.  Has a designated root.
2.  Contains a path from root to every other vertex.
3.  Has minimal total edge weight.
4.  Has no cycles (it's a tree/arborescence).

## Algorithm
This implementation uses the **Chu-Liu / Edmonds Algorithm**.
- **Complexity**: `O(E log V)` or `O(VE)` depending on implementation details (this version is recursive `O(VE)` worst case but practical).
- **Features**: Supports **full edge reconstruction**, returning the list of edges forming the MST.

## Header
```cpp
#include <graphlib/directed_mst.h>
```

## Function

```cpp
struct DirectedEdge {
    int u, v;
    long long weight;
    int id; // User-defined ID, preserved in result
};

/**
 * @brief Computes the DMST rooted at 'root'.
 * @param n Number of vertices.
 * @param root The root vertex ID.
 * @param edges List of directed edges.
 * @param result_edges Output vector filled with IDs of selected edges.
 * @return Total weight of the MST, or -1 if impossible (root cannot reach all nodes).
 */
long long directed_mst(int n, int root, const std::vector<DirectedEdge>& edges, std::vector<int>& result_edges);
```

## Example

```cpp
#include <graphlib/graphlib.h>
#include <graphlib/directed_mst.h>
#include <iostream>
#include <vector>

int main() {
    int n = 4;
    int root = 0;
    std::vector<graphlib::DirectedEdge> edges;
    
    // 0 -> 1 (10)
    // 0 -> 2 (10)
    // 1 -> 2 (5)
    // 2 -> 3 (5)
    // 1 -> 3 (20)
    // Cycle 2->1 (4)? No let's make it simple acyclic first or cyclic
    
    edges.push_back({0, 1, 10, 0});
    edges.push_back({0, 2, 10, 1});
    edges.push_back({1, 2, 5, 2});
    edges.push_back({2, 3, 5, 3});
    edges.push_back({1, 3, 20, 4});
    
    std::vector<int> result;
    long long cost = graphlib::directed_mst(n, root, edges, result);
    
    std::cout << "Cost: " << cost << "\n"; // Expected 10+5+5=20
    std::cout << "Edges: ";
    for(int id : result) std::cout << id << " ";
    std::cout << "\n";
}
```
