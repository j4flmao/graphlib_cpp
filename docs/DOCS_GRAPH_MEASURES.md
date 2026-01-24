# Graph Measures and Analytics

This module provides tools for Social Network Analysis (SNA) and quantifying graph properties.

Header:
```cpp
#include <graphlib/graph_measures.h>
```

## 1. Centrality Measures

These metrics identify the most "important" nodes in a network.

### 1.1 Degree Centrality
Number of connections.
```cpp
auto deg = graphlib::degree_centrality(g);
```

### 1.2 Closeness Centrality
Inverse of the average shortest path distance to all other nodes.
```cpp
auto close = graphlib::closeness_centrality(g);
```

### 1.3 Betweenness Centrality (Brandes' Algorithm)
Quantifies the number of times a node acts as a bridge along the shortest path between two other nodes.
```cpp
auto bet = graphlib::betweenness_centrality(g);
```

### 1.4 Eigenvector Centrality
Measure of influence where connections to high-scoring nodes contribute more.
```cpp
auto eigen = graphlib::eigenvector_centrality(g);
```

### 1.5 PageRank
Google's famous algorithm for ranking web pages.
```cpp
auto rank = graphlib::pagerank(g, 0.85); // Damping factor 0.85
```

### 1.6 Katz Centrality & HITS
- **Katz**: Generalization of degree centrality.
- **HITS**: Hubs and Authorities scores.

## 2. Graph Properties

- **Diameter**: Longest shortest path.
- **Radius**: Minimum eccentricity.
- **Center**: Set of nodes with eccentricity equal to radius.
- **Periphery**: Set of nodes with eccentricity equal to diameter.
- **Jaccard Index / Adamic-Adar**: Link prediction scores.

## 3. Community Detection

### 3.1 Label Propagation Algorithm (LPA)
Fast, near-linear time community detection.
```cpp
auto communities = graphlib::label_propagation_communities(g);
```

### 3.2 Modularity
Calculates the quality of a community partition.
```cpp
double Q = graphlib::modularity(g, communities);
```

## 4. Weisfeiler-Lehman Hash
Generates a structural hash of the graph, useful for fast isomorphism testing.
```cpp
std::string hash = graphlib::weisfeiler_lehman_hash(g, 2); // 2 iterations
```
