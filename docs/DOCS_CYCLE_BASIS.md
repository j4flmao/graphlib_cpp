# Cycle Basis Algorithms

## Overview

This module provides algorithms for computing cycle bases and cycle-related operations in graphs.

## Header

```cpp
#include <graphlib/cycle_basis.h>
```

## API Reference

### Minimum Cycle Basis

```cpp
std::vector<Cycle> minimum_cycle_basis(const Graph& g);
```

Computes the Minimum Cycle Basis of an undirected graph using Horton's Algorithm.

A cycle basis is a set of cycles that can generate all cycles through XOR operations. The minimum cycle basis minimizes the total weight of all cycles.

**Complexity**: O(m³n) where m = edges, n = vertices

**Example**:
```cpp
Graph g(5, false);
g.add_edge(0, 1, 1);
g.add_edge(1, 2, 1);
g.add_edge(2, 3, 1);
g.add_edge(3, 0, 1);
g.add_edge(1, 3, 2);

auto basis = graphlib::minimum_cycle_basis(g);
for (const auto& cycle : basis) {
    std::cout << "Cycle weight: " << cycle.weight << "\n";
}
```

### Fundamental Cycles

```cpp
std::vector<Cycle> fundamental_cycles(const Graph& g);
```

Computes fundamental cycles from a spanning tree. For each non-tree edge, the fundamental cycle is the unique cycle formed by adding that edge to the tree.

### Girth

```cpp
int girth(const Graph& g);
```

Finds the length of the shortest cycle in the graph.

**Returns**: The girth value, or -1 if the graph is acyclic.

### Find All Cycles

```cpp
std::vector<std::vector<int>> find_all_cycles(const Graph& g, int max_cycles = 1000);
```

Finds all simple cycles in the graph using Johnson's algorithm.

**Warning**: Can be exponential in output size. Use `max_cycles` to limit.

### Count Cycles

```cpp
long long count_cycles(const Graph& g, int max_length);
```

Counts simple cycles up to a given length.

### Longest Cycle

```cpp
std::vector<int> longest_cycle(const Graph& g);
```

Finds the longest simple cycle. For small graphs (n ≤ 20), uses exact algorithm. For larger graphs, uses heuristics.

## Data Structures

### Cycle

```cpp
struct Cycle {
    std::vector<std::pair<int, int>> edges;  // Edges as (u, v) pairs
    long long weight;  // Sum of edge weights
};
```
