# Network Motifs and Graphlets

## Overview

This module provides algorithms for detecting and counting network motifs, graphlets, and subgraph patterns.

## Header

```cpp
#include <graphlib/motif.h>
```

## API Reference

### Triangle Counting

```cpp
TriangleResult count_triangles(const Graph& g);
```

Counts all triangles using node-iterator algorithm with degree ordering.

**Complexity**: O(m√m) where m = edges

**Example**:
```cpp
Graph g(5, false);
g.add_edge(0, 1); g.add_edge(1, 2); g.add_edge(2, 0);  // Triangle
g.add_edge(1, 3); g.add_edge(3, 4);

auto result = graphlib::count_triangles(g);
std::cout << "Total triangles: " << result.total_triangles << "\n";  // 1
std::cout << "Triangles at vertex 1: " << result.per_vertex[1] << "\n";  // 1
```

### List Triangles

```cpp
std::vector<std::tuple<int, int, int>> list_triangles(const Graph& g, int max_triangles = 0);
```

Lists all triangles as (u, v, w) tuples.

### K-Clique Counting

```cpp
long long count_4cliques(const Graph& g);
long long count_k_cliques(const Graph& g, int k);
```

Count cliques of size k.

### Network Motifs (Directed Graphs)

```cpp
std::map<MotifType, long long> count_3node_motifs(const Graph& g);
```

Counts 3-node network motifs in directed graphs.

**Motif Types**:
```cpp
enum class MotifType {
    M1,   // A -> B -> C (feedforward)
    M2,   // A -> B, A -> C (fan-out)
    M3,   // A -> B, C -> B (fan-in)
    M4,   // A -> B -> C, A -> C (feedforward loop)
    M5,   // A <-> B -> C
    M6,   // A -> B -> C -> A (cycle)
    // ... and more
};
```

### Graphlets

```cpp
std::vector<long long> count_graphlets(const Graph& g, int k);
```

Counts graphlets (induced subgraphs) of size k (2-5).

### Graphlet Degree Distribution

```cpp
std::vector<std::vector<long long>> graphlet_degree_distribution(const Graph& g, int max_size = 4);
```

For each vertex, counts participation in each graphlet orbit. Useful for network comparison.

### Transitivity (Global Clustering)

```cpp
double transitivity(const Graph& g);
```

Computes global clustering coefficient: 3 × triangles / connected_triples

### Pattern Finding

```cpp
std::vector<std::vector<int>> find_motif_occurrences(const Graph& g, const Graph& pattern, 
                                                      int max_matches = 1000);
```

Finds all occurrences of a pattern subgraph.

### Path Counting

```cpp
long long count_paths_of_length(const Graph& g, int u, int v, int k);
std::vector<long long> count_simple_paths(const Graph& g, int max_length);
```

## Data Structures

### TriangleResult

```cpp
struct TriangleResult {
    long long total_triangles;
    std::vector<long long> per_vertex;  // Triangles per vertex
};
```

## Use Cases

1. **Network Fingerprinting**: Use motif/graphlet distributions to characterize networks
2. **Biological Networks**: Identify functional modules via motif enrichment
3. **Social Networks**: Analyze triadic closure patterns
4. **Anomaly Detection**: Compare motif distributions across network snapshots
