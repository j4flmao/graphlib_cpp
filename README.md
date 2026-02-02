# GraphLib - Professional C++ Graph Algorithms Library

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.15+-green.svg)](https://cmake.org/)

A comprehensive, high-performance C++ library for graph algorithms and data structures, built with modern C++17 and designed for both academic learning and production use.

## 🎯 Features

### Core Graph Algorithms
- **Connectivity**: Connected Components, Bridges, Articulation Points, Strongly Connected Components (Tarjan, Kosaraju), 2-SAT, Block-Cut Tree
- **Maximum Flow**: Edmonds-Karp, Dinic's algorithm, Push-Relabel, Min-Cost Max Flow, Min-Cost Circulation
- **Bipartite Graphs**: 2-coloring, maximum matching (Hopcroft-Karp, Kuhn), Hungarian assignment
- **General Graph Matching**: Edmonds' Blossom (maximum cardinality and weighted)
- **DAG Operations**: Topological sort (Kahn, DFS), longest/shortest paths, path counting, SCC condensation
- **Shortest Paths**: Dijkstra (Bidirectional), Bellman-Ford, Floyd-Warshall, Johnson, A*, 0-1 BFS, K-Shortest Paths (Yen), Minimum Mean Cycle
- **Minimum Spanning Tree**: Kruskal, Prim, Directed MST (Chu-Liu/Edmonds)
- **Tree Algorithms**: LCA (Binary Lifting), Heavy-Light Decomposition, Path Sum/Max Queries, Centroid Decomposition, Dominator Tree, Tree Decomposition
- **Min Cut**: Global Min Cut (Stoer-Wagner), Gomory-Hu Tree

### NP-Hard & Approximation Algorithms
- **TSP**: Bitmask DP (exact), Christofides (1.5-approx), MST-based (2-approx), Simulated Annealing, Ant Colony
- **Max Clique**: Bron-Kerbosch (exact), Maximum Weight Clique
- **Graph Coloring**: Welsh-Powell (greedy), DSATUR
- **Other**: Chinese Postman, Maximum Weight Closure, Maximum Density Subgraph, Vertex Cover (2-approx)

### Advanced Algorithms
- **Planarity**: Planarity Testing (Demoucron), Face Enumeration
- **Chordal Graphs**: Recognition (MCS), Perfect Elimination Ordering, Max Clique/Coloring
- **Graph Isomorphism**: VF2, Subgraph Isomorphism, Weisfeiler-Lehman Hash
- **Steiner Tree**: Dreyfus-Wagner (exact)
- **Cycle Analysis**: Minimum Cycle Basis (Horton), Fundamental Cycles, Girth, Cycle Enumeration (Johnson)

### Graph Measures & Network Analysis
- **Centrality**: Degree, Closeness, Betweenness (Brandes), Eigenvector, PageRank, Katz, HITS
- **Clustering**: Local/Global Clustering Coefficient, Transitivity
- **Community Detection**: Label Propagation (LPA), Louvain Modularity
- **Spectral Analysis**: Fiedler Vector, Spectral Bisection
- **Graph Properties**: Eccentricity, Diameter, Radius, Center, Periphery, K-Core Decomposition
- **Link Prediction**: Jaccard Index, Adamic-Adar Index

### Network Motifs & Subgraph Patterns
- **Triangle Counting**: Fast node-iterator algorithm O(m√m)
- **K-Clique Counting**: 4-cliques, general k-cliques
- **Network Motifs**: 3-node directed motif census
- **Graphlets**: Graphlet enumeration, Graphlet Degree Distribution

### Graph I/O & Formats
- **Read/Write**: Edge List, DIMACS, Matrix Market, CSV, Adjacency Matrix
- **Export**: DOT (Graphviz) for visualization

### Random Walk Algorithms
- **Basic Walks**: Uniform random walks, configurable length/count
- **Node2Vec**: Biased random walks with p/q parameters
- **Metrics**: Hitting Time, Commute Time, Cover Time
- **Analysis**: Stationary Distribution, Personalized PageRank

### Compressed Graph Representations
- **CSR (Compressed Sparse Row)**: Optimal for forward traversal
- **CSC (Compressed Sparse Column)**: Optimal for reverse traversal
- **COO (Coordinate)**: Optimal for graph construction

### Temporal Graphs
- **Time-Varying Edges**: Edges with timestamps and durations
- **Temporal Paths**: Earliest Arrival, Latest Departure, Fastest/Shortest paths
- **Analysis**: Temporal reachability, Temporal betweenness, Burstiness

### Signed Graphs
- **Balance Theory**: Balance checking, Balance partition
- **Analysis**: Frustration index, Signed clustering, Status scores
- **Prediction**: Sign prediction using triadic closure

### Multigraphs & Hypergraphs
- **Multigraph**: Multiple edges, edge IDs, self-loops, simplification
- **Hypergraph**: Hyperedges, bipartite/clique expansion, dual graph

### Parallel Algorithms
- **Parallel BFS**: Frontier-based parallelism
- **Parallel Connected Components**: Shiloach-Vishkin
- **Parallel PageRank**: Multi-threaded power iteration
- **Parallel Triangle Counting**
- **Parallel SSSP**: Delta-Stepping
- **Parallel Betweenness**: Source-parallel Brandes

### Graph Generators
- **Random Models**: Erdős-Rényi, Barabási-Albert, Watts-Strogatz, R-MAT
- **Structured**: Complete, Cycle, Path, Grid, Random Tree

### Data Structures
- **Splay Tree**: Self-balancing BST, O(log n) amortized, split/join, range queries
- **Link-Cut Tree**: Dynamic tree, O(log n) path queries, link/cut operations
- **DSU with Rollback**: Union-Find with state rollback

---

## 📦 Installation

### Prerequisites

- **C++17 compatible compiler**: GCC 7+, Clang 6+, or MSVC 2019+
- **CMake** 3.15 or later

### Quick Install

```bash
git clone https://github.com/j4flmao/graphlib_cpp
cd graphlib_cpp
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --install build
```

---

## 🚀 Quick Start

### Basic Usage

```cpp
#include <graphlib/graphlib.h>
using namespace graphlib;

int main() {
    Graph g(5, true);
    g.add_edge(0, 1, 10);
    g.add_edge(1, 2, 5);
    
    ShortestPath sp(5);
    sp.add_edge(0, 1, 10);
    sp.add_edge(1, 2, 5);
    auto dist = sp.dijkstra(0, 1e18);
    
    return 0;
}
```

### Network Analysis

```cpp
Graph g = generator::barabasi_albert(1000, 3);
auto pr = pagerank(g);
auto communities = louvain_communities(g);
auto tri = count_triangles(g);
```

### Random Walks

```cpp
Node2VecConfig config;
config.p = 0.5; config.q = 2.0;
auto walks = node2vec_walks(g, config);
```

### Temporal Graphs

```cpp
TemporalGraph tg(100, true);
tg.add_edge(0, 1, 100);
tg.add_edge(1, 2, 150);
auto arrival = tg.earliest_arrival(0, 0);
```

### Parallel Algorithms

```cpp
using namespace graphlib::parallel;
auto dist = parallel_bfs(g, 0);
auto pr = parallel_pagerank(g);
```

## 📚 Documentation

| Module | Documentation |
|--------|---------------|
| Core Graph | [DOCS_GRAPH_CORE.md](docs/DOCS_GRAPH_CORE.md) |
| Shortest Paths | [DOCS_SHORTEST_PATH.md](docs/DOCS_SHORTEST_PATH.md) |
| Maximum Flow | [DOCS_MAX_FLOW.md](docs/DOCS_MAX_FLOW.md) |
| NP-Hard | [DOCS_NP_HARD.md](docs/DOCS_NP_HARD.md) |
| Graph Measures | [DOCS_GRAPH_MEASURES.md](docs/DOCS_GRAPH_MEASURES.md) |
| Cycle Basis | [DOCS_CYCLE_BASIS.md](docs/DOCS_CYCLE_BASIS.md) |
| Graph I/O | [DOCS_GRAPH_IO.md](docs/DOCS_GRAPH_IO.md) |
| Random Walks | [DOCS_RANDOM_WALK.md](docs/DOCS_RANDOM_WALK.md) |
| Motifs | [DOCS_MOTIF.md](docs/DOCS_MOTIF.md) |
| Sparse Formats | [DOCS_SPARSE_GRAPH.md](docs/DOCS_SPARSE_GRAPH.md) |
| Temporal Graphs | [DOCS_TEMPORAL_GRAPH.md](docs/DOCS_TEMPORAL_GRAPH.md) |
| Signed Graphs | [DOCS_SIGNED_GRAPH.md](docs/DOCS_SIGNED_GRAPH.md) |
| Multigraphs | [DOCS_MULTIGRAPH.md](docs/DOCS_MULTIGRAPH.md) |
| Parallel | [DOCS_PARALLEL.md](docs/DOCS_PARALLEL.md) |
| Clique | [DOCS_CLIQUE.md](docs/DOCS_CLIQUE.md) |
| Coloring | [DOCS_COLORING.md](docs/DOCS_COLORING.md) |
| 2-SAT | [DOCS_TWO_SAT.md](docs/DOCS_TWO_SAT.md) |
| Isomorphism | [DOCS_ISOMORPHISM.md](docs/DOCS_ISOMORPHISM.md) |
| Centroid Decomposition | [DOCS_CENTROID.md](docs/DOCS_CENTROID.md) |
| Link-Cut Tree | [DOCS_LINK_CUT_TREE.md](docs/DOCS_LINK_CUT_TREE.md) |
| Dynamic Connectivity | [DOCS_DYNAMIC_CONNECTIVITY.md](docs/DOCS_DYNAMIC_CONNECTIVITY.md) |
| Dominator Tree | [DOCS_DOMINATOR.md](docs/DOCS_DOMINATOR.md) |
| Chordal Graphs | [DOCS_CHORDAL.md](docs/DOCS_CHORDAL.md) |

## 🧪 Testing

```bash
cmake -B build -S . -DBUILD_TESTS=ON
cmake --build build
ctest --test-dir build
```

## 📄 License

MIT License - see [LICENSE](LICENSE) file.
