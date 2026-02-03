# GraphLib

<div align="center">

[![CI](https://github.com/j4flmao/graphlib_cpp/actions/workflows/cmake.yml/badge.svg)](https://github.com/j4flmao/graphlib_cpp/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.15+-green.svg)](https://cmake.org/)
[![Vcpkg](https://img.shields.io/badge/Vcpkg-Supported-purple)](https://vcpkg.io/)
[![Documentation](https://img.shields.io/badge/docs-reference-orange.svg)](docs/)

<br/>

[![GitHub stars](https://img.shields.io/github/stars/j4flmao/graphlib_cpp?style=social)](https://github.com/j4flmao/graphlib_cpp/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/j4flmao/graphlib_cpp?style=social)](https://github.com/j4flmao/graphlib_cpp/network/members)
[![GitHub watchers](https://img.shields.io/github/watchers/j4flmao/graphlib_cpp?style=social)](https://github.com/j4flmao/graphlib_cpp/watchers)
[![GitHub contributors](https://img.shields.io/github/contributors/j4flmao/graphlib_cpp?color=blue)](https://github.com/j4flmao/graphlib_cpp/graphs/contributors)

**Professional C++ Graph Algorithms Library**

*High-performance, modern C++17 library designed for academic research and production-grade network analysis.*

[**Explore Docs**](docs/) · [**Report Bug**](https://github.com/j4flmao/graphlib_cpp/issues) · [**Request Feature**](https://github.com/j4flmao/graphlib_cpp/issues)

</div>

---

## 📖 Table of Contents

- [Overview](#-overview)
- [Key Features](#-key-features)
- [Installation](#-installation)
  - [CMake FetchContent](#cmake-fetchcontent)
  - [Build from Source](#build-from-source)
- [Quick Start](#-quick-start)
- [Documentation](#-documentation)
- [Contributing](#-contributing)
- [License](#-license)

---

## 🔭 Overview

**GraphLib** fills the gap between academic graph theory and production engineering. It provides a robust collection of graph algorithms implemented with:
- **Modern C++17**: Utilizing structured bindings, `constexpr`, and move semantics.
- **Performance**: Cache-friendly sparse matrix representations (CSR/CSC) and parallel algorithms.
- **Flexibility**: Generic interfaces supporting directed, undirected, temporal, and signed graphs.

Whether you are analyzing social networks, optimizing logistics, or researching structural balance theory, GraphLib offers the tools you need.

---

## 🎯 Key Features

<table>
  <tr>
    <td valign="top" width="50%">
      <h3>🦄 Core Algorithms</h3>
      <ul>
        <li><b>Shortest Paths</b>: Bidirectional Dijkstra, A*, Johnson, Yen's K-Shortest.</li>
        <li><b>Flow & Cuts</b>: Dinic, Push-Relabel, Min-Cost Max-Flow, Global Min Cut.</li>
        <li><b>Connectivity</b>: Tarjan's SCC, Bridges, Articulation Points, 2-SAT.</li>
        <li><b>Trees</b>: LCA, Centroid Decomposition, Heavy-Light Decomposition.</li>
      </ul>
    </td>
    <td valign="top" width="50%">
      <h3>🕸️ Network Analysis</h3>
      <ul>
        <li><b>Centrality</b>: PageRank, Betweenness, Closeness, Eigenvector.</li>
        <li><b>Community</b>: Louvain Modularity, Label Propagation.</li>
        <li><b>Motifs</b>: Fast Triangle Counting, K-Clique, Graphlet Degree.</li>
        <li><b>Isomorphism</b>: VF2 Algorithm, Weisfeiler-Lehman.</li>
      </ul>
    </td>
  </tr>
  <tr>
    <td valign="top" width="50%">
      <h3>⚡ High Performance</h3>
      <ul>
        <li><b>Parallelism</b>: Multi-threaded BFS, PageRank, Delta-Stepping SSSP.</li>
        <li><b>Sparse Formats</b>: Memory-efficient CSR/CSC/COO representations.</li>
        <li><b>Dynamic Trees</b>: Link-Cut Trees, Splay Trees.</li>
      </ul>
    </td>
    <td valign="top" width="50%">
      <h3>🧪 Specialized Graphs</h3>
      <ul>
        <li><b>Temporal</b>: Time-varying edges and temporal reachability.</li>
        <li><b>Signed</b>: Balance theory, frustration index, sign prediction.</li>
        <li><b>NP-Hard Solvers</b>: TSP (Exact/Approx), Max Clique, Coloring.</li>
      </ul>
    </td>
  </tr>
</table>

---

## 📦 Installation

### CMake FetchContent (Recommended)
You can include GraphLib directly in your project using CMake's `FetchContent`.

```cmake
include(FetchContent)
FetchContent_Declare(
  graphlib
  GIT_REPOSITORY https://github.com/j4flmao/graphlib_cpp.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(graphlib)

target_link_libraries(your_target PRIVATE graphlib)
```

### Build from Source
Standard CMake build process:

```bash
git clone https://github.com/j4flmao/graphlib_cpp.git
cd graphlib_cpp

# 1. Configure
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# 2. Build
cmake --build build --config Release -j 4

# 3. Install
sudo cmake --install build
```

---

## 🚀 Quick Start

### 1. Minimal Example: Dijkstra's Algorithm

```cpp
#include <graphlib/graphlib.h>
#include <iostream>

int main() {
    // Directed graph with 5 vertices
    graphlib::Graph g(5, true); 
    
    // add_edge(u, v, weight)
    g.add_edge(0, 1, 10);
    g.add_edge(1, 2, 20);
    g.add_edge(0, 2, 100);

    // Compute shortest paths from node 0
    graphlib::ShortestPath sp(g);
    auto dist = sp.dijkstra(0);

    std::cout << "Distance 0 -> 2: " << dist[2] << std::endl; // Output: 30
    return 0;
}
```

### 2. Advanced: Community Detection

```cpp
#include <graphlib/graphlib.h>

int main() {
    // Generate a random geometric graph
    auto g = graphlib::generator::random_geometric(1000, 0.1);
    
    // Detect communities using the Louvain method
    auto communities = graphlib::louvain_communities(g);
    
    // Calculate network modularity
    double q = graphlib::modularity(g, communities);
    
    return 0;
}
```

---

## 📚 Documentation

Detailed documentation is available in the **[docs/](docs/)** directory.

| Category | Reference |
|----------|-----------|
| **Basics** | [Graph Core](docs/DOCS_GRAPH_CORE.md) • [I/O](docs/DOCS_GRAPH_IO.md) • [Standards](docs/CPP_STANDARDS.md) |
| **Pathfinding** | [Shortest Paths](docs/DOCS_SHORTEST_PATH.md) • [K-Shortest](docs/DOCS_K_SHORTEST_PATHS.md) |
| **Core Algo** | [Max Flow](docs/DOCS_MAX_FLOW.md) • [Connectivity](docs/DOCS_CONNECTIVITY_SCC.md) • [Trees](docs/DOCS_TREE.md) |
| **Analysis** | [Measures](docs/DOCS_GRAPH_MEASURES.md) • [Community](docs/DOCS_COMMUNITY.md) • [Motifs](docs/DOCS_MOTIF.md) |
| **Advanced** | [NP-Hard](docs/DOCS_NP_HARD.md) • [Parallel](docs/DOCS_PARALLEL.md) • [Isomorphism](docs/DOCS_ISOMORPHISM.md) |

---

## 🤝 Contributing

We welcome contributions from the community!

1.  **Issue**: Open an issue to discuss features or bugs.
2.  **Fork**: Fork the repository on GitHub.
3.  **Branch**: Create a feature branch (`git checkout -b feature/new-algo`).
4.  **Code**: Follow the [Coding Standards](docs/CPP_STANDARDS.md).
5.  **Test**: Add unit tests in `tests/` and verify with `ctest`.
6.  **PR**: Submit a Pull Request.

See [CONTRIBUTING.md](CONTRIBUTING.md) for more details.

---

## 📜 License

Distributed under the MIT License. See [LICENSE](LICENSE) for more information.

<br />
<div align="center">
  <p>
    Made with 💻 and ☕ by the <a href="https://github.com/j4flmao">GraphLib</a>
  </p>
  <p>
    <a href="https://github.com/j4flmao/graphlib_cpp/issues">Report Issue</a> •
    <a href="https://github.com/j4flmao/graphlib_cpp/pulls">Submit Request</a>
  </p>
</div>
