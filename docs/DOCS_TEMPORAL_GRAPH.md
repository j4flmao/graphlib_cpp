# Temporal Graph Algorithms

## Overview

This module provides support for temporal (time-varying) graphs where edges have timestamps.

## Header

```cpp
#include <graphlib/temporal_graph.h>
```

## API Reference

### TemporalGraph Class

```cpp
class TemporalGraph {
public:
    explicit TemporalGraph(int n, bool directed = true);
    
    void add_edge(int from, int to, long long timestamp, 
                  long long weight = 1, long long duration = 0);
    void add_edges(const std::vector<TemporalEdge>& edges);
    
    int vertex_count() const;
    int edge_count() const;
    long long min_timestamp() const;
    long long max_timestamp() const;
    
    void sort_by_time();
};
```

**TemporalEdge**:
```cpp
struct TemporalEdge {
    int from, to;
    long long weight;
    long long timestamp;
    long long duration;  // 0 for instantaneous edges
};
```

**Example**:
```cpp
TemporalGraph tg(5, true);
tg.add_edge(0, 1, 100);  // Edge at time 100
tg.add_edge(1, 2, 150);  // Edge at time 150
tg.add_edge(2, 3, 200);  // Edge at time 200
tg.add_edge(0, 3, 120);  // Direct edge at time 120
```

### Snapshots

```cpp
Graph snapshot_at(long long t) const;
Graph snapshot_window(long long t1, long long t2) const;
std::vector<TemporalEdge> edges_in_window(long long t1, long long t2) const;
```

**Example**:
```cpp
// Get graph at time 150
Graph g150 = tg.snapshot_at(150);

// Get graph for time window [100, 200]
Graph window = tg.snapshot_window(100, 200);
```

### Aggregation

```cpp
Graph aggregate(const std::string& aggregation = "sum") const;
```

Aggregates temporal graph to static graph.
- `"sum"`: Sum of edge weights over all occurrences
- `"count"`: Number of occurrences
- `"mean"`, `"max"`, `"min"`: Corresponding statistics

### Temporal Shortest Paths

```cpp
// Earliest arrival time at each vertex
std::vector<long long> earliest_arrival(int source, long long start_time = 0) const;

// Latest possible departure time
std::vector<long long> latest_departure(int target, long long arrive_by) const;

// Fastest path (minimum travel time)
std::pair<long long, long long> fastest_path(int source, int target, long long start_time = 0) const;

// Shortest temporal path (minimum edges)
int shortest_temporal_path(int source, int target, long long start_time = 0) const;
```

**Temporal Path Rules**:
- Each edge's timestamp must be ≥ arrival time at the source vertex
- Travel takes 1 time unit (can be customized)

**Example**:
```cpp
TemporalGraph tg(4, true);
tg.add_edge(0, 1, 10);
tg.add_edge(1, 2, 20);
tg.add_edge(2, 3, 30);

// Earliest arrival starting at time 0
auto arrival = tg.earliest_arrival(0, 0);
std::cout << "Arrive at 3 by: " << arrival[3] << "\n";  // 31
```

### Reachability

```cpp
std::vector<bool> temporal_reachability(int source, long long start_time = 0) const;
```

### Activity Analysis

```cpp
std::vector<long long> vertex_activity(int v) const;
double vertex_burstiness(int v) const;
```

**Burstiness** B = (σ - μ) / (σ + μ):
- B ≈ 1: Bursty activity (clustered events)
- B ≈ -1: Regular activity (evenly spaced)
- B ≈ 0: Random (Poisson-like)

### Sliding Windows

```cpp
std::vector<Graph> sliding_windows(long long window_size) const;
```

Creates sequence of static graph snapshots.

### Temporal SCC

```cpp
int temporal_scc(const TemporalGraph& g, std::vector<int>& component);
```

Temporal strongly connected components: vertices reachable from each other via temporal paths.

### Temporal Motifs

```cpp
std::vector<std::vector<TemporalEdge>> temporal_motifs(const TemporalGraph& g, long long delta);
```

Finds temporal motifs (edge sequences within time window δ).

## Use Cases

1. **Communication Networks**: Analyze email/message patterns
2. **Transportation**: Find temporal routes in transit networks
3. **Epidemiology**: Model disease spread over time
4. **Social Networks**: Study information cascades
5. **Financial Networks**: Detect temporal trading patterns
