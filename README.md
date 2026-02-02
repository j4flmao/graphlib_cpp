# GraphLib - Professional C++ Graph Algorithms Library

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.15+-green.svg)](https://cmake.org/)

A comprehensive, high-performance C++ library for graph algorithms and data structures, built with modern C++17 and designed for both academic learning and production use.

## 🎯 Features

### Graph Algorithms
- **Connectivity**: Connected Components, Bridges, Articulation Points, Strongly Connected Components (Tarjan, Kosaraju), 2-SAT
- **Maximum Flow**: Edmonds-Karp, Dinic's algorithm, Min-Cost Max Flow, Min-Cost Circulation
- **Bipartite Graphs**: 2-coloring, maximum matching (Hopcroft-Karp), Hungarian assignment
- **General Graph Matching**: Edmonds' Blossom (maximum cardinality and weighted)
- **DAG Operations**: Topological sort, longest/shortest paths, path counting, SCC condensation
- **Shortest Paths**: Dijkstra (Bidirectional), Bellman-Ford, Floyd-Warshall, Johnson, A*, K-Shortest Paths (Yen's Algorithm), Minimum Mean Cycle
- **Minimum Spanning Tree**: Kruskal, Prim with Union-Find, Directed MST (Chu-Liu/Edmonds)
- **Tree Algorithms**: Lowest Common Ancestor (LCA), Heavy-Light Decomposition, Path Sum/Max Queries, Centroid Decomposition, Dominator Tree (Lengauer-Tarjan), Tree Decomposition (Min-Degree, Min-Fill)
- **NP-Hard Problems**: TSP (Bitmask DP, Christofides Approx, Simulated Annealing, Ant Colony), Max Clique (Bron-Kerbosch), Graph Coloring (Welsh-Powell, DSATUR), Chinese Postman Problem, Maximum Weight Closure, Maximum Density Subgraph, Vertex Cover Approx
- **Advanced Algorithms**: Tree Isomorphism (Rooted/Unrooted), Steiner Tree (Dreyfus-Wagner), Tree Center, Planarity Testing (Demoucron), Chordal Graphs (MCS, PEO, Max Clique/Coloring), Graph Isomorphism (VF2, WL Hash), Hypergraphs (Dual Graph, Incidence)
- **Graph Measures**: Centrality (Degree, Closeness, Betweenness, Eigenvector, PageRank, Katz, HITS), Community Detection (LPA, Modularity/Louvain), Spectral Analysis (Fiedler Vector)
- **Generators**: Random (Erdős-Rényi, Watts-Strogatz, Barabási-Albert), Grid, Tree, Complete, Cycle
- **Min Cut**: Global Min Cut (Stoer-Wagner), Gomory-Hu Tree

### Data Structures
- **Splay Tree**: Self-balancing BST with O(log n) amortized operations
  - Insert, search, delete
  - Split and join operations
  - Range queries
- **Link-Cut Tree**: Dynamic tree data structure for O(log n) path queries and link/cut operations
- **DSU with Rollback**: Disjoint Set Union supporting state rollback (useful for dynamic connectivity)

### Design Principles
✅ **Modern C++17** with RAII and proper memory management  
✅ **Pointer-based** implementation for flexibility  
✅ **Cross-platform** (Linux, Windows, macOS)  
✅ **Shared library** (DLL/SO) with clean API  
✅ **Well-tested** with comprehensive unit tests (GTest)  
✅ **Production-ready** with proper error handling  

---

## 📦 Installation

### Prerequisites

- **C++17 compatible compiler**: GCC 7+, Clang 6+, or MSVC 2019+
- **CMake** 3.15 or later
- **vcpkg** (optional but recommended)
- **GNU Make** (optional, for automation)

### Quick Install

```bash
# Clone the repository
git clone https://github.com/j4flmao/graphlib_cpp
cd graphlib

# Build and install (Method 1: Make)
make all
sudo make install

# OR Build and install (Method 2: CMake)
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --install build
```

### Platform-Specific Instructions

<details>
<summary><b>Ubuntu/Debian</b></summary>

```bash
# Install dependencies
sudo apt update
sudo apt install build-essential cmake git

# Build
make all
sudo make install
```
</details>

<details>
<summary><b>Windows (MSVC)</b></summary>

```powershell
# Using Visual Studio Developer Command Prompt
cmake -B build -G "Visual Studio 16 2019" -A x64
cmake --build build --config Release
cmake --install build --prefix "C:\Program Files\GraphLib"
```
</details>

<details>
<summary><b>macOS</b></summary>

```bash
# Install dependencies
brew install cmake

# Build
mkdir build && cd build
cmake ..
make
sudo make install
```
</details>

---

## 🚀 Quick Start

### Basic Graph Creation

```cpp
#include <graphlib/graph.h>
#include <graphlib/shortest_path.h>
#include <iostream>

int main() {
    // Create a directed graph with 5 vertices
    Graph g(5, true);

    // Add edges (u, v, weight)
    g.add_edge(0, 1, 10);
    g.add_edge(0, 4, 5);
    g.add_edge(1, 2, 1);
    g.add_edge(2, 3, 4);
    g.add_edge(4, 1, 3);
    g.add_edge(4, 2, 9);
    g.add_edge(4, 3, 2);
    g.add_edge(3, 0, 7);

    // Run Dijkstra's Algorithm
    std::vector<int> dist;
    std::vector<int> parent;
    dijkstra(g, 0, dist, parent);

    // Print distances
    for (int i = 0; i < 5; ++i) {
        std::cout << "Distance to " << i << ": " << dist[i] << std::endl;
    }

    return 0;
}
```

### Advanced Features

#### Community Detection (Louvain)
```cpp
#include <graphlib/graph_measures.h>

Graph g(100);
// ... add edges ...
auto communities = louvain_method(g);
std::cout << "Modularity: " << modularity(g, communities) << std::endl;
```

#### Metric TSP Approximation (Christofides)
```cpp
#include <graphlib/np_hard.h>

Graph g = ...; // Complete metric graph
auto [cost, path] = tsp_christofides(g);
std::cout << "Approx Cost: " << cost << std::endl;
```

#### TSP Solving (Simulated Annealing)
```cpp
#include <graphlib/heuristics.h>

Graph g = ...; // Complete graph with weights
auto path = solve_tsp_simulated_annealing(g);
```

## 📚 Documentation

Detailed documentation for all modules is available in the `docs/` directory:
- [Graph Algorithms](docs/algorithms.md)
- [Data Structures](docs/data_structures.md)
- [Advanced Features](docs/advanced_features.md)
- [NP-Hard Problems](docs/np_hard.md)
- [Hypergraphs](docs/DOCS_HYPERGRAPH.md)

## 🤝 Contributing

Contributions are welcome! Please read `CONTRIBUTING.md` for details on our code of conduct and the process for submitting pull requests.

## � License

This project is licensed under the MIT License - see the `LICENSE` file for details.
