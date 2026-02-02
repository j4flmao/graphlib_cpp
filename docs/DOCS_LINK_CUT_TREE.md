# Link-Cut Tree

## Overview

A **Link-Cut Tree** is a dynamic tree data structure that maintains a forest of rooted trees and supports path queries and dynamic connectivity operations. It was introduced by Sleator and Tarjan in 1983.

Unlike static tree structures, Link-Cut Trees allow you to:
- **Link** two trees by adding an edge
- **Cut** an edge to split a tree into two
- **Query paths** between any two nodes (sum, max, etc.)

All operations run in **O(log N) amortized time complexity**.

### How It Works

The data structure represents the forest using **Splay Trees**. Each tree in the forest is decomposed into "preferred paths" — chains of nodes where each node's preferred child is the most recently accessed descendant. The `access(u)` operation restructures the tree so that the path from the root to `u` becomes a single preferred path, enabling efficient path queries.

## API Reference

### Constructor

```cpp
explicit LinkCutTree(int n);
```
Creates a Link-Cut Tree with `n` isolated nodes (indexed 0 to n-1).

### Destructor

```cpp
~LinkCutTree();
```

### Node Value Operations

```cpp
void set_value(int u, long long val);
```
Sets the value of node `u`.

```cpp
long long get_value(int u);
```
Returns the value of node `u`.

### Path Queries

```cpp
long long query_path_sum(int u, int v);
```
Returns the **sum** of all values on the path from `u` to `v`.

```cpp
long long query_path_max(int u, int v);
```
Returns the **maximum** value on the path from `u` to `v`.

### Tree Modification

```cpp
void link(int u, int v);
```
Adds an edge between `u` and `v`, connecting two separate trees.
- **Precondition**: `u` and `v` must be in different trees, and `u` should be the root of its tree (use `make_root()` first if needed).

```cpp
void cut(int u, int v);
```
Removes the edge between `u` and `v`, splitting one tree into two.

### Connectivity & Structure

```cpp
bool is_connected(int u, int v);
```
Returns `true` if `u` and `v` are in the same tree.

```cpp
int find_root(int u);
```
Returns the root of the tree containing node `u`.

```cpp
void make_root(int u);
```
Makes `u` the root of its tree by reversing the path from `u` to the original root.

```cpp
void access(int u);
```
Exposes the path from the tree root to `u` in the underlying splay tree. This is the core operation used internally by other methods.

## Complexity

| Operation | Time Complexity |
|-----------|-----------------|
| `set_value` | O(log N) amortized |
| `get_value` | O(log N) amortized |
| `query_path_sum` | O(log N) amortized |
| `query_path_max` | O(log N) amortized |
| `link` | O(log N) amortized |
| `cut` | O(log N) amortized |
| `is_connected` | O(log N) amortized |
| `find_root` | O(log N) amortized |
| `make_root` | O(log N) amortized |
| `access` | O(log N) amortized |

**Space Complexity**: O(N)

## Usage Examples

### Basic Tree Construction

```cpp
#include <graphlib/link_cut_tree.h>

graphlib::LinkCutTree lct(5);

// Set node values
lct.set_value(0, 10);
lct.set_value(1, 20);
lct.set_value(2, 30);
lct.set_value(3, 15);
lct.set_value(4, 25);

// Build a tree: 0 - 1 - 2
//                   |
//                   3 - 4
lct.make_root(0);
lct.link(0, 1);

lct.make_root(2);
lct.link(2, 1);

lct.make_root(3);
lct.link(3, 1);

lct.make_root(4);
lct.link(4, 3);
```

### Path Queries

```cpp
// Query sum on path from 0 to 2 (goes through 1)
long long sum = lct.query_path_sum(0, 2);  // 10 + 20 + 30 = 60

// Query max on path from 4 to 0 (goes through 3, 1)
long long max_val = lct.query_path_max(4, 0);  // max(25, 15, 20, 10) = 25
```

### Dynamic Connectivity

```cpp
// Check connectivity
bool connected = lct.is_connected(0, 4);  // true

// Cut the edge between 1 and 3
lct.cut(1, 3);

// Now 0,1,2 are in one tree and 3,4 are in another
connected = lct.is_connected(0, 4);  // false
connected = lct.is_connected(0, 2);  // true
connected = lct.is_connected(3, 4);  // true

// Re-link the trees differently
lct.make_root(3);
lct.link(3, 2);  // Now 3 is connected to 2 instead of 1
```

### Finding Tree Root

```cpp
lct.make_root(1);
int root = lct.find_root(4);  // Returns 1 (after make_root(1))
```

## Applications

### 1. Dynamic Connectivity

Link-Cut Trees efficiently answer connectivity queries in a dynamically changing forest:

```cpp
graphlib::LinkCutTree forest(n);

// Process edge insertions and deletions
void add_edge(int u, int v) {
    if (!forest.is_connected(u, v)) {
        forest.make_root(u);
        forest.link(u, v);
    }
}

void remove_edge(int u, int v) {
    forest.cut(u, v);
}

bool query_connected(int u, int v) {
    return forest.is_connected(u, v);
}
```

### 2. Lowest Common Ancestor (LCA) Queries

While this implementation focuses on path queries, Link-Cut Trees can support LCA queries by accessing both nodes and finding where their paths meet:

```cpp
// After access(u), the path from root to u is exposed
// access(v) will then expose path to v, and LCA can be found
// at the point where paths diverge
```

### 3. Path Aggregation in Dynamic Trees

Useful for problems requiring aggregate statistics on tree paths that change over time:

```cpp
// Example: Maximum edge weight on path in a dynamic graph
// Store edge weights at the deeper endpoint
lct.set_value(node_id, edge_weight);
long long max_on_path = lct.query_path_max(u, v);
```

### 4. Network Flow and Minimum Cost Algorithms

Link-Cut Trees accelerate algorithms like:
- **Maximum flow** (finding augmenting paths)
- **Minimum spanning tree** maintenance
- **Dynamic tree problems** in competitive programming

### 5. Maintaining Forest Structure

Track and modify tree structures efficiently:

```cpp
// Rerooting operations
lct.make_root(new_root);

// Tree structure queries
int tree_root = lct.find_root(node);
```

## Notes

- Nodes are 0-indexed (valid indices: 0 to n-1)
- Before calling `link(u, v)`, ensure `u` is the root of its tree using `make_root(u)`
- The `cut(u, v)` operation assumes an edge exists between `u` and `v`
- Path queries include both endpoints
