# Advanced Data Structures

This module provides advanced data structures for complex graph problems and dynamic queries.

Header: `<graphlib/link_cut_tree.h>`, `<graphlib/dsu_rollback.h>`

## Link-Cut Tree

Implements a dynamic tree data structure that maintains a forest of rooted trees. It supports path queries and dynamic link/cut operations in O(log N) time.

### Class: `LinkCutTree`

#### Constructor
- `LinkCutTree(int n)`: Initializes a forest of `n` isolated vertices (0 to n-1).

#### Methods
- `void set_value(int u, long long val)`: Sets the value of vertex `u`.
- `long long get_value(int u)`: Returns the current value of vertex `u`.
- `long long query_path_sum(int u, int v)`: Returns the sum of values on the path between `u` and `v`.
- `long long query_path_max(int u, int v)`: Returns the maximum value on the path between `u` and `v`.
- `void link(int u, int v)`: Adds an edge between `u` and `v`. `u` becomes a child of `v` (if `u` is a root).
- `void cut(int u, int v)`: Removes the edge between `u` and `v`.
- `bool is_connected(int u, int v)`: Checks if `u` and `v` are in the same tree.
- `int find_root(int u)`: Returns the root of the tree containing `u`.
- `void make_root(int u)`: Makes `u` the root of its tree (rerooting).

### Example
```cpp
#include <graphlib/link_cut_tree.h>

graphlib::LinkCutTree lct(5);
lct.set_value(0, 10);
lct.set_value(1, 20);
lct.link(0, 1); // 0 is now connected to 1

long long sum = lct.query_path_sum(0, 1); // Returns 30
lct.cut(0, 1); // Disconnect
```

## DSU with Rollback

A variation of Disjoint Set Union (Union-Find) that supports snapshotting and rolling back operations. Useful for algorithms like "Dynamic Connectivity" or offline processing with divide-and-conquer.

### Class: `DsuRollback`

#### Constructor
- `DsuRollback(int n)`: Initializes `n` disjoint sets.

#### Methods
- `bool unite(int x, int y)`: Unites sets containing `x` and `y`. Returns `true` if merged. (Uses union by rank, no path compression to allow rollback).
- `int find(int x) const`: Finds the representative of `x`.
- `int snapshot()`: Saves the current state and returns a snapshot ID.
- `void rollback()`: Reverts the last operation (unite) since the last snapshot.
- `void rollback_to(int snapshot_id)`: Reverts operations until the state matches `snapshot_id`.
- `int component_count() const`: Returns the current number of connected components.
- `bool connected(int x, int y) const`: Checks if `x` and `y` are connected.

### Example
```cpp
#include <graphlib/dsu_rollback.h>

graphlib::DsuRollback dsu(5);
dsu.unite(0, 1);
int snap = dsu.snapshot();

dsu.unite(1, 2);
// 0, 1, 2 are connected
dsu.rollback(); 
// Back to state 'snap': 0-1 connected, 2 isolated
```
