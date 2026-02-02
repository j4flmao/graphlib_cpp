# Graph I/O

## Overview

This module provides functions for reading and writing graphs in various formats.

## Header

```cpp
#include <graphlib/graph_io.h>
```

All functions are in the `graphlib::io` namespace.

## Supported Formats

| Format | Read | Write | Description |
|--------|------|-------|-------------|
| Edge List | ✅ | ✅ | Simple "from to [weight]" format |
| DIMACS | ✅ | ✅ | Standard format for optimization problems |
| Matrix Market | ✅ | ✅ | Scientific computing sparse matrix format |
| Adjacency Matrix | ✅ | ✅ | Dense matrix format |
| DOT (Graphviz) | ❌ | ✅ | Visualization format |
| CSV | ✅ | ✅ | Comma-separated values |

## API Reference

### Edge List Format

```cpp
// Read from file
GraphReadResult read_edge_list(const std::string& filename, 
                               bool directed = false, 
                               bool one_based = false);

// Read from stream
GraphReadResult read_edge_list(std::istream& input, 
                               bool directed = false, 
                               bool one_based = false);

// Write to file
bool write_edge_list(const Graph& g, const std::string& filename, 
                     WriteOptions options = WriteOptions());
```

**Edge List Format**:
```
# Comment lines start with # or %
n m
0 1 10
0 2 5
1 2 3
```

**Example**:
```cpp
auto result = graphlib::io::read_edge_list("graph.txt", false, true);
if (result.success) {
    Graph& g = result.graph;
    std::cout << "Loaded graph with " << g.vertex_count() << " vertices\n";
}
```

### DIMACS Format

```cpp
GraphReadResult read_dimacs(const std::string& filename);
bool write_dimacs(const Graph& g, const std::string& filename, 
                  const std::string& problem_type = "edge");
```

**DIMACS Format**:
```
c Comment
p sp 5 7
a 1 2 10
a 1 3 5
```

### Matrix Market Format

```cpp
GraphReadResult read_matrix_market(const std::string& filename);
bool write_matrix_market(const Graph& g, const std::string& filename);
```

### DOT Format (Graphviz)

```cpp
bool write_dot(const Graph& g, const std::string& filename, 
               const std::string& graph_name = "G",
               const std::vector<std::string>& node_labels = {});
```

**Example**:
```cpp
Graph g(3, true);
g.add_edge(0, 1, 5);
g.add_edge(1, 2, 3);

graphlib::io::write_dot(g, "graph.dot", "MyGraph", {"A", "B", "C"});
// Output:
// digraph MyGraph {
//   0 [label="A"];
//   1 [label="B"];
//   2 [label="C"];
//   0 -> 1 [label="5", weight=5];
//   1 -> 2 [label="3", weight=3];
// }
```

### CSV Format

```cpp
GraphReadResult read_csv(const std::string& filename, 
                         bool has_header = true,
                         char delimiter = ',', 
                         bool directed = false);

bool write_csv(const Graph& g, const std::string& filename, 
               bool write_header = true, char delimiter = ',');
```

## Utility Functions

```cpp
// Convert to adjacency list
std::vector<std::vector<std::pair<int, long long>>> to_adjacency_list(const Graph& g);

// Create graph from adjacency list
Graph from_adjacency_list(const std::vector<std::vector<std::pair<int, long long>>>& adj, 
                          bool directed = false);

// Get edge list
std::vector<std::tuple<int, int, long long>> to_edge_list(const Graph& g);
```

## Data Structures

### GraphReadResult

```cpp
struct GraphReadResult {
    Graph graph;
    std::vector<std::string> node_labels;
    std::map<std::string, std::vector<double>> node_attributes;
    std::map<std::string, std::vector<double>> edge_attributes;
    bool success;
    std::string error_message;
};
```

### WriteOptions

```cpp
struct WriteOptions {
    bool write_weights = true;
    bool write_labels = true;
    int precision = 6;
    std::string comment_prefix = "#";
};
```
