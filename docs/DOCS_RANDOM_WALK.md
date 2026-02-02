# Random Walk Algorithms

## Overview

This module provides random walk algorithms for graph sampling, embedding, and analysis.

## Header

```cpp
#include <graphlib/random_walk.h>
```

## API Reference

### Basic Random Walks

```cpp
std::vector<std::vector<int>> random_walks(const Graph& g, 
                                            RandomWalkConfig config = RandomWalkConfig());

std::vector<std::vector<int>> random_walks_from(const Graph& g, 
                                                 const std::vector<int>& start_nodes,
                                                 RandomWalkConfig config = RandomWalkConfig());
```

**Configuration**:
```cpp
struct RandomWalkConfig {
    int walk_length = 80;       // Length of each walk
    int num_walks = 10;         // Walks per node
    double restart_prob = 0.0;  // Probability of restart
    int seed = -1;              // Random seed (-1 for random)
};
```

**Example**:
```cpp
Graph g(100, false);
// ... add edges ...

RandomWalkConfig config;
config.walk_length = 50;
config.num_walks = 5;

auto walks = graphlib::random_walks(g, config);
for (const auto& walk : walks) {
    for (int v : walk) std::cout << v << " ";
    std::cout << "\n";
}
```

### Node2Vec Biased Walks

```cpp
std::vector<std::vector<int>> node2vec_walks(const Graph& g, 
                                              Node2VecConfig config = Node2VecConfig());
```

Node2Vec uses biased random walks controlled by parameters p and q:
- **p** (return parameter): Low p → more likely to backtrack
- **q** (in-out parameter): Low q → explore outward (DFS-like); High q → stay local (BFS-like)

```cpp
struct Node2VecConfig : RandomWalkConfig {
    double p = 1.0;  // Return parameter
    double q = 1.0;  // In-out parameter
};
```

**Example**:
```cpp
Node2VecConfig config;
config.walk_length = 80;
config.num_walks = 10;
config.p = 0.5;  // More local exploration
config.q = 2.0;  // BFS-like behavior

auto walks = graphlib::node2vec_walks(g, config);
```

### Random Walk Metrics

#### Hitting Time

```cpp
double hitting_time(const Graph& g, int source, int target,
                    int num_simulations = 1000, int max_steps = 10000, int seed = -1);
```

Expected number of steps for a random walk from source to reach target.

#### Commute Time

```cpp
double commute_time(const Graph& g, int u, int v,
                    int num_simulations = 1000, int seed = -1);
```

H(u,v) + H(v,u) - expected round-trip time.

#### Cover Time

```cpp
double cover_time(const Graph& g, int start_vertex = -1,
                  int num_simulations = 100, int seed = -1);
```

Expected steps to visit all vertices.

### Stationary Distribution

```cpp
std::vector<double> stationary_distribution(const Graph& g, int iterations = 100);
```

For undirected graphs: π_v = degree(v) / (2|E|)  
For directed graphs: uses power iteration.

### Personalized PageRank

```cpp
std::vector<double> random_walk_with_restart(const Graph& g, int source,
                                              double restart_prob = 0.15, 
                                              int iterations = 100);
```

Random walk that restarts at source with given probability.

### Graph Sampling

```cpp
std::vector<int> random_walk_sampling(const Graph& g, int sample_size, int seed = -1);
```

Sample vertices using random walks (useful for large graphs).

## Use Cases

1. **Graph Embeddings**: Use walks as input to Word2Vec for node embeddings
2. **Link Prediction**: Use hitting/commute time as similarity measures
3. **Community Detection**: Similar nodes have shorter hitting times
4. **Graph Sampling**: Sample large graphs while preserving structure
