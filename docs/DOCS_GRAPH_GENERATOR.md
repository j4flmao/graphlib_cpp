# Graph Generators

This module facilitates testing and benchmarking by generating various types of synthetic graphs.

Header:
```cpp
#include <graphlib/graph_generator.h>
```

## 1. Classic Graphs

### 1.1 Complete Graph ($K_n$)
```cpp
auto K5 = graphlib::make_complete_graph(5, false); // Undirected
```

### 1.2 Cycle Graph ($C_n$)
```cpp
auto C10 = graphlib::make_cycle_graph(10, false);
```

### 1.3 Path Graph ($P_n$)
```cpp
auto P10 = graphlib::make_path_graph(10, false);
```

### 1.4 Grid Graph (2D Mesh)
Creates an $R \times C$ grid.
```cpp
auto grid = graphlib::make_grid_graph(5, 5, false); // 5x5 grid
```

### 1.5 Star Graph
One center connected to $N-1$ leaves.
```cpp
auto star = graphlib::make_star_graph(10, false);
```

## 2. Random Graphs

### 2.1 Erdős-Rényi ($G(n, p)$)
Each edge exists with probability $p$.
```cpp
auto er = graphlib::make_random_graph(100, 0.1, false);
```

### 2.2 Watts-Strogatz (Small World)
Constructs a ring lattice and rewires edges with probability $\beta$.
```cpp
// 100 nodes, each connected to 4 neighbors (k=4), rewire prob 0.3
auto ws = graphlib::make_watts_strogatz_graph(100, 4, 0.3, false);
```

### 2.3 Barabási-Albert (Scale-Free)
Generates a graph using preferential attachment.
```cpp
// 100 nodes, each new node attaches to 2 existing nodes
auto ba = graphlib::make_barabasi_albert_graph(100, 2, false);
```

### 2.4 Random Tree
Generates a random tree using Prüfer sequences or random attachment.
```cpp
auto tree = graphlib::make_random_tree(50);
```
