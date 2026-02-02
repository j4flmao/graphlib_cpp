# Signed Graphs

## Overview

This module provides support for signed graphs where edges can be positive (+) or negative (-), commonly used in social network analysis and balance theory.

## Header

```cpp
#include <graphlib/signed_graph.h>
```

## API Reference

### SignedGraph Class

```cpp
class SignedGraph {
public:
    explicit SignedGraph(int n, bool directed = false);
    
    void add_edge(int from, int to, int sign, long long weight = 1);
    void add_positive_edge(int from, int to, long long weight = 1);
    void add_negative_edge(int from, int to, long long weight = 1);
    
    int vertex_count() const;
    int get_sign(int u, int v) const;  // Returns +1, -1, or 0 (no edge)
};
```

**Example**:
```cpp
SignedGraph sg(4, false);
sg.add_positive_edge(0, 1);  // Friends
sg.add_positive_edge(1, 2);  // Friends
sg.add_negative_edge(0, 2);  // Enemies
sg.add_positive_edge(2, 3);
```

### Balance Theory

A signed graph is **balanced** if it can be partitioned into two groups where:
- All positive edges are within groups
- All negative edges are between groups

```cpp
bool is_balanced() const;
bool find_balance_partition(std::vector<int>& partition) const;
```

**Weak Balance**: Can be partitioned into any number of groups with the same property.

```cpp
bool is_weakly_balanced() const;
```

### Frustration Index

```cpp
int frustration_index() const;
```

Minimum number of edges to remove/flip to make the graph balanced.

### Triangle Analysis

```cpp
std::pair<long long, long long> count_signed_triangles() const;

void triangle_analysis(long long& balanced_ppp, long long& balanced_pnn,
                       long long& unbalanced_ppn, long long& unbalanced_nnn) const;
```

**Triangle Types**:
- **PPP** (all positive): Balanced ✓
- **PNN** (1 positive, 2 negative): Balanced ✓
- **PPN** (2 positive, 1 negative): Unbalanced ✗
- **NNN** (all negative): Unbalanced ✗

### Signed Clustering

```cpp
double signed_clustering_coefficient(int v) const;
```

Ratio of balanced triangles at vertex v.

### Status and Ranking

```cpp
std::vector<double> status_scores() const;
std::vector<double> signed_pagerank(double d = 0.85, int iterations = 100) const;
```

### Community Detection

```cpp
std::vector<int> polarized_communities() const;
```

Finds communities that minimize negative edges within and positive edges between.

### Sign Prediction

```cpp
int predict_sign(int u, int v) const;
```

Predicts edge sign based on balance theory (triadic closure).

### Subgraph Extraction

```cpp
Graph to_unsigned_graph() const;
Graph positive_subgraph() const;
Graph negative_subgraph() const;
```

### Factory Function

```cpp
SignedGraph make_signed_graph(int n,
    const std::vector<std::pair<int, int>>& trust_edges,
    const std::vector<std::pair<int, int>>& distrust_edges);
```

## Use Cases

### Social Networks

```cpp
SignedGraph social(5);
// Alice (0) trusts Bob (1)
social.add_positive_edge(0, 1);
// Bob trusts Carol (2)
social.add_positive_edge(1, 2);
// Alice distrusts Carol
social.add_negative_edge(0, 2);

// This is balanced: Alice and Bob in one group, Carol in another
std::cout << "Balanced: " << social.is_balanced() << "\n";

// Predict: Will Bob trust Dave if Alice distrusts Dave?
social.add_negative_edge(0, 3);  // Alice distrusts Dave
int predicted = social.predict_sign(1, 3);  // Predicts -1 (distrust)
```

### Correlation Networks

```cpp
// Gene expression: positive = co-expressed, negative = anti-correlated
SignedGraph genes(100);
// Add edges based on correlation signs
```

## Applications

1. **Social Network Analysis**: Friend/enemy relationships
2. **Political Networks**: Alliance/conflict patterns
3. **Correlation Networks**: Positive/negative correlations
4. **Recommender Systems**: Like/dislike relationships
5. **Biological Networks**: Activation/inhibition in gene networks
