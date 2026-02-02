# GraphLib – Clique Algorithms (`clique.h`)

This document covers clique detection algorithms:

- Maximum clique finding.
- Enumeration of all maximal cliques.

---

## 1. Overview

Header:

```cpp
#include <graphlib/clique.h>
```

A **clique** is a subset of vertices in an undirected graph where every two distinct vertices are adjacent (i.e., a complete subgraph).

Key concepts:

- **Maximal clique**: A clique that cannot be extended by adding any adjacent vertex.
- **Maximum clique**: The largest clique in the graph (a maximal clique with the most vertices).

The Maximum Clique problem is **NP-hard**, so the algorithms have exponential worst-case complexity. However, the Bron-Kerbosch algorithm with pivoting performs well on many practical graphs, especially dense ones.

---

## 2. API Reference

### 2.1 `maximum_clique`

```cpp
std::vector<int> maximum_clique(const Graph& g);
```

Finds **one** maximum clique in the graph.

| Parameter | Description |
|-----------|-------------|
| `g` | The input undirected graph |

**Returns**: A vector of vertex indices forming a maximum clique.

**Complexity**:
- **Time**: O(3^(n/3)) worst case, but typically much faster with pivoting
- **Space**: O(n) for recursion stack and working sets

---

### 2.2 `find_all_maximal_cliques`

```cpp
std::vector<std::vector<int>> find_all_maximal_cliques(const Graph& g);
```

Finds **all** maximal cliques in the graph.

| Parameter | Description |
|-----------|-------------|
| `g` | The input undirected graph |

**Returns**: A vector of cliques, where each clique is a vector of vertex indices.

**Complexity**:
- **Time**: O(3^(n/3)) worst case (Moon-Moser bound on number of maximal cliques)
- **Space**: O(n + output size)

---

## 3. Algorithm Description

### Bron-Kerbosch Algorithm

Both functions use the **Bron-Kerbosch algorithm with pivot selection**, a classic backtracking algorithm for clique enumeration.

The algorithm maintains three sets:
- **R**: Current clique being built
- **P**: Candidate vertices that can extend R
- **X**: Excluded vertices (already processed)

**Pivot optimization**: At each recursive step, a pivot vertex is chosen from P ∪ X that maximizes |P ∩ N(pivot)|. This minimizes the number of recursive branches by skipping neighbors of the pivot.

**Pseudocode**:
```
BronKerbosch(R, P, X):
    if P is empty and X is empty:
        report R as a maximal clique
    choose pivot u from P ∪ X to maximize |P ∩ N(u)|
    for each vertex v in P \ N(u):
        BronKerbosch(R ∪ {v}, P ∩ N(v), X ∩ N(v))
        move v from P to X
```

---

## 4. Usage Examples

### 4.1 Finding the Maximum Clique

```cpp
#include <graphlib/graphlib.h>
#include <graphlib/clique.h>
#include <iostream>

int main() {
    // Create a graph with a triangle (3-clique) and an additional vertex
    graphlib::Graph g(4);
    
    // Triangle: vertices 0, 1, 2
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    
    // Vertex 3 connected only to vertex 0
    g.add_edge(0, 3); g.add_edge(3, 0);
    
    auto clique = graphlib::maximum_clique(g);
    
    std::cout << "Maximum clique size: " << clique.size() << "\n";
    std::cout << "Vertices: ";
    for (int v : clique) {
        std::cout << v << " ";
    }
    std::cout << "\n";
    // Output: Maximum clique size: 3
    //         Vertices: 0 1 2
}
```

### 4.2 Finding All Maximal Cliques

```cpp
#include <graphlib/graphlib.h>
#include <graphlib/clique.h>
#include <iostream>

int main() {
    // Create a "bowtie" graph: two triangles sharing a vertex
    graphlib::Graph g(5);
    
    // First triangle: 0, 1, 2
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 0); g.add_edge(0, 2);
    
    // Second triangle: 2, 3, 4
    g.add_edge(2, 3); g.add_edge(3, 2);
    g.add_edge(3, 4); g.add_edge(4, 3);
    g.add_edge(4, 2); g.add_edge(2, 4);
    
    auto cliques = graphlib::find_all_maximal_cliques(g);
    
    std::cout << "Found " << cliques.size() << " maximal cliques:\n";
    for (size_t i = 0; i < cliques.size(); ++i) {
        std::cout << "  Clique " << i << ": ";
        for (int v : cliques[i]) {
            std::cout << v << " ";
        }
        std::cout << "\n";
    }
    // Output: Found 2 maximal cliques:
    //           Clique 0: 0 1 2
    //           Clique 1: 2 3 4
}
```

### 4.3 Clique-Based Analysis

```cpp
#include <graphlib/graphlib.h>
#include <graphlib/clique.h>
#include <algorithm>
#include <iostream>

int main() {
    graphlib::Graph g(6);
    
    // Build a sample social network
    g.add_edge(0, 1); g.add_edge(1, 0);
    g.add_edge(0, 2); g.add_edge(2, 0);
    g.add_edge(1, 2); g.add_edge(2, 1);
    g.add_edge(2, 3); g.add_edge(3, 2);
    g.add_edge(3, 4); g.add_edge(4, 3);
    g.add_edge(3, 5); g.add_edge(5, 3);
    g.add_edge(4, 5); g.add_edge(5, 4);
    
    auto cliques = graphlib::find_all_maximal_cliques(g);
    
    // Compute clique statistics
    size_t max_size = 0;
    size_t total_cliques = cliques.size();
    
    for (const auto& c : cliques) {
        max_size = std::max(max_size, c.size());
    }
    
    std::cout << "Total maximal cliques: " << total_cliques << "\n";
    std::cout << "Maximum clique size: " << max_size << "\n";
    
    // Count cliques by size
    std::vector<int> size_histogram(max_size + 1, 0);
    for (const auto& c : cliques) {
        size_histogram[c.size()]++;
    }
    
    std::cout << "Clique size distribution:\n";
    for (size_t s = 1; s <= max_size; ++s) {
        if (size_histogram[s] > 0) {
            std::cout << "  Size " << s << ": " << size_histogram[s] << " cliques\n";
        }
    }
}
```

---

## 5. Complexity Analysis

| Function | Time Complexity | Space Complexity |
|----------|-----------------|------------------|
| `maximum_clique` | O(3^(n/3)) worst case | O(n) |
| `find_all_maximal_cliques` | O(3^(n/3)) worst case | O(n + output) |

**Notes**:
- The bound O(3^(n/3)) ≈ O(1.44^n) comes from the Moon-Moser theorem, which states that any n-vertex graph has at most 3^(n/3) maximal cliques.
- Pivot selection significantly reduces the practical running time.
- Dense graphs often have fewer maximal cliques, making the algorithm faster in practice.

---

## 6. Applications

- **Social network analysis**: Finding tightly-knit groups of friends.
- **Bioinformatics**: Detecting protein complexes in protein-protein interaction networks.
- **Coding theory**: Finding maximum independent sets (clique in complement graph).
- **Computer vision**: Object recognition via correspondence graphs.
- **Scheduling**: Finding conflict-free task sets.

---

## 7. Important Notes

1. **Undirected graphs**: The algorithms assume an undirected graph. For directed graphs, convert to undirected first or define cliques appropriately.

2. **Edge representation**: The graph should represent edges in both directions for undirected behavior (e.g., `add_edge(u, v)` and `add_edge(v, u)`).

3. **Performance**: For very large graphs (n > 100), consider:
   - Preprocessing to remove low-degree vertices.
   - Using vertex ordering heuristics (degeneracy ordering).
   - Parallelization for independent subproblems.

4. **Output size**: The number of maximal cliques can be exponential. For large graphs, `find_all_maximal_cliques` may produce very large output.
