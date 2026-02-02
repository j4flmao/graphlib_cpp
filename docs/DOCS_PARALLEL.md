# Parallel Graph Algorithms

## Overview

This module provides parallel implementations of common graph algorithms using C++ threads.

## Header

```cpp
#include <graphlib/parallel.h>
```

All functions are in the `graphlib::parallel` namespace.

## Configuration

```cpp
struct ParallelConfig {
    int num_threads = 0;        // 0 = auto (hardware concurrency)
    int grain_size = 1024;      // Minimum work per thread
    bool enable_stealing = true;
};

void set_parallel_config(ParallelConfig config);
int get_num_threads();
```

## Execution Policy

```cpp
enum class ExecutionPolicy {
    Sequential,  // Single-threaded
    Parallel     // Multi-threaded
};
```

All parallel functions accept an `ExecutionPolicy` parameter (default: `Parallel`).

## Parallel Algorithms

### BFS

```cpp
std::vector<int> parallel_bfs(const Graph& g, int source, 
                               ExecutionPolicy policy = ExecutionPolicy::Parallel);

std::vector<int> parallel_bfs(const CSRGraph& g, int source,
                               ExecutionPolicy policy = ExecutionPolicy::Parallel);
```

Uses frontier-based parallelism.

### Connected Components

```cpp
int parallel_connected_components(const Graph& g, std::vector<int>& component,
                                   ExecutionPolicy policy = ExecutionPolicy::Parallel);
```

Uses Shiloach-Vishkin style parallel union-find.

### PageRank

```cpp
std::vector<double> parallel_pagerank(const Graph& g, double d = 0.85,
                                       int iterations = 100, double tolerance = 1e-6,
                                       ExecutionPolicy policy = ExecutionPolicy::Parallel);
```

### Triangle Counting

```cpp
long long parallel_triangle_count(const Graph& g,
                                   ExecutionPolicy policy = ExecutionPolicy::Parallel);
```

### SSSP (Delta-Stepping)

```cpp
std::vector<long long> parallel_sssp_delta_stepping(const Graph& g, int source,
                                                     long long delta = 0,
                                                     ExecutionPolicy policy = ExecutionPolicy::Parallel);
```

Delta-Stepping is a parallel-friendly SSSP algorithm. `delta = 0` enables auto-tuning.

### Betweenness Centrality

```cpp
std::vector<double> parallel_betweenness_centrality(const Graph& g,
                                                     ExecutionPolicy policy = ExecutionPolicy::Parallel);
```

Parallelizes across source vertices.

### Label Propagation

```cpp
std::vector<int> parallel_label_propagation(const Graph& g, int max_iterations = 100,
                                             ExecutionPolicy policy = ExecutionPolicy::Parallel);
```

### K-Core Decomposition

```cpp
std::vector<int> parallel_kcore(const Graph& g,
                                 ExecutionPolicy policy = ExecutionPolicy::Parallel);
```

### Graph Contraction

```cpp
Graph parallel_contract(const Graph& g, const std::vector<int>& mapping, int new_n,
                         ExecutionPolicy policy = ExecutionPolicy::Parallel);
```

### Maximal Independent Set (MIS)

```cpp
std::vector<int> parallel_maximal_independent_set(const Graph& g, 
                                                   ExecutionPolicy policy = ExecutionPolicy::Parallel);
```

Finds a maximal independent set such that no two vertices in the set are connected, and no other vertex can be added to the set. Uses Luby's algorithm.

### Graph Coloring

```cpp
std::vector<int> parallel_coloring(const Graph& g, 
                                    ExecutionPolicy policy = ExecutionPolicy::Parallel);
```

Assigns colors to vertices such that no adjacent vertices share the same color. Uses Jones-Plassmann algorithm.


## Parallel Utilities

### Prefix Sum

```cpp
void parallel_prefix_sum(std::vector<long long>& arr,
                          ExecutionPolicy policy = ExecutionPolicy::Parallel);
```

### Parallel For-Each

```cpp
void parallel_for_vertices(const Graph& g, std::function<void(int)> func,
                            ExecutionPolicy policy = ExecutionPolicy::Parallel);

void parallel_for_edges(const Graph& g, std::function<void(int, int, long long)> func,
                         ExecutionPolicy policy = ExecutionPolicy::Parallel);
```

## Example

```cpp
#include <graphlib/graphlib.h>
#include <iostream>

using namespace graphlib;
using namespace graphlib::parallel;

int main() {
    // Configure parallelism
    ParallelConfig config;
    config.num_threads = 8;
    set_parallel_config(config);
    
    // Create large graph
    Graph g = generator::barabasi_albert(100000, 5);
    
    // Convert to CSR for better cache performance
    CSRGraph csr(g);
    
    // Parallel BFS
    auto dist = parallel_bfs(csr, 0);
    
    // Parallel PageRank
    auto pr = parallel_pagerank(g, 0.85, 50);
    
    // Parallel triangle counting
    long long triangles = parallel_triangle_count(g);
    
    // Parallel connected components
    std::vector<int> comp;
    int num_comp = parallel_connected_components(g, comp);
    
    std::cout << "Triangles: " << triangles << "\n";
    std::cout << "Components: " << num_comp << "\n";
    
    return 0;
}
```

## Performance Tips

1. **Use CSRGraph**: Better cache locality for parallel traversal
2. **Grain size**: Increase for small graphs to reduce overhead
3. **Sequential fallback**: Use `ExecutionPolicy::Sequential` for small graphs
4. **Thread count**: Don't exceed physical cores for compute-bound algorithms
5. **Memory**: Parallel algorithms may use more memory for thread-local storage

## Thread Safety

- Graph data structures are **not thread-safe** for concurrent modification
- Parallel algorithms use internal synchronization
- Safe to run multiple parallel algorithms concurrently on **different** graphs
