# Dominator Tree

## Overview

A **dominator tree** is a fundamental data structure in control flow analysis. In a directed graph with a designated root node (entry point), a node `u` **dominates** node `v` if every path from the root to `v` must pass through `u`. The **immediate dominator** of a node `v` is the closest strict dominator of `v` — the unique node that dominates `v` but does not dominate any other dominator of `v`.

The dominator tree represents the dominance relationships: each node's parent in the tree is its immediate dominator, and the root of the tree is the entry node.

### Key Concepts

| Term | Definition |
|------|------------|
| **Dominates** | Node `u` dominates `v` if all paths from root to `v` go through `u` |
| **Strict Dominator** | `u` strictly dominates `v` if `u` dominates `v` and `u ≠ v` |
| **Immediate Dominator (idom)** | The closest strict dominator of a node |
| **Dominator Tree** | Tree where each node's parent is its immediate dominator |

---

## API Reference

### Class: `DominatorTree`

```cpp
namespace graphlib {

class DominatorTree {
public:
    // Construct dominator tree from graph with given root
    DominatorTree(const Graph& graph, int root);

    // Returns the immediate dominator of u
    // Returns -1 if u is the root or unreachable from root
    int get_idom(int u) const;

    // Returns true if u dominates v
    bool dominates(int u, int v) const;

    // Returns the dominator tree as an adjacency list
    std::vector<std::vector<int>> get_tree() const;
};

} // namespace graphlib
```

### Constructor

```cpp
DominatorTree(const Graph& graph, int root);
```

| Parameter | Description |
|-----------|-------------|
| `graph` | The input directed graph (control flow graph) |
| `root` | The entry node (root) from which dominance is computed |

**Complexity:** O(n α(n)) where n is the number of vertices and α is the inverse Ackermann function.

### Methods

#### `get_idom(int u) const`

Returns the immediate dominator of node `u`.

- Returns `-1` if `u` is the root node
- Returns `-1` if `u` is unreachable from the root

```cpp
int idom = dom_tree.get_idom(5);  // Get immediate dominator of node 5
```

#### `dominates(int u, int v) const`

Checks if node `u` dominates node `v`.

- Returns `true` if `u == v` (every node dominates itself)
- Returns `false` if `v` is unreachable

```cpp
if (dom_tree.dominates(entry, block)) {
    // entry dominates block
}
```

#### `get_tree() const`

Returns the dominator tree as an adjacency list where `tree[u]` contains all nodes immediately dominated by `u`.

```cpp
auto tree = dom_tree.get_tree();
for (int child : tree[root]) {
    // Process children in dominator tree
}
```

---

## The Lengauer-Tarjan Algorithm

The implementation uses the **Lengauer-Tarjan algorithm**, the most efficient known algorithm for computing dominators. It runs in nearly linear time: O(n α(n)) with path compression.

### Algorithm Phases

1. **DFS Numbering**: Perform depth-first search from the root, assigning DFS numbers and recording the DFS tree parent for each node.

2. **Compute Semi-Dominators**: Process nodes in reverse DFS order. For each node `w`, the semi-dominator `sdom(w)` is the node with the smallest DFS number that can reach `w` via a path where all intermediate nodes have DFS numbers greater than `w`.

3. **Compute Immediate Dominators**: Using the semi-dominators and a union-find data structure with path compression, determine the immediate dominator for each node:
   - If `sdom(w) = idom(w)`, then `idom(w) = sdom(w)`
   - Otherwise, `idom(w) = idom(u)` where `u` is found via the union-find structure

4. **Finalize**: A second pass corrects deferred immediate dominator assignments.

### Pseudocode

```
DominatorTree(G, root):
    DFS(root) → compute dfn[], parent[]
    
    for w in reverse DFS order (excluding root):
        for each predecessor v of w:
            u = vertex with min sdom on path to forest root
            sdom[w] = min(sdom[w], sdom[u])
        
        add w to bucket[sdom[w]]
        link(parent[w], w)
        
        for each v in bucket[parent[w]]:
            u = eval(v)
            idom[v] = sdom[u] < sdom[v] ? u : parent[w]
    
    for w in DFS order:
        if idom[w] ≠ sdom[w]:
            idom[w] = idom[idom[w]]
```

---

## Usage Examples

### Basic Usage

```cpp
#include "graphlib/dominator_tree.h"
#include "graphlib/graph_core.h"

int main() {
    // Create a control flow graph
    graphlib::Graph cfg(6);
    
    // Entry → A → B → Exit
    //         ↓   ↑
    //         C → D
    cfg.add_edge(0, 1);  // Entry → A
    cfg.add_edge(1, 2);  // A → B
    cfg.add_edge(1, 3);  // A → C
    cfg.add_edge(3, 4);  // C → D
    cfg.add_edge(4, 2);  // D → B
    cfg.add_edge(2, 5);  // B → Exit
    
    // Build dominator tree from entry node (0)
    graphlib::DominatorTree dom_tree(cfg, 0);
    
    // Query immediate dominators
    int idom_B = dom_tree.get_idom(2);  // Returns 1 (A)
    int idom_D = dom_tree.get_idom(4);  // Returns 3 (C)
    
    // Check dominance relationships
    bool entry_dom_all = dom_tree.dominates(0, 5);  // true: Entry dominates Exit
    bool a_dom_b = dom_tree.dominates(1, 2);        // true: A dominates B
    
    return 0;
}
```

### Traversing the Dominator Tree

```cpp
#include "graphlib/dominator_tree.h"
#include <queue>
#include <iostream>

void print_dominator_tree(const graphlib::DominatorTree& dom_tree, 
                          int root, int n) {
    auto tree = dom_tree.get_tree();
    
    // BFS traversal of dominator tree
    std::queue<std::pair<int, int>> q;  // (node, depth)
    q.push({root, 0});
    
    while (!q.empty()) {
        auto [node, depth] = q.front();
        q.pop();
        
        // Print with indentation
        for (int i = 0; i < depth; ++i) std::cout << "  ";
        std::cout << "Node " << node << "\n";
        
        for (int child : tree[node]) {
            q.push({child, depth + 1});
        }
    }
}
```

### Finding Dominance Frontier

```cpp
// The dominance frontier of a node n is the set of nodes where
// n's dominance ends (useful for SSA construction)
std::vector<std::set<int>> compute_dominance_frontier(
    const graphlib::Graph& cfg,
    const graphlib::DominatorTree& dom_tree,
    int n) 
{
    std::vector<std::set<int>> df(n);
    
    for (int b = 0; b < n; ++b) {
        // Count predecessors
        std::vector<int> preds;
        for (int u = 0; u < n; ++u) {
            for (auto* e = cfg.get_edges(u); e; e = e->next) {
                if (e->to == b) preds.push_back(u);
            }
        }
        
        if (preds.size() >= 2) {
            for (int p : preds) {
                int runner = p;
                while (runner != dom_tree.get_idom(b) && runner != -1) {
                    df[runner].insert(b);
                    runner = dom_tree.get_idom(runner);
                }
            }
        }
    }
    return df;
}
```

---

## Applications

### Compiler Optimization

| Application | Description |
|-------------|-------------|
| **SSA Construction** | Dominator trees determine where to place φ-functions in Static Single Assignment form |
| **Dead Code Elimination** | Code that doesn't dominate any exit is potentially dead |
| **Loop Detection** | Back edges in a CFG point to loop headers that dominate the source |
| **Code Motion** | Instructions can be hoisted to dominating blocks for optimization |

### Program Analysis

- **Control Dependence**: Computing which statements control execution of others
- **Data Flow Analysis**: Dominance information speeds up iterative algorithms
- **Pointer Analysis**: Determining which definitions reach a use

### Security Analysis

- **Taint Analysis**: Tracking if untrusted input dominates security-critical operations
- **Vulnerability Detection**: Finding paths that bypass security checks

### Graph Theory

- **Reachability Queries**: Efficiently answer "must pass through" queries
- **Network Analysis**: Finding critical nodes in directed networks
- **Fault Tolerance**: Identifying single points of failure

---

## Complexity

| Operation | Time Complexity |
|-----------|-----------------|
| Construction | O(n α(n)) ≈ O(n) |
| `get_idom(u)` | O(1) |
| `dominates(u, v)` | O(depth of tree) |
| `get_tree()` | O(n) |

**Space Complexity:** O(n + m) where m is the number of edges.

---

## References

1. Lengauer, T., & Tarjan, R. E. (1979). "A Fast Algorithm for Finding Dominators in a Flowgraph." *ACM Transactions on Programming Languages and Systems*.

2. Cooper, K. D., Harvey, T. J., & Kennedy, K. (2001). "A Simple, Fast Dominance Algorithm." *Software Practice & Experience*.

3. Cytron, R., et al. (1991). "Efficiently Computing Static Single Assignment Form and the Control Dependence Graph." *ACM TOPLAS*.
