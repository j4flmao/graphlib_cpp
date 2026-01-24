# Chordal Graphs

This module provides algorithms specifically optimized for **Chordal Graphs** (graphs where every cycle of length > 3 has a chord). Many NP-hard problems on general graphs are polynomial-time solvable on chordal graphs.

Header:
```cpp
#include <graphlib/chordal.h>
```

## 1. Recognition and Ordering

### 1.1 Maximum Cardinality Search (MCS)
Computes a Perfect Elimination Ordering (PEO) if one exists.
- **Time Complexity**: $O(V + E)$.

```cpp
std::vector<int> peo = graphlib::maximum_cardinality_search(g);
```

### 1.2 Check Chordality
Checks if a graph is chordal using the PEO property.
```cpp
bool is_chordal = graphlib::is_chordal(g);
```

## 2. Optimization Problems on Chordal Graphs

If a graph is chordal, the following problems can be solved exactly and efficiently:

### 2.1 Maximum Clique
Finds the size of the largest clique (complete subgraph).
- **Complexity**: $O(V + E)$ (after PEO).

```cpp
int max_clique_size = graphlib::max_clique_chordal(g);
```

### 2.2 Chromatic Number
Finds the minimum number of colors needed to color the graph. For chordal graphs, $\chi(G) = \omega(G)$ (Chromatic Number = Max Clique Size).
- **Complexity**: $O(V + E)$.

```cpp
int chromatic_num = graphlib::chromatic_number_chordal(g);
```

### 2.3 Maximum Independent Set
Finds the size of the maximum independent set.
- **Complexity**: $O(V + E)$.

```cpp
int max_is = graphlib::max_independent_set_chordal(g);
```

## Example

```cpp
#include <graphlib/chordal.h>
#include <graphlib/graph_core.h>

graphlib::Graph g(4, false);
// Create a chordal graph (e.g., a diamond shape with a chord)
g.add_edge(0, 1);
g.add_edge(1, 2);
g.add_edge(2, 3);
g.add_edge(3, 0);
g.add_edge(0, 2); // The chord

if (graphlib::is_chordal(g)) {
    std::cout << "Graph is chordal.\n";
    std::cout << "Max Clique Size: " << graphlib::max_clique_chordal(g) << "\n";
}
```
