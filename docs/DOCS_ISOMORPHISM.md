# Graph Isomorphism

This module provides algorithms to check if two graphs are isomorphic (structurally identical) or if one is a subgraph of the other.

Header:
```cpp
#include <graphlib/isomorphism.h>
```

## 1. VF2 Algorithm

The library implements the **VF2 Algorithm**, which is effective for large graphs, especially sparse ones. It works for both directed and undirected graphs.

### 1.1 Graph Isomorphism
Checks if Graph A is isomorphic to Graph B.

```cpp
bool iso = graphlib::is_isomorphic(g1, g2);
```

### 1.2 Subgraph Isomorphism
Checks if Graph A is isomorphic to a subgraph of Graph B (monomorphism).

```cpp
bool sub_iso = graphlib::is_subgraph_isomorphic(small_g, large_g);
```

## 2. Weisfeiler-Lehman Test (Hash-based)

For a faster (but probabilistic/heuristic) check, you can use the Weisfeiler-Lehman (WL) graph hashing from `graph_measures.h`. If the hashes differ, the graphs are definitely NOT isomorphic. If they are the same, they represent a high probability of isomorphism (or at least structural similarity).

## Example

```cpp
#include <graphlib/isomorphism.h>
#include <graphlib/graph_core.h>

graphlib::Graph g1(3, false);
g1.add_edge(0, 1);
g1.add_edge(1, 2);

graphlib::Graph g2(3, false);
g2.add_edge(1, 2);
g2.add_edge(2, 0);

// g1: 0-1-2
// g2: 1-2-0 (same structure, just relabeled)

if (graphlib::is_isomorphic(g1, g2)) {
    std::cout << "Graphs are isomorphic!" << std::endl;
}
```
