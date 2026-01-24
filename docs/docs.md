# GraphLib – C++ Graph Algorithms Library

This document is a practical guide to the public API of GraphLib. It focuses on how to include headers, construct objects, call algorithms, and interpret results in real projects.

All code samples assume C++17 or later and **0-based vertex indices**.

```cpp
#include <graphlib/graphlib.h>
```

---

## Navigation

- [1. Getting Started](#1-getting-started)
- [2. Core Graph Structure (`graph_core.h`)](#2-core-graph-structure-graph_coreh)
- [3. Maximum Flow and Min-Cost Flow (`max_flow.h`)](#3-maximum-flow-and-min-cost-flow-max_flowh)
- [4. Bipartite Graphs and Assignment (`bipartite.h`)](#4-bipartite-graphs-and-assignment-bipartiteh)
- [5. General Graph Matching (`general_matching.h`)](#5-general-graph-matching-general_matchingh)
- [6. Shortest Paths (`shortest_path.h`)](#6-shortest-paths-shortest_pathh)
- [7. DAG Algorithms (`dag.h`)](#7-dag-algorithms-dagh)
- [8. Minimum Spanning Tree and Union-Find (`mst.h`)](#8-minimum-spanning-tree-and-union-find-msth)
- [9. Connectivity and SCC (`connectivity.h`, `scc.h`)](#9-connectivity-and-scc-connectivityh-scch)
- [10. Splay Tree (`splay_tree.h`)](#10-splay-tree-splay_treeh)
- [11. Tree and LCA (`tree.h`)](#11-tree-and-lca-treeh)
- [12. Advanced Tree Algorithms (`centroid_decomposition.h`, `dominator_tree.h`)](#12-advanced-tree-algorithms-centroid_decompositionh-dominator_treeh)
- [13. Directed MST (`directed_mst.h`)](#13-directed-mst-directed_msth)
- [14. Offline Dynamic Connectivity (`dynamic_connectivity.h`)](#14-offline-dynamic-connectivity-dynamic_connectivityh)
- [15. NP-Hard Algorithms (`np_hard.h`)](#15-np-hard-algorithms-np_hardh)
- [16. Planarity Testing (`planarity.h`)](#16-planarity-testing-planarityh)
- [17. K-Shortest Paths (`k_shortest_paths.h`)](#17-k-shortest-paths-k_shortest_pathsh)
- [18. Chordal Graphs (`chordal.h`)](#18-chordal-graphs-chordalh)
- [19. Graph Isomorphism (`isomorphism.h`)](#19-graph-isomorphism-isomorphismh)
- [20. Graph Measures (`graph_measures.h`)](#20-graph-measures-graph_measuresh)
- [21. Graph Generators (`graph_generator.h`)](#21-graph-generators-graph_generatorh)
- [22. Minimum Cut (`min_cut.h`)](#22-minimum-cut-min_cuth)
- [23. Cycle Algorithms (`cycle_algo.h`)](#23-cycle-algorithms-cycle_algoh)
- [24. Tree Decomposition (`tree_decomposition.h`)](#24-tree-decomposition-tree_decompositionh)
- [25. Master Header and Module Includes](#25-master-header-and-module-includes)

This file focuses on **how to call the code**. For theoretical details, please refer to standard algorithm textbooks (CLRS, etc.).

---

## 1. Getting Started

Use this section if you just installed GraphLib and want a minimal working example.

- **Namespace**: all symbols live in `graphlib`.
- **Single include**: for convenience you can include `graphlib.h`.
- **Per-module includes**: for faster builds, include only what you use.

Typical compile line after installation:

```bash
g++ -std=c++17 -O2 main.cpp -lgraphlib -o main
```

Check library version:

```cpp
#include <graphlib/graphlib.h>
#include <iostream>

int main() {
    std::cout << graphlib::get_version() << "\n";
}
```

---

## 2. Core Graph Structure (`graph_core.h`)

High-level:

- Adjacency-list graph used by several other modules.
- Helper functions for random graphs and basic connectivity checks.
 - Utilities for connectivity, BFS helpers, Eulerian trails/cycles, and min-cut queries.

Main API:

- `Graph`, `make_complete_graph`, `make_random_graph`, `is_connected`, `is_tree`, `make_graph_from_edges`, `make_weighted_graph_from_edges`, `bfs_distances`, `bfs_multi_source`, `has_eulerian_trail_undirected`, `has_eulerian_cycle_undirected`, `eulerian_trail_undirected`, `has_eulerian_trail_directed`, `has_eulerian_cycle_directed`, `eulerian_trail_directed`, `global_min_cut_undirected`, `gomory_hu_tree`.

Detailed documentation and examples: see [DOCS_GRAPH_CORE.md](DOCS_GRAPH_CORE.md).

---

## 3. Maximum Flow and Min-Cost Flow (`max_flow.h`)

High-level:

- Classic max flow (Edmonds–Karp, Dinic).
- Min-cost max flow and min-cost circulation with lower/upper bounds.

Typical use cases:

- Routing, scheduling, capacity planning, transportation with costs.

Main API:

- `MaxFlow`, `CirculationEdge`, `min_cost_circulation`.

Detailed documentation and examples: see [DOCS_MAX_FLOW.md](DOCS_MAX_FLOW.md).

---

## 4. Bipartite Graphs and Assignment (`bipartite.h`)

High-level:

- Bipartite checking and maximum matching.
- Hungarian algorithm for assignment problems.

Typical use cases:

- Matching jobs to workers, tasks to machines, students to projects.

Main API:

- `BipartiteGraph`, `is_bipartite`, `maximum_matching`, `hungarian_min_cost`.

Detailed documentation and examples: see [DOCS_BIPARTITE.md](DOCS_BIPARTITE.md).

---

## 5. General Graph Matching (`general_matching.h`)

High-level:

- Edmonds’ Blossom algorithm for general (non-bipartite) graphs.
- Maximum cardinality and maximum weight matching.

Typical use cases:

- Pairing nodes in arbitrary graphs.
- Optimizing total weight of pairings.

Main API:

- `GeneralMatching`, `maximum_matching`, `maximum_weight_matching`.

Detailed documentation and examples: see [DOCS_GENERAL_MATCHING.md](DOCS_GENERAL_MATCHING.md).

---

## 6. Shortest Paths (`shortest_path.h`)

High-level:

- Single-source and all-pairs shortest path algorithms.
- Supports non-negative and negative edge weights.
- Includes 0–1 BFS, A* search, multi-source Dijkstra, và minimum mean cycle.

Typical use cases:

- Routing, distance queries, cost minimization.
- Multi-source reachability and nearest-facility queries.

Main API:

- `ShortestPath`, `dijkstra`, `zero_one_bfs`, `bellman_ford`, `floyd_warshall`, `johnson`, `minimum_mean_cycle`, `a_star`, `multi_source_dijkstra`.

Detailed documentation and examples: see [DOCS_SHORTEST_PATH.md](DOCS_SHORTEST_PATH.md).

---

## 7. DAG Algorithms (`dag.h`)

High-level:

- Topological sort, longest/shortest paths in DAGs, path counting.
- Online cycle detection when adding edges to maintain DAG invariants.

Typical use cases:

- Task scheduling with dependencies.
- Critical path analysis.
- Interactive tools that must reject operations creating cycles.

Main API:

- `DAG`, `topological_sort_kahn`, `topological_sort_dfs`, `longest_path`, `shortest_path`, `count_paths`, `would_create_cycle`, `add_edge_checked`.

Detailed documentation and examples: see [DOCS_DAG.md](DOCS_DAG.md).

---

## 8. Minimum Spanning Tree and Union-Find (`mst.h`)

High-level:

- Union-Find data structure.
- Kruskal and Prim minimum spanning tree algorithms.

Typical use cases:

- Building minimum-cost networks.
- Connectivity and clustering tasks.

Main API:

- `UnionFind`, `MstEdge`, `MST::kruskal`, `MST::prim`.

Detailed documentation and examples: see [DOCS_MST_UNIONFIND.md](DOCS_MST_UNIONFIND.md).

---

## 9. Connectivity and SCC (`connectivity.h`, `scc.h`)

High-level:

- Connected components, bridges, articulation points.
- Strongly connected components (Kosaraju and Tarjan).
- SCC condensation DAG helper.
- 2-SAT solver built on SCC.
- Dynamic SCC structure with incremental edge insertions.

Typical use cases:

- Reliability analysis, cut vertices/edges, SCC decomposition.

Main API:

- `Connectivity`, `connected_components`, `bridges`, `articulation_points`.
- `SCC`, `kosaraju`, `tarjan`.
- `DynamicSCC`, `add_edge`, `component_count`, `component_id`, `strongly_connected`.
- `build_scc_condensation_dag` (returns `DAG` from SCC components).
- `TwoSAT`, `add_clause`, `add_unit_clause`, `solve`.

Detailed documentation and examples: see [DOCS_CONNECTIVITY_SCC.md](DOCS_CONNECTIVITY_SCC.md).

---

## 10. Splay Tree (`splay_tree.h`)

High-level:

- Self-adjusting binary search tree.
- Supports insert, search, remove, range-sum, split/join.

Typical use cases:

- Dynamic ordered sets with locality of reference.
- Range queries with frequent updates.

Main API:

- `SplayTree`, `insert`, `search`, `remove`, `range_sum`, `split`, `join`.

Detailed documentation and examples: see [DOCS_SPLAY_TREE.md](DOCS_SPLAY_TREE.md).

---

## 11. Tree and LCA (`tree.h`)

High-level:

- Lightweight tree structure built on top of `Graph`.
- Binary lifting LCA queries and distance on trees.

Typical use cases:

- Lowest common ancestor queries in rooted trees.
- Distances on trees in competitive programming problems.

Main API:

- `TreeLCA`, `build`, `lca`, `parent`, `depth`, `distance`, `kth_on_path`, `is_ancestor`, `kth_ancestor`, `subtree_size`, `hld_pos`, `hld_head`, `hld_decompose_path`.
- `TreePathSum`, `query_path`, `query_subtree`, `set_value`, `add_value`, `get_value`.
- `TreePathMax`, `query_path`, `query_subtree`, `set_value`, `get_value`.

Example usage:

```cpp
int n = 5;
graphlib::TreeLCA tree(n);
tree.add_edge(0, 1);
tree.add_edge(1, 2);
tree.add_edge(1, 3);
tree.add_edge(3, 4);

tree.build(0);

int w = tree.lca(2, 4);
int d = tree.distance(2, 4);
```

See `examples/example_lca.cpp` for a contest-style program that reads a tree and answers `(lca, distance)` queries.

---

## 12. Advanced Tree Algorithms (`centroid_decomposition.h`, `dominator_tree.h`)

High-level:

- **Centroid Decomposition**: Divide-and-conquer on trees for path queries ($O(\log n)$ depth).
- **Dominator Tree**: Dominance relationships in flow graphs (Lengauer-Tarjan algorithm).

Typical use cases:

- Path counting/summation constraints (e.g., "count paths with length $L$").
- Compiler optimizations, loop invariant motion, control flow analysis.

Main API:

- `CentroidDecomposition`, `build`, `query_path`.
- `DominatorTree`, `build`, `immediate_dominator`, `dominates`.

Detailed documentation and examples: see [DOCS_ADVANCED_TREE.md](DOCS_ADVANCED_TREE.md).

---

## 13. Directed MST (`directed_mst.h`)

High-level:

- Minimum Spanning Arborescence for directed graphs (Chu-Liu/Edmonds algorithm).
- Finding the cheapest tree rooted at a specific vertex that reaches all other vertices.

Typical use cases:

- Network design where links are directed (e.g., broadcast trees).
- Dependency resolution with costs.

Main API:

- `directed_mst`.

Detailed documentation and examples: see [DOCS_DIRECTED_MST.md](DOCS_DIRECTED_MST.md).

---

## 14. Offline Dynamic Connectivity (`dynamic_connectivity.h`)

High-level:

- Handling edge additions, removals, and connectivity queries over time.
- Uses Segment Tree + DSU with Rollback to solve the problem "offline" (knowing all queries in advance).

Typical use cases:

- Analyzing network reliability over a timeline of events.
- Simulating link failures and restorations.

Main API:

- `DynamicConnectivity`, `add_edge`, `remove_edge`, `query`, `solve`.

Detailed documentation and examples: see [DOCS_DYNAMIC_CONNECTIVITY.md](DOCS_DYNAMIC_CONNECTIVITY.md).

---

## 15. NP-Hard Algorithms (`np_hard.h`)

High-level:

- Exact solvers for TSP and Max Clique (for small/medium graphs).
- Heuristic solver for Graph Coloring.
- Chinese Postman Problem (Route Inspection).

Typical use cases:

- Optimization problems on small datasets.
- Route planning requiring edge coverage.
- Scheduling resources with conflict constraints (coloring).

Main API:

- `tsp_bitmask` (TSP, $N \le 20$).
- `max_clique_bron_kerbosch`.
- `greedy_coloring` (Welsh-Powell).
- `chinese_postman`.

Detailed documentation and examples: see [DOCS_NP_HARD.md](DOCS_NP_HARD.md).

---

## 16. Master Header and Module Includes

High-level:

- `graphlib/graphlib.h` is the single-include header for the entire library.
- Convenient for quick prototypes and small projects.

For larger projects, prefer including only the specific module headers to reduce compile times.

---

## 17. Development Standards

If you plan to contribute to GraphLib or want to understand the coding conventions used:

- **Coding Style**: Naming conventions, formatting, and best practices.
- **Project Structure**: File organization and namespaces.

See [CPP_STANDARDS.md](CPP_STANDARDS.md) for the full guide.

---

## References

API module documents and examples:

- [DOCS_GRAPH_CORE.md](DOCS_GRAPH_CORE.md) – core graph structure and helpers.
- [DOCS_MAX_FLOW.md](DOCS_MAX_FLOW.md) – max flow, min-cost flow, circulation.
- [DOCS_BIPARTITE.md](DOCS_BIPARTITE.md) – bipartite graphs and assignment.
- [DOCS_GENERAL_MATCHING.md](DOCS_GENERAL_MATCHING.md) – general graph matching (Blossom).
- [DOCS_NP_HARD.md](DOCS_NP_HARD.md) – TSP, Max Clique, Coloring, Chinese Postman.
- [DOCS_ADVANCED_ALGO.md](DOCS_ADVANCED_ALGO.md) – Tree Isomorphism, Steiner Tree.
- [DOCS_SHORTEST_PATH.md](DOCS_SHORTEST_PATH.md) – Dijkstra, Bellman-Ford, etc.
- [DOCS_DAG.md](DOCS_DAG.md) – DAG algorithms.
- [DOCS_MST_UNIONFIND.md](DOCS_MST_UNIONFIND.md) – MST and Union-Find.
- [DOCS_CONNECTIVITY_SCC.md](DOCS_CONNECTIVITY_SCC.md) – connectivity and SCC.
- [DOCS_SPLAY_TREE.md](DOCS_SPLAY_TREE.md) – splay tree data structure.
- [DOCS_TREE.md](DOCS_TREE.md) – tree LCA and distance queries.
- [DOCS_ADVANCED_TREE.md](DOCS_ADVANCED_TREE.md) – Centroid Decomposition, Dominator Tree.
- [DOCS_ADVANCED_STRUCTURES.md](DOCS_ADVANCED_STRUCTURES.md) – Link-Cut Tree, DSU with Rollback.
- [DOCS_PLANARITY.md](DOCS_PLANARITY.md) – Planarity check and embedding.
- [DOCS_K_SHORTEST_PATHS.md](DOCS_K_SHORTEST_PATHS.md) – Yen's K-Shortest Paths.
- [DOCS_CHORDAL.md](DOCS_CHORDAL.md) – Chordal graph algorithms.
- [DOCS_ISOMORPHISM.md](DOCS_ISOMORPHISM.md) – VF2 Isomorphism.
- [DOCS_GRAPH_MEASURES.md](DOCS_GRAPH_MEASURES.md) – Centrality and Communities.
- [DOCS_GRAPH_GENERATOR.md](DOCS_GRAPH_GENERATOR.md) – Random and synthetic graphs.
- [DOCS_MIN_CUT.md](DOCS_MIN_CUT.md) – Global min-cut and Gomory-Hu.
- [DOCS_CYCLE_ALGO.md](DOCS_CYCLE_ALGO.md) – Minimum mean cycle.
- [DOCS_TREE_DECOMPOSITION.md](DOCS_TREE_DECOMPOSITION.md) – Tree decomposition.

---

## Examples Index

This table links documentation modules to concrete example programs in the `examples/` directory.

| Docs module | Example file(s) | Description |
|------------|-----------------|-------------|
| `DOCS_GRAPH_CORE.md` | `examples/example_graph_core.cpp` | Core graph usage, connectivity checks, and BFS distances |
| `DOCS_MAX_FLOW.md` | `examples/example_max_flow.cpp` | Basic maximum flow (Edmonds–Karp, Dinic) on a small network |
| `DOCS_MAX_FLOW.md` | `examples/example_min_cost_flow.cpp` | Min-cost max flow with capacities and per-unit costs |
| `DOCS_BIPARTITE.md` | `examples/example_bipartite.cpp` | Bipartite maximum matching and Hungarian minimum-cost assignment |
| `DOCS_GENERAL_MATCHING.md` | `examples/example_general_matching.cpp` | General (non-bipartite) maximum and maximum-weight matching using Blossom |
| `DOCS_SHORTEST_PATH.md` | `examples/example_johnson.cpp`, `examples/example_shortest_path.cpp` | All-pairs and single-source shortest paths (Dijkstra, Bellman-Ford, Johnson) |
| `DOCS_DAG.md` | `examples/example_dag.cpp` | DAG topological sort, longest/shortest paths, and path counting |
| `DOCS_MST_UNIONFIND.md` | `examples/example_mst.cpp` | Kruskal and Prim algorithms |
| `DOCS_SPLAY_TREE.md` | `examples/example_splay_tree.cpp` | Splay tree insert/search/remove and range-sum queries |
| Tree and LCA (`tree.h`) | `examples/example_lca.cpp` | Contest-style LCA and distance queries on a tree |
| `DOCS_CONNECTIVITY_SCC.md` | `examples/example_twosat.cpp` | Two-SAT solver built on SCC and implication graph |
| `DOCS_NP_HARD.md` | `examples/example_np_hard.cpp` | NP-Hard problems (TSP, Max Clique, Coloring, Chinese Postman) |
| `DOCS_ADVANCED_TREE.md` | (Planned) | Centroid Decomposition and Dominator Tree |

Planned examples (to be extended in future iterations):

- Connectivity and SCC condensation DAGs based on `DOCS_CONNECTIVITY_SCC.md`.
