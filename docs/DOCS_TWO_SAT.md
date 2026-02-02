# 2-Satisfiability

## Overview
Solves the **2-SAT** problem: given a set of boolean clauses where each clause acts on two literals (e.g., `A OR !B`), find a satisfying assignment.

## Header
```cpp
#include <graphlib/two_sat.h>
```

## Class `TwoSat`

### `TwoSat(int n)`
Initializes for `n` variables (0 to n-1).

### `add_clause(i, val_i, j, val_j)`
Adds constraint `(variable_i == val_i OR variable_j == val_j)`.
- `val` is boolean (`true` or `false`).
- Equivalent to standard CNF clause.

### `add_implication(i, val_i)`
Forces `variable_i` to be `val_i`.

### `solve(assignment)`
- Returns `true` if satisfiable.
- Fills `assignment` vector with boolean values.
- Complexity: O(N + M) (Linear time).

## Example

```cpp
#include <graphlib/graphlib.h>
#include <graphlib/two_sat.h>
#include <iostream>

int main() {
    // Solve: (x0 OR x1) AND (!x0 OR x2) AND (!x1 OR !x2)
    graphlib::TwoSat sat(3);
    
    sat.add_clause(0, true, 1, true);   // x0 OR x1
    sat.add_clause(0, false, 2, true);  // !x0 OR x2
    sat.add_clause(1, false, 2, false); // !x1 OR !x2
    
    std::vector<bool> result;
    if (sat.solve(result)) {
        std::cout << "Satisfiable: ";
        for(bool b : result) std::cout << (b ? "T" : "F");
        std::cout << "\n";
    } else {
        std::cout << "Unsatisfiable\n";
    }
}
```
