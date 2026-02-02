# Link-Cut Tree

## Overview

A **Link-Cut Tree** is a dynamic data structure that maintains a forest of rooted trees. It supports the following operations in **O(log N)** amortized time:
- **Link(u, v)**: Add an edge between u and v (merging two trees).
- **Cut(u, v)**: Remove the edge between u and v (splitting a tree).
- **Connected(u, v)**: Check if u and v are in the same tree.
- **Path Query(u, v)**: Compute sum, max, etc., on the path between u and v.
- **Update(u, val)**: Update the value of a node.

It is implemented using **Splay Trees** (Heavy-Light Decomposition concept but dynamic).

## Header

```cpp
#include <graphlib/link_cut_tree.h>
```

## Class `LinkCutTree`

```cpp
explicit LinkCutTree(int n);
```

Initializes a forest of `n` isolated vertices.

### Operations

- `void link(int u, int v)`: Adds edge (u, v). If u is already connected to another parent or they are in the same tree, behavior depends on implementation (this implementation attempts to make u a child of v). PRECONDITION: u and v disjoint.
- `void cut(int u, int v)`: Removes edge (u, v). PRECONDITION: Edge exists.
- `bool is_connected(int u, int v)`: Returns true if connected.
- `int find_root(int u)`: Returns the root of the tree containing u.
- `void make_root(int u)`: Reroots the tree at u.
- `void set_value(int u, long long val)`: Sets node value.
- `long long query_path_sum(int u, int v)`: Returns sum of values on path u-v.
- `long long query_path_max(int u, int v)`: Returns max value on path u-v.

### Example

```cpp
#include <graphlib/graphlib.h>
#include <graphlib/link_cut_tree.h>
#include <iostream>
#include <cassert>

int main() {
    graphlib::LinkCutTree lct(5);
    
    lct.set_value(0, 10);
    lct.set_value(1, 20);
    lct.set_value(2, 30);
    
    // Link 0-1 and 1-2
    lct.link(0, 1);
    lct.link(1, 2);
    
    assert(lct.is_connected(0, 2));
    
    // Path sum 0-1-2: 10+20+30 = 60
    long long sum = lct.query_path_sum(0, 2);
    std::cout << "Sum: " << sum << "\n";
    
    // Cut 1-2
    lct.cut(1, 2);
    assert(!lct.is_connected(0, 2));
}
```
