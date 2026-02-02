# Centroid Decomposition

## Overview
**Centroid Decomposition** decomposes a tree into a hierarchy of centroids.
- **Properties**: The height of the centroid tree is at most `O(log N)`.
- **Applications**: Analyzing paths, divide and conquer on trees (e.g., counting paths of length K).

## Header
```cpp
#include <graphlib/tree_centroid.h>
```

## Class `CentroidDecomposition`

### Constructor
```cpp
CentroidDecomposition cd(tree);
```
- Performs the decomposition immediately.
- Complexity: `O(N log N)`.

### Methods
- `int get_centroid_parent(int u)`: Returns the parent of `u` in the centroid tree. `-1` for the root.
- `int get_centroid_root()`: Returns the root of the centroid tree.

## Example

```cpp
#include <graphlib/graphlib.h>
#include <graphlib/tree_centroid.h>
#include <iostream>

int main() {
    // Path: 0-1-2-3-4
    graphlib::Graph g(5);
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 3); g.add_edge(3, 2);
    g.add_edge(3, 4); g.add_edge(4, 3);
    
    graphlib::CentroidDecomposition cd(g);
    
    int root = cd.get_centroid_root();
    std::cout << "Centroid Root: " << root << "\n"; // Likely 2
    
    std::cout << "Parent of 1: " << cd.get_centroid_parent(1) << "\n"; // Likely 2
}
```
