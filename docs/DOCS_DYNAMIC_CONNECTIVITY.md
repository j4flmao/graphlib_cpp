# Dynamic Connectivity

## Overview

The `DynamicConnectivity` class implements **offline dynamic connectivity** using a segment tree over time combined with a rollback-enabled Disjoint Set Union (DSU) data structure.

**Offline** means all operations (edge additions, removals, and connectivity queries) must be specified upfront before calling `solve()`, which processes them all at once and returns the query results.

This is useful for scenarios where you need to answer connectivity queries while edges are being added and removed over time, but you know all operations in advance.

## API Reference

### Header

```cpp
#include <graphlib/dynamic_connectivity.h>
```

### Class: `graphlib::DynamicConnectivity`

#### Constructor

```cpp
explicit DynamicConnectivity(int n);
```

Creates a dynamic connectivity structure for a graph with `n` vertices (labeled `0` to `n-1`). Initially, the graph has no edges.

#### Methods

| Method | Description |
|--------|-------------|
| `void add_edge(int u, int v)` | Add an undirected edge between vertices `u` and `v`. If the edge already exists, this is a no-op. |
| `void remove_edge(int u, int v)` | Remove the edge between vertices `u` and `v`. If the edge doesn't exist, this is a no-op. |
| `void query(int u, int v)` | Register a connectivity query asking whether `u` and `v` are connected at the current point in time. |
| `std::vector<bool> solve()` | Process all operations and return the results of all queries in the order they were issued. |

### Usage Pattern

1. Create a `DynamicConnectivity` object with the number of vertices.
2. Interleave `add_edge()`, `remove_edge()`, and `query()` calls to describe the sequence of operations.
3. Call `solve()` to get all query results.

**Important:** After calling `solve()`, the object should not be reused.

## Complexity Analysis

| Operation | Time Complexity |
|-----------|-----------------|
| `add_edge(u, v)` | O(log Q) amortized |
| `remove_edge(u, v)` | O(log Q) |
| `query(u, v)` | O(1) |
| `solve()` | O((E + Q) · log Q · α(n)) |

Where:
- **Q** = total number of queries
- **E** = total number of edge insertions
- **n** = number of vertices
- **α(n)** = inverse Ackermann function (effectively constant)

**Space Complexity:** O(E · log Q + Q)

The segment tree has O(Q) nodes, and each edge interval is stored in O(log Q) nodes.

## Usage Examples

### Basic Example

```cpp
#include <graphlib/dynamic_connectivity.h>
#include <iostream>

int main() {
    graphlib::DynamicConnectivity dc(5);
    
    // Add edges
    dc.add_edge(0, 1);
    dc.add_edge(1, 2);
    
    // Query: Are 0 and 2 connected?
    dc.query(0, 2);  // Query 0
    
    // Remove an edge
    dc.remove_edge(1, 2);
    
    // Query: Are 0 and 2 still connected?
    dc.query(0, 2);  // Query 1
    
    // Add a different path
    dc.add_edge(2, 3);
    dc.add_edge(3, 0);
    
    // Query: Are 0 and 2 connected now?
    dc.query(0, 2);  // Query 2
    
    // Solve all queries
    std::vector<bool> results = dc.solve();
    
    std::cout << "Query 0: " << (results[0] ? "connected" : "not connected") << "\n";
    std::cout << "Query 1: " << (results[1] ? "connected" : "not connected") << "\n";
    std::cout << "Query 2: " << (results[2] ? "connected" : "not connected") << "\n";
    
    return 0;
}
```

**Output:**
```
Query 0: connected
Query 1: not connected
Query 2: connected
```

### Tracking Component Count Over Time

```cpp
#include <graphlib/dynamic_connectivity.h>
#include <vector>

// Simulate network connectivity changes
void simulate_network() {
    const int NUM_SERVERS = 100;
    graphlib::DynamicConnectivity dc(NUM_SERVERS);
    
    // Time 0: Initial connections
    dc.add_edge(0, 1);
    dc.add_edge(1, 2);
    dc.add_edge(2, 3);
    dc.query(0, 3);  // Are server 0 and 3 connected?
    
    // Time 1: Link failure
    dc.remove_edge(1, 2);
    dc.query(0, 3);  // Still connected after failure?
    
    // Time 2: Backup link activated
    dc.add_edge(0, 3);
    dc.query(0, 3);  // Connected via backup?
    
    auto results = dc.solve();
    // results[0] = true  (connected via 0-1-2-3)
    // results[1] = false (link broken, no alternate path)
    // results[2] = true  (connected via direct 0-3 link)
}
```

## Algorithm Description

The offline dynamic connectivity algorithm works by leveraging a **segment tree over time** combined with a **rollback DSU**.

### Key Insight

Each edge exists for a contiguous interval of time `[t_add, t_remove)`. By building a segment tree where each leaf represents a query time point, we can decompose each edge's lifetime into O(log Q) nodes in the tree.

### Algorithm Steps

1. **Collection Phase**: Record all `add_edge`, `remove_edge`, and `query` operations with their timestamps.

2. **Interval Construction**: For each edge, compute its active interval `[start, end)`:
   - `start` = timestamp when `add_edge` was called
   - `end` = timestamp when `remove_edge` was called (or end of all operations if never removed)

3. **Segment Tree Construction**: Build a segment tree over the Q query time points. Each edge interval is added to all segment tree nodes that it fully covers (similar to segment tree range updates).

4. **DFS Traversal with Rollback**: Traverse the segment tree using DFS:
   - When entering a node, unite all edges stored in that node using the rollback DSU.
   - At leaf nodes, answer all queries for that time point.
   - When leaving a node, rollback the DSU to the state before entering.

### Why Rollback DSU?

Standard DSU with path compression doesn't support undo operations. The rollback DSU uses **union by rank without path compression**, allowing O(log n) find operations while maintaining the ability to rollback to previous states.

### Visual Representation

```
Time:        0       1       2       3
             |-------|-------|-------|
Edge (0,1):  [===============]
Edge (1,2):  [=======]
Edge (2,3):          [===============]

Segment Tree:
              [0, 3]
             /      \
        [0,1]        [2,3]
        /   \        /   \
      [0]   [1]    [2]   [3]  <- Queries answered at leaves
```

Edges are stored at the highest possible nodes covering their intervals, and the DFS traversal ensures correct connectivity state at each query point.
