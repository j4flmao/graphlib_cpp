# Tree Decomposition

## Overview

A **Tree Decomposition** of a graph $G = (V, E)$ is a mapping of $G$ into a tree $T$ where each node of $T$ (called a **bag**) contains a subset of vertices from $V$. This structure is crucial for defining the **treewidth** of a graph and solving many NP-hard problems efficiently on graphs with small treewidth.

The properties of a tree decomposition are:
1. **Vertex Coverage**: Every vertex $v \in V$ belongs to at least one bag.
2. **Edge Coverage**: For every edge $(u, v) \in E$, there is at least one bag containing both $u$ and $v$.
3. **Coherence (Running Intersection Property)**: If a vertex $v$ is present in bags $B_i$ and $B_j$, then $v$ is present in all bags on the path between $B_i$ and $B_j$ in $T$.

The **width** of a tree decomposition is $\max |B_i| - 1$. The **treewidth** of $G$ is the minimum width over all possible tree decompositions.

Finding the optimal tree decomposition is NP-hard. GraphLib provides a heuristic based on **Min-Degree Fill-in** (Elimination Game).

## API

### `graphlib::compute_tree_decomposition`

```cpp
TreeDecomposition compute_tree_decomposition(const Graph& g);
```

Computes a tree decomposition using the min-degree heuristic.

**Parameters:**
- `g`: The input graph.

**Returns:**
- `TreeDecomposition`: A struct containing:
    - `bags`: A vector of `Bag` structs (each has `id` and `vertices`).
    - `adj`: Adjacency list of the decomposition tree (indices refer to `bags` vector).
    - `width`: The width of the computed decomposition.

## Example

```cpp
#include <graphlib/graphlib.h>
#include <iostream>

using namespace graphlib;

int main() {
    // 0-1-2 (Path), Treewidth 1
    Graph g(3);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    
    TreeDecomposition td = compute_tree_decomposition(g);
    
    std::cout << "Decomposition Width: " << td.width << "\n";
    
    for (const auto& bag : td.bags) {
        std::cout << "Bag " << bag.id << ": ";
        for (int v : bag.vertices) std::cout << v << " ";
        std::cout << "\n";
    }
}
```
