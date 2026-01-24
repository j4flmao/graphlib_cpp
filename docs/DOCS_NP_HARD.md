# NP-Hard Graph Problems

This module provides implementations for several classic NP-hard graph problems. While these problems are computationally expensive (often exponential time complexity), these implementations are optimized for small to medium-sized inputs or use standard heuristics/approximations.

## Overview

- **Traveling Salesperson Problem (TSP)**: Solved exactly using Bitmask Dynamic Programming.
- **Maximum Clique**: Solved exactly using the Bron-Kerbosch algorithm with pivoting.
- **Graph Coloring**: Solved approximately using a greedy strategy with Welsh-Powell heuristic.
- **Chinese Postman Problem**: Solved exactly (for undirected graphs) by transforming into a Minimum Weight Perfect Matching problem.

## API Reference

### 1. Traveling Salesperson Problem (TSP)

Finds the minimum cost to visit every vertex exactly once and return to the starting vertex.

- **Complexity**: $O(n^2 \cdot 2^n)$
- **Constraint**: $n \le 20$ (due to memory and time limits of bitmask DP).

```cpp
#include <graphlib/np_hard.h>

// Create a small graph (n <= 20)
graphlib::Graph g(4);
g.add_edge(0, 1, 10); g.add_edge(1, 0, 10);
g.add_edge(1, 2, 15); g.add_edge(2, 1, 15);
// ... add more edges

long long cost = graphlib::tsp_bitmask(g);
if (cost != -1) {
    std::cout << "Minimum TSP cost: " << cost << std::endl;
} else {
    std::cout << "No tour possible or n too large." << std::endl;
}
```

### 2. Maximum Clique

Finds the size of the largest subset of vertices where every two distinct vertices are adjacent.

- **Algorithm**: Bron-Kerbosch with pivoting.
- **Complexity**: Worst-case $O(3^{n/3})$, but efficient for many sparse graphs.

```cpp
#include <graphlib/np_hard.h>

graphlib::Graph g(10, false); // Undirected
// ... add edges

int max_clique_size = graphlib::max_clique_bron_kerbosch(g);
std::cout << "Max clique size: " << max_clique_size << std::endl;
```

### 3. Graph Coloring

Assigns colors to vertices such that no two adjacent vertices share the same color. Tries to minimize the number of colors used.

- **Algorithm**: Greedy strategy with Welsh-Powell heuristic (orders vertices by descending degree).
- **Complexity**: $O(V + E)$
- **Note**: This is a heuristic and does not guarantee the minimum possible number of colors (Chromatic Number), but usually provides a good approximation.

```cpp
#include <graphlib/np_hard.h>
#include <vector>

graphlib::Graph g(5, false);
// ... add edges

std::vector<int> colors;
int num_colors = graphlib::greedy_coloring(g, colors);

std::cout << "Used " << num_colors << " colors." << std::endl;
for(int i=0; i<5; ++i) {
    std::cout << "Vertex " << i << ": Color " << colors[i] << std::endl;
}
```

### 4. Chinese Postman Problem (Route Inspection)

Finds the minimum length closed walk that traverses every edge at least once.

- **Algorithm**: 
  1. Identify vertices with odd degrees.
  2. Find minimum weight perfect matching on the complete graph of odd-degree vertices (using pairwise shortest path distances).
  3. Add the matched paths to the original graph to make it Eulerian.
- **Complexity**: Dominated by All-Pairs Shortest Path and Matching. Approx $O(V^3)$ or $O(V \cdot E \log V)$ depending on implementation.
- **Constraint**: Graph must be connected.

```cpp
#include <graphlib/np_hard.h>

graphlib::Graph g(6, false);
// ... add weighted edges

long long min_route_len = graphlib::chinese_postman(g);
if (min_route_len != -1) {
    std::cout << "Minimum route length: " << min_route_len << std::endl;
}
```
