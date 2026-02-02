# Tree Isomorphism

## Overview
Efficiently checks if two **rooted or unrooted trees** are isomorphic.
- **Complexity**: `O(N log N)` or potentially `O(N)` with hash maps (implementation uses sorting `O(N log N)`).
- Much faster than general graph isomorphism (`VF2` which is exponential in worst case).

## Header
```cpp
#include <graphlib/isomorphism.h>
```

## Function

```cpp
/**
 * @brief Checks if two trees are isomorphic.
 * @param t1 First tree.
 * @param t2 Second tree.
 * @return true if isomorphic.
 */
bool is_tree_isomorphic(const Graph& t1, const Graph& t2);
```

## Algorithm (AHU)
The implementation follows the **Aho-Hopcroft-Ullman** algorithm (or similar canonical labeling):
1.  Find the **center(s)** of the tree. A tree has 1 or 2 centers.
2.  Root the tree at the center(s).
3.  Compute a **canonical string/hash** for the rooted tree by recursively sorting child hashes.
    - `Hash(u) = "(" + SortedList(Hash(v) for v in children) + ")"`
4.  Compare canonical forms.

## Example

```cpp
#include <graphlib/graphlib.h>
#include <graphlib/isomorphism.h>
#include <iostream>

int main() {
    graphlib::Graph t1(3);
    t1.add_edge(0, 1); t1.add_edge(1, 0);
    t1.add_edge(1, 2); t1.add_edge(2, 1);
    
    graphlib::Graph t2(3);
    t2.add_edge(2, 0); t2.add_edge(0, 2);
    t2.add_edge(0, 1); t2.add_edge(1, 0);
    
    if (graphlib::is_tree_isomorphic(t1, t2)) {
        std::cout << "Isomorphic!\n";
    }
}
```
