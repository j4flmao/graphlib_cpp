# 2-SAT Solver

## Overview

The **2-Satisfiability (2-SAT)** problem is a special case of boolean satisfiability where each clause contains exactly two literals. Given a boolean formula in Conjunctive Normal Form (CNF) with clauses of at most 2 literals, 2-SAT determines whether there exists an assignment of truth values to variables that satisfies all clauses.

Unlike general SAT (NP-complete), 2-SAT is solvable in **polynomial time** using graph algorithms.

### Problem Definition

Given variables x₀, x₁, ..., xₙ₋₁ and clauses of the form:
```
(a OR b) AND (c OR d) AND ...
```

Find an assignment of `true`/`false` to each variable such that every clause evaluates to `true`, or determine that no such assignment exists.

## API Reference

### Class: `TwoSat`

**Header:** `#include <graphlib/two_sat.h>`  
**Namespace:** `graphlib`

#### Constructor

```cpp
explicit TwoSat(int n);
```

Creates a 2-SAT solver for `n` boolean variables (0-indexed: 0, 1, ..., n-1).

| Parameter | Description |
|-----------|-------------|
| `n` | Number of boolean variables |

#### Methods

##### `add_clause`

```cpp
void add_clause(int i, bool val_i, int j, bool val_j);
```

Adds a clause: **(xᵢ == val_i) OR (xⱼ == val_j)**

| Parameter | Description |
|-----------|-------------|
| `i` | Index of first variable (0-indexed) |
| `val_i` | Required value of xᵢ for this literal to be true |
| `j` | Index of second variable (0-indexed) |
| `val_j` | Required value of xⱼ for this literal to be true |

**Examples:**
- `add_clause(0, true, 1, false)` → (x₀ OR ¬x₁)
- `add_clause(0, false, 1, true)` → (¬x₀ OR x₁)
- `add_clause(2, true, 3, true)` → (x₂ OR x₃)

##### `add_implication`

```cpp
void add_implication(int i, bool val_i);
```

Adds a unit clause: **(xᵢ == val_i)** must be true.

| Parameter | Description |
|-----------|-------------|
| `i` | Index of the variable (0-indexed) |
| `val_i` | Required value of xᵢ |

**Examples:**
- `add_implication(0, true)` → x₀ must be true
- `add_implication(1, false)` → x₁ must be false

##### `solve`

```cpp
bool solve(std::vector<bool>& assignment);
```

Solves the 2-SAT problem.

| Parameter | Description |
|-----------|-------------|
| `assignment` | Output vector filled with the satisfying assignment |

| Returns | Description |
|---------|-------------|
| `true` | System is satisfiable; `assignment[i]` contains the value of xᵢ |
| `false` | System is unsatisfiable; `assignment` is undefined |

## Complexity

| Operation | Time | Space |
|-----------|------|-------|
| Constructor | O(1) | O(1) |
| `add_clause` | O(1) | O(1) per clause |
| `add_implication` | O(1) | O(1) |
| `solve` | **O(V + E)** | O(V + E) |

Where:
- **V = 2n** (each variable has two nodes: true and false)
- **E** = number of implications (2 per clause)

The solver uses **Kosaraju's or Tarjan's SCC algorithm** internally, achieving linear time complexity.

## Algorithm Description

### Implication Graph

The 2-SAT problem is reduced to a graph problem by constructing an **implication graph**:

1. **Nodes**: For each variable xᵢ, create two nodes:
   - Node `2i` representing xᵢ = false
   - Node `2i + 1` representing xᵢ = true

2. **Edges**: Each clause (a OR b) is equivalent to two implications:
   - ¬a → b (if a is false, b must be true)
   - ¬b → a (if b is false, a must be true)

### Strongly Connected Components (SCC)

The system is **satisfiable** if and only if no variable xᵢ has both its true and false nodes in the same SCC.

**Why?** If xᵢ = true and xᵢ = false are in the same SCC, there's a path from true → false and false → true, creating a contradiction.

### Assignment Extraction

When satisfiable, the assignment is determined by the **topological order** of SCCs:
- If the SCC containing xᵢ = false comes after the SCC containing xᵢ = true, then xᵢ = false
- Otherwise, xᵢ = true

## Usage Examples

### Example 1: Simple Satisfiable System

```cpp
#include <graphlib/two_sat.h>
#include <iostream>

int main() {
    // 3 variables: x0, x1, x2
    graphlib::TwoSat sat(3);
    
    // Clauses:
    // (x0 OR x1)
    sat.add_clause(0, true, 1, true);
    
    // (¬x0 OR x2)
    sat.add_clause(0, false, 2, true);
    
    // (¬x1 OR ¬x2)
    sat.add_clause(1, false, 2, false);
    
    std::vector<bool> assignment;
    if (sat.solve(assignment)) {
        std::cout << "Satisfiable!\n";
        for (int i = 0; i < 3; ++i) {
            std::cout << "x" << i << " = " << (assignment[i] ? "true" : "false") << "\n";
        }
    } else {
        std::cout << "Unsatisfiable\n";
    }
    return 0;
}
```

### Example 2: Unsatisfiable System

```cpp
#include <graphlib/two_sat.h>

int main() {
    graphlib::TwoSat sat(1);
    
    // x0 must be true
    sat.add_implication(0, true);
    
    // x0 must be false
    sat.add_implication(0, false);
    
    std::vector<bool> assignment;
    // Returns false - contradiction!
    bool result = sat.solve(assignment);
    return 0;
}
```

### Example 3: Graph Coloring (2 Colors)

Determine if a graph can be 2-colored (no adjacent vertices share a color):

```cpp
#include <graphlib/two_sat.h>
#include <vector>

bool is_bipartite(int n, const std::vector<std::pair<int,int>>& edges) {
    graphlib::TwoSat sat(n);
    
    for (auto& [u, v] : edges) {
        // If u is color 0, v must be color 1 (and vice versa)
        // (xu == true) OR (xv == true)  - not both false
        sat.add_clause(u, true, v, true);
        // (xu == false) OR (xv == false) - not both true
        sat.add_clause(u, false, v, false);
    }
    
    std::vector<bool> colors;
    return sat.solve(colors);
}
```

### Example 4: Scheduling Constraints

```cpp
#include <graphlib/two_sat.h>

// Schedule n tasks where some pairs conflict
bool schedule(int n, 
              const std::vector<std::pair<int,int>>& conflicts,
              std::vector<bool>& morning_shift) {
    // Variable i = true means task i is in morning, false = afternoon
    graphlib::TwoSat sat(n);
    
    for (auto& [i, j] : conflicts) {
        // Conflicting tasks cannot both be in morning
        sat.add_clause(i, false, j, false);
        // Conflicting tasks cannot both be in afternoon
        sat.add_clause(i, true, j, true);
    }
    
    return sat.solve(morning_shift);
}
```

## See Also

- [Strongly Connected Components](DOCS_SCC.md) - The underlying algorithm used by 2-SAT
- [Directed Graphs](DOCS_DIGRAPH.md) - Graph representation for the implication graph
