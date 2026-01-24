# GraphLib – Maximum Flow and Min-Cost Flow (`max_flow.h`)

This document covers all flow-related functionality:

- Maximum flow (Edmonds–Karp, Dinic).
- Min-cost max flow.
- Min-cost circulation with lower/upper bounds.

---

## 1. Overview

Header:

```cpp
#include <graphlib/max_flow.h>
```

Main types:

- `graphlib::MaxFlow` – flow network and algorithms.
- `graphlib::CirculationEdge` – edge description for min-cost circulation.
- `graphlib::min_cost_circulation` – standalone circulation solver.

---

## 2. MaxFlow Class

### 2.1 Construction

```cpp
graphlib::MaxFlow flow(6);
```

- `MaxFlow(int n)`:
  - Creates a flow network with vertices `0..n-1`.

### 2.2 Adding Edges

Capacity-only edges:

```cpp
flow.add_edge(0, 1, 16);
flow.add_edge(0, 2, 13);
```

Capacity + cost edges:

```cpp
flow.add_edge(1, 3, 12, 1); // capacity 12, cost 1 per unit of flow
```

APIs:

- `void add_edge(int from, int to, long long capacity)`:
  - Adds a directed edge with the given capacity and zero cost.
- `void add_edge(int from, int to, long long capacity, long long cost)`:
  - Adds a directed edge with capacity and per-unit cost (for min-cost flow).
- `void add_undirected_edge(int u, int v, long long capacity)`:
  - Adds two directed edges `u -> v` and `v -> u` with the same capacity.
- `void add_undirected_edge(int u, int v, long long capacity, long long cost)`:
  - Adds two directed edges `u -> v` and `v -> u` with the same capacity and cost.

The undirected helpers are convenient when modelling undirected networks in competitive programming tasks.

Constraints and modelling notes:

- Capacities should be non-negative.
- Costs can be positive or zero for standard min-cost flow.
- Negative costs are allowed in some formulations but may introduce negative cycles:
  - Ensure your model does not create unlimited negative-cost cycles.
  - If you suspect issues, start by testing with non-negative costs.

### 2.3 Maximum Flow Algorithms

```cpp
long long value1 = flow.edmonds_karp(0, 5);
long long value2 = flow.dinic(0, 5);
```

- `long long edmonds_karp(int s, int t)`:
  - BFS-based augmenting paths.
  - Simple but slower on large graphs.
- `long long dinic(int s, int t)`:
  - Level graph + blocking flow.
  - Typically much faster for large networks.

### 2.4 Minimum Cut Reachability

After running a max flow algorithm, you can identify the set of vertices reachable from the source in the residual graph. These vertices form the "source side" of a minimum $s-t$ cut.

```cpp
std::vector<char> reachable;
flow.min_cut_reachable_from_source(0, reachable);

if (reachable[2]) {
    // vertex 2 is on the source side of the min cut
}
```

- `void min_cut_reachable_from_source(int source, std::vector<char>& reachable) const`:
  - `reachable` will be resized to `n` and `reachable[i]` will be `1` (true) if `i` is reachable from `source` in the residual graph, `0` otherwise.
  - **Prerequisite**: You must run `edmonds_karp` or `dinic` first to saturate the flow.

Guidelines:

- Prefer `dinic` for performance-critical applications.
- Use `edmonds_karp` if you want a conceptually simpler algorithm.

---

## 3. Min-Cost Max Flow

### 3.1 API

```cpp
std::pair<long long, long long> min_cost_max_flow(int s, int t);
```

- Returns `(total_flow, total_cost)`:
  - `total_flow`: maximum flow value from `s` to `t`.
  - `total_cost`: minimum possible total cost among all max-flow solutions.

### 3.2 Example

```cpp
graphlib::MaxFlow flow(4);

flow.add_edge(0, 1, 2, 1); // capacity 2, cost 1
flow.add_edge(0, 2, 1, 2); // capacity 1, cost 2
flow.add_edge(1, 3, 2, 1);
flow.add_edge(2, 3, 1, 1);

auto result = flow.min_cost_max_flow(0, 3);
long long total_flow = result.first;
long long total_cost = result.second;
```

Typical use cases:

- Assign shipments to multiple routes with different costs.
- Maximize throughput while minimizing energy or monetary cost.

### 3.3 Interpreting the Result

Given:

```cpp
auto [flow_value, total_cost] = flow.min_cost_max_flow(s, t);
```

- `flow_value`:
  - Equals the maximum amount of flow that can be pushed from `s` to `t` given capacities.
- `total_cost`:
  - Sum over all edges of `flow_on_edge * edge_cost`.
  - If you change capacities or costs, both `flow_value` and `total_cost` may change.

Common patterns:

- If you want to enforce an exact required flow `F`:
  - You can:
    - Add an extra edge from `t` back to `s` with capacity `F` and cost 0 (circulation formulation), or
    - Check whether `flow_value == F` after running min-cost max flow.

---

## 4. Min-Cost Circulation with Bounds

Sometimes you want a circulation (flow that satisfies flow conservation at every node) with:

- Lower and upper bounds on edges.
- Costs on edges.

The library exposes:

```cpp
struct CirculationEdge {
    int from;
    int to;
    long long lower;
    long long upper;
    long long cost;
};

bool min_cost_circulation(int n,
                          const std::vector<CirculationEdge>& edges,
                          long long& total_cost);
```

### 4.1 Example

```cpp
#include <graphlib/max_flow.h>
#include <vector>

int n = 3;
std::vector<graphlib::CirculationEdge> edges;

graphlib::CirculationEdge e1;
e1.from = 0;
e1.to = 1;
e1.lower = 1;
e1.upper = 5;
e1.cost = 2;
edges.push_back(e1);

long long total_cost = 0;
bool ok = graphlib::min_cost_circulation(n, edges, total_cost);

if (ok) {
    // feasible circulation found, total_cost holds its minimum cost
}
```

Semantics:

- Each edge `(from, to)` must carry flow `f` satisfying:
  - `lower <= f <= upper`.
- Flow is conserved at every node (total in = total out).
- Among all feasible circulations, total cost `sum(f * cost)` is minimized.

Use cases:

- Supply–demand systems with required minimum and maximum flows.
- Advanced scheduling and resource allocation with constraints.

### 4.2 Modelling Supply and Demand

You can model supply and demand using bounds:

- For a node with net supply `b`:
  - Add an artificial edge from a super-source with lower bound `b` and upper bound `b`.
- For a node with net demand `d`:
  - Add an edge to a super-sink with lower bound `d` and upper bound `d`.

Then call `min_cost_circulation` on the transformed network. This approach:

- Ensures all supply is sent to demand nodes.
- Respects edge capacities and lower bounds.

---

## 5. Tips and Best Practices

- For pure max-flow without costs:
  - Use only `add_edge(from, to, capacity)` and call `dinic`.
- For min-cost max-flow:
  - Always use the cost overload of `add_edge`.
  - Be careful with cost signs; large negative costs may indicate modelling issues.
- For lower/upper bounds:
  - Prefer modelling with `min_cost_circulation` instead of manually transforming the graph.
