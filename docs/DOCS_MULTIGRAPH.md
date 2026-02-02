# Multigraphs and Hypergraphs

## Overview

This module provides support for multigraphs (multiple edges between same vertices) and hypergraphs (edges connecting multiple vertices).

## Header

```cpp
#include <graphlib/multigraph.h>
```

## Multigraph

### API Reference

```cpp
class Multigraph {
public:
    explicit Multigraph(int n, bool directed = false);
    
    // Returns unique edge ID
    int add_edge(int from, int to, long long weight = 1);
    bool remove_edge(int edge_id);
    int remove_all_edges(int u, int v);
    
    int vertex_count() const;
    int edge_count() const;
    
    const std::vector<MultiEdge>& edges_from(int v) const;
    const MultiEdge* get_edge(int edge_id) const;
    
    int edge_multiplicity(int u, int v) const;
    std::vector<MultiEdge> edges_between(int u, int v) const;
    
    int degree(int v) const;          // Counting multiplicities
    int simple_degree(int v) const;   // Distinct neighbors
    
    bool has_parallel_edges() const;
    bool has_self_loops() const;
    
    int remove_self_loops();
    int simplify(const std::string& keep = "max");  // "min", "max", "sum", "first"
    
    Graph to_simple_graph() const;
    
    bool is_eulerian() const;
    std::vector<int> eulerian_path() const;
    
    long long total_weight() const;
    std::vector<MultiEdge> all_edges() const;
};
```

**MultiEdge**:
```cpp
struct MultiEdge {
    int id;
    int from, to;
    long long weight;
};
```

**Example**:
```cpp
Multigraph mg(3, false);

int e1 = mg.add_edge(0, 1, 10);
int e2 = mg.add_edge(0, 1, 5);   // Parallel edge
int e3 = mg.add_edge(1, 2, 3);
int e4 = mg.add_edge(0, 0, 1);   // Self-loop

std::cout << "Multiplicity (0,1): " << mg.edge_multiplicity(0, 1) << "\n";  // 2
std::cout << "Has self-loops: " << mg.has_self_loops() << "\n";  // true

// Simplify to keep maximum weight
mg.simplify("max");  // Keeps edge with weight 10 between 0-1
```

## Hypergraph

### API Reference

```cpp
class Hypergraph {
public:
    explicit Hypergraph(int n);
    
    int add_hyperedge(const std::vector<int>& vertices, long long weight = 1);
    bool remove_hyperedge(int edge_id);
    
    int vertex_count() const;
    int edge_count() const;
    
    const Hyperedge* get_hyperedge(int edge_id) const;
    std::vector<int> edges_containing(int v) const;
    
    int degree(int v) const;  // Number of hyperedges containing v
    int rank(int edge_id) const;  // Number of vertices in hyperedge
    int max_rank() const;
    
    Graph to_bipartite_expansion() const;
    Graph to_clique_expansion() const;
    Hypergraph dual() const;
    
    bool are_co_members(int u, int v) const;
    int co_membership_count(int u, int v) const;
};
```

**Hyperedge**:
```cpp
struct Hyperedge {
    int id;
    std::vector<int> vertices;
    long long weight;
};
```

**Example**:
```cpp
Hypergraph hg(5);

// Hyperedge connecting vertices 0, 1, 2 (e.g., a group)
int e1 = hg.add_hyperedge({0, 1, 2});

// Another hyperedge connecting 1, 2, 3
int e2 = hg.add_hyperedge({1, 2, 3});

// Co-membership analysis
std::cout << "1 and 2 share: " << hg.co_membership_count(1, 2) << " hyperedges\n";  // 2
std::cout << "0 and 3 share: " << hg.co_membership_count(0, 3) << " hyperedges\n";  // 0
```

### Hypergraph Representations

**Bipartite Expansion**:
```cpp
Graph bipartite = hg.to_bipartite_expansion();
// Vertices 0..n-1: original vertices
// Vertices n..n+m-1: hyperedges
// Edges connect vertices to their hyperedges
```

**Clique Expansion**:
```cpp
Graph clique = hg.to_clique_expansion();
// Each hyperedge becomes a clique among its vertices
```

**Dual Hypergraph**:
```cpp
Hypergraph dual = hg.dual();
// Vertices become hyperedges and vice versa
```

## Use Cases

### Multigraphs

1. **Transportation Networks**: Multiple routes between cities
2. **Communication Networks**: Multiple channels between nodes
3. **Social Networks**: Multiple types of relationships
4. **Financial Networks**: Multiple transactions between entities

### Hypergraphs

1. **Collaboration Networks**: Papers with multiple authors
2. **Database Relations**: Rows connecting multiple attributes
3. **Chemical Compounds**: Molecules as hyperedges on atoms
4. **Social Groups**: Membership in clubs/teams
5. **Set Systems**: Any collection of subsets
