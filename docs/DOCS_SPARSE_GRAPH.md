# Compressed Graph Representations

## Overview

This module provides memory-efficient graph representations optimized for read-heavy operations, parallel algorithms, and ML pipelines.

## Header

```cpp
#include <graphlib/sparse_graph.h>
```

## Graph Formats

| Format | Best For | Memory |
|--------|----------|--------|
| CSR (Compressed Sparse Row) | Out-degree queries, forward traversal | O(n + m) |
| CSC (Compressed Sparse Column) | In-degree queries, reverse traversal | O(n + m) |
| COO (Coordinate) | Building graphs, exporting to ML | O(m) |

## API Reference

### CSRGraph

```cpp
class CSRGraph {
public:
    CSRGraph();
    CSRGraph(int n, const std::vector<std::tuple<int, int, long long>>& edges, bool directed = true);
    explicit CSRGraph(const Graph& g);
    
    int vertex_count() const;
    int edge_count() const;
    bool is_directed() const;
    int degree(int v) const;
    
    std::pair<const int*, const int*> neighbors(int v) const;
    bool has_edge(int u, int v) const;
    long long get_weight(int u, int v) const;
    
    Graph to_graph() const;
    CSRGraph transpose() const;
    
    // Raw access for algorithms
    const std::vector<int>& row_ptr() const;
    const std::vector<int>& col_idx() const;
    const std::vector<long long>& values() const;
};
```

**Example**:
```cpp
Graph g(4, false);
g.add_edge(0, 1, 5);
g.add_edge(1, 2, 3);
g.add_edge(2, 3, 4);

CSRGraph csr(g);

// Iterate neighbors efficiently
auto [begin, end] = csr.neighbors(1);
for (const int* it = begin; it != end; ++it) {
    std::cout << "Neighbor: " << *it << "\n";
}
```

### CSCGraph

```cpp
class CSCGraph {
public:
    explicit CSCGraph(const Graph& g);
    explicit CSCGraph(const CSRGraph& csr);
    
    int in_degree(int v) const;
    std::pair<const int*, const int*> predecessors(int v) const;
    
    CSRGraph to_csr() const;
};
```

### COOGraph

```cpp
class COOGraph {
public:
    COOGraph(int n, bool directed = true);
    explicit COOGraph(const Graph& g);
    
    void add_edge(int from, int to, long long weight = 1);
    void reserve(int expected_edges);
    void sort();
    void remove_duplicates();
    
    const std::vector<int>& row() const;
    const std::vector<int>& col() const;
    const std::vector<long long>& data() const;
    
    CSRGraph to_csr() const;
    CSCGraph to_csc() const;
};
```

**Example - Building for ML Export**:
```cpp
COOGraph coo(1000);

// Build graph incrementally
for (const auto& [u, v, w] : edges) {
    coo.add_edge(u, v, w);
}

// Remove duplicates and sort
coo.remove_duplicates();

// Export edge indices for PyTorch Geometric
const auto& row = coo.row();
const auto& col = coo.col();
// row and col can be copied directly to tensors
```

## Conversion Functions

```cpp
CSRGraph graph_to_csr(const Graph& g);
CSCGraph graph_to_csc(const Graph& g);
COOGraph graph_to_coo(const Graph& g);
```

## Performance Comparison

| Operation | Graph (adj list) | CSR | COO |
|-----------|------------------|-----|-----|
| Neighbor iteration | O(deg) | O(deg) ✓ | O(m) |
| Edge lookup | O(deg) | O(log deg) ✓ | O(m) |
| Add edge | O(1) ✓ | O(m) rebuild | O(1) ✓ |
| Memory | O(n + 2m) | O(n + m) ✓ | O(3m) |
| Cache locality | Poor | Excellent ✓ | Good |

## Use Cases

1. **Large-Scale BFS/SSSP**: CSR provides cache-friendly neighbor access
2. **PageRank/Graph Neural Networks**: CSR + CSC for forward/backward passes
3. **Graph Construction**: COO for building, convert to CSR for algorithms
4. **ML Pipeline**: Export COO indices directly to PyTorch/NumPy
