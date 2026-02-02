# Graph Isomorphism Algorithms

## Overview

Graph isomorphism determines whether two graphs are structurally identical — that is, whether there exists a one-to-one mapping between their vertices that preserves adjacency. Two graphs G₁ and G₂ are isomorphic if there exists a bijection f: V(G₁) → V(G₂) such that (u, v) ∈ E(G₁) if and only if (f(u), f(v)) ∈ E(G₂).

This module provides:
- **Graph Isomorphism**: Check if two graphs have identical structure
- **Subgraph Isomorphism**: Check if one graph is contained within another
- **Tree Isomorphism**: Specialized fast algorithm for tree structures

## API Reference

### `is_isomorphic`

```cpp
bool is_isomorphic(const Graph& pattern, const Graph& target, std::vector<int>* mapping = nullptr);
```

Checks if `pattern` is isomorphic to `target`.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `pattern` | `const Graph&` | First graph to compare |
| `target` | `const Graph&` | Second graph to compare |
| `mapping` | `std::vector<int>*` | Optional output mapping (pattern_node → target_node) |

**Returns:** `true` if the graphs are isomorphic, `false` otherwise.

**Time Complexity:** O(N! × N) worst case, but typically much faster with VF2 pruning.

---

### `is_subgraph_isomorphic`

```cpp
bool is_subgraph_isomorphic(const Graph& pattern, const Graph& target, std::vector<int>* mapping = nullptr);
```

Checks if `pattern` is isomorphic to a subgraph of `target`. The pattern graph must be embeddable within the target graph.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `pattern` | `const Graph&` | The smaller pattern graph to find |
| `target` | `const Graph&` | The larger graph to search within |
| `mapping` | `std::vector<int>*` | Optional output mapping (pattern_node → target_node) |

**Returns:** `true` if the pattern exists as a subgraph in target, `false` otherwise.

**Time Complexity:** O(N! × N) worst case (NP-complete problem).

---

### `find_all_subgraph_isomorphisms`

```cpp
std::vector<std::vector<int>> find_all_subgraph_isomorphisms(const Graph& pattern, const Graph& target);
```

Finds all occurrences of `pattern` within `target`.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `pattern` | `const Graph&` | The pattern graph to find |
| `target` | `const Graph&` | The graph to search within |

**Returns:** A vector of all mappings, where each mapping is a `std::vector<int>` representing pattern_node → target_node.

**Time Complexity:** O(N! × N × M) where M is the number of matches.

---

### `is_tree_isomorphic`

```cpp
bool is_tree_isomorphic(const Graph& t1, const Graph& t2);
```

Specialized isomorphism check for trees. Much faster than general graph isomorphism.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `t1` | `const Graph&` | First tree (must be connected with n-1 edges) |
| `t2` | `const Graph&` | Second tree (must be connected with n-1 edges) |

**Returns:** `true` if the trees are isomorphic, `false` otherwise.

**Time Complexity:** O(N log N) or O(N) depending on implementation.

**Preconditions:** Both graphs must be valid trees (connected, exactly n-1 edges).

---

## Algorithm Descriptions

### VF2 Algorithm (General Graph Isomorphism)

The VF2 algorithm is a state-space search algorithm that efficiently explores possible node mappings using feasibility rules to prune the search space.

**Key concepts:**
1. **State**: A partial mapping between pattern and target nodes
2. **Candidate pairs**: Nodes that can potentially extend the current mapping
3. **Feasibility rules**: Consistency checks that prune invalid branches early

**Feasibility rules include:**
- **Rpred**: Predecessor consistency (for directed graphs)
- **Rsucc**: Successor consistency (for directed graphs)
- **Rin/Rout**: Connectivity to already-mapped nodes
- **Rnew**: Future connectivity constraints

The algorithm backtracks when no valid extension exists, making it efficient in practice despite exponential worst-case complexity.

### Tree Isomorphism (AHU Algorithm)

Tree isomorphism uses canonical form comparison based on the Aho-Hopcroft-Ullman (AHU) algorithm:

1. **Root selection**: Find the center(s) of both trees
2. **Canonical labeling**: Assign labels to nodes bottom-up based on children's labels
3. **Comparison**: Trees are isomorphic if their canonical forms match

For trees with one center, compare once. For trees with two centers (bicentral), try both possible root pairings.

---

## Usage Examples

### Basic Isomorphism Check

```cpp
#include <graphlib/isomorphism.h>
#include <graphlib/graph_core.h>
#include <iostream>

int main() {
    graphlib::Graph g1(4, false);  // 4-node undirected graph
    g1.add_edge(0, 1);
    g1.add_edge(1, 2);
    g1.add_edge(2, 3);
    g1.add_edge(3, 0);  // Square

    graphlib::Graph g2(4, false);
    g2.add_edge(0, 2);
    g2.add_edge(2, 1);
    g2.add_edge(1, 3);
    g2.add_edge(3, 0);  // Same square, different labeling

    std::vector<int> mapping;
    if (graphlib::is_isomorphic(g1, g2, &mapping)) {
        std::cout << "Graphs are isomorphic!\n";
        std::cout << "Mapping: ";
        for (int i = 0; i < mapping.size(); i++) {
            std::cout << i << "->" << mapping[i] << " ";
        }
        std::cout << "\n";
    }
    return 0;
}
```

### Finding Pattern in a Larger Graph

```cpp
#include <graphlib/isomorphism.h>
#include <graphlib/graph_core.h>
#include <iostream>

int main() {
    // Create a triangle pattern
    graphlib::Graph triangle(3, false);
    triangle.add_edge(0, 1);
    triangle.add_edge(1, 2);
    triangle.add_edge(2, 0);

    // Create a larger graph containing triangles
    graphlib::Graph target(5, false);
    target.add_edge(0, 1);
    target.add_edge(1, 2);
    target.add_edge(2, 0);  // First triangle
    target.add_edge(2, 3);
    target.add_edge(3, 4);
    target.add_edge(4, 2);  // Second triangle

    auto matches = graphlib::find_all_subgraph_isomorphisms(triangle, target);
    std::cout << "Found " << matches.size() << " triangles\n";
    
    for (const auto& match : matches) {
        std::cout << "Triangle: ";
        for (int node : match) {
            std::cout << node << " ";
        }
        std::cout << "\n";
    }
    return 0;
}
```

### Tree Isomorphism

```cpp
#include <graphlib/isomorphism.h>
#include <graphlib/graph_core.h>
#include <iostream>

int main() {
    // Create two binary trees with same structure
    graphlib::Graph tree1(7, false);
    tree1.add_edge(0, 1);
    tree1.add_edge(0, 2);
    tree1.add_edge(1, 3);
    tree1.add_edge(1, 4);
    tree1.add_edge(2, 5);
    tree1.add_edge(2, 6);

    graphlib::Graph tree2(7, false);
    tree2.add_edge(3, 0);
    tree2.add_edge(3, 6);
    tree2.add_edge(0, 1);
    tree2.add_edge(0, 2);
    tree2.add_edge(6, 4);
    tree2.add_edge(6, 5);

    if (graphlib::is_tree_isomorphic(tree1, tree2)) {
        std::cout << "Trees are isomorphic!\n";
    } else {
        std::cout << "Trees are NOT isomorphic.\n";
    }
    return 0;
}
```

---

## Complexity Summary

| Function | Time Complexity | Space Complexity |
|----------|-----------------|------------------|
| `is_isomorphic` | O(N! × N) worst, practical: O(N²) to O(N³) | O(N) |
| `is_subgraph_isomorphic` | O(N! × N) worst (NP-complete) | O(N) |
| `find_all_subgraph_isomorphisms` | O(N! × N × M) | O(N × M) |
| `is_tree_isomorphic` | O(N log N) or O(N) | O(N) |

Where N = number of nodes, M = number of matches found.

---

## Notes

- For general graphs, isomorphism testing is believed to be in NP but not NP-complete (quasi-polynomial algorithm exists).
- Subgraph isomorphism is NP-complete.
- Tree isomorphism is in P and can be solved efficiently.
- The VF2 algorithm performs well on sparse graphs and graphs with distinct degree sequences.
- Consider using `is_tree_isomorphic` when you know both inputs are trees for significant performance gains.
