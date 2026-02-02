# NP-Hard Problems

## Overview
This module contains solutions and approximations for classic NP-Hard graph problems.

## Header
```cpp
#include <graphlib/np_hard.h>
```

## Algorithms

### Traveling Salesperson Problem (TSP)

1.  **Exact (Bitmask DP)**:
    *   `long long tsp_bitmask(const Graph& g)`
    *   Finds the minimum cost Hamiltonian Cycle.
    *   **Complexity**: $O(n^2 2^n)$.
    *   **Limit**: $N \le 20$. Returns -1 if $N > 20$.

2.  **Metric Approx (MST)**:
    *   `std::pair<long long, std::vector<int>> tsp_metric_approx(const Graph& g)`
    *   **Ratio**: 2-approximation.
    *   **Requirement**: Metric graph (triangle inequality).

3.  **Christofides Algorithm**:
    *   `std::pair<long long, std::vector<int>> tsp_christofides(const Graph& g)`
    *   **Ratio**: 1.5-approximation.
    *   **Requirement**: Metric graph.
    *   **Method**: MST + Minimum Weight Perfect Matching on odd-degree vertices + Eulerian Tour.

### Chinese Postman Problem
*   `long long chinese_postman(const Graph& g)`
*   Finds the minimum length closed walk traversing every edge at least once.
*   **Complexity**: Polynomial (uses Matching).

### Maximum Independent Set
*   `int max_independent_set(const Graph& g)`
*   Finds size of Max Independent Set (uses Max Clique on complement).

### Maximum Cut Approximation
*   `std::pair<long long, std::vector<int>> max_cut_approx(const Graph& g)`
*   Approximates the Max Cut (partition of vertices into two sets maximizing weights of edges between them).
*   **Method**: Greedy local search.
*   **Ratio**: 0.5-approximation.

### Feedback Vertex Set Approximation
*   `std::vector<int> feedback_vertex_set_approx(const Graph& g)`
*   Finds a subset of vertices whose removal makes the graph acyclic (a forest).
*   **Method**: Greedy heuristic (iterative cycle finding and max-degree node removal).


## Example: TSP
```cpp
#include <graphlib/graphlib.h>
#include <graphlib/np_hard.h>
#include <iostream>

int main() {
    // Complete graph with weights
    graphlib::Graph g(4);
    // ... add edges ...
    
    // Exact
    long long cost = graphlib::tsp_bitmask(g);
    
    // Approx
    auto result = graphlib::tsp_christofides(g);
    std::cout << "Approx Cost: " << result.first << "\n";
}
```
