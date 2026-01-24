# C++ Coding Standards

This document outlines the coding standards and best practices for the GraphLib project. We aim for modern, clean, and efficient C++17 code.

## 1. General Philosophy

- **Standard Version**: Use **C++17**. Features like `std::optional`, structured bindings, and `if constexpr` are encouraged where appropriate.
- **RAII**: Resource Acquisition Is Initialization. Avoid manual memory management (`new`/`delete`). Use `std::unique_ptr`, `std::shared_ptr`, or container classes (`std::vector`).
- **Const Correctness**: Mark methods `const` if they do not modify the object state. Use `const` variables where values are immutable.
- **Performance**: Prefer efficient algorithms and data structures. Avoid unnecessary copies (use `const &` for non-primitive parameters).

## 2. Naming Conventions

We generally follow the C++ Standard Library style (snake_case) for most things, but use PascalCase for types to distinguish them clearly.

| Element | Style | Example |
|---------|-------|---------|
| **Classes / Structs** | PascalCase | `Graph`, `MaxFlow`, `SplayTree` |
| **Functions / Methods** | snake_case | `add_edge`, `max_flow`, `is_connected` |
| **Variables** | snake_case | `edge_count`, `current_node` |
| **Member Variables** | snake_case + trailing underscore | `adj_`, `size_`, `root_` |
| **Constants** | UPPER_CASE | `MAX_VERTICES`, `INF` |
| **Template Types** | PascalCase (often T, U) | `T`, `EdgeType` |
| **Namespaces** | snake_case | `graphlib`, `internal` |

### Rationale
- **PascalCase for Types**: Makes it easy to distinguish types from variables (e.g., `Graph graph;`).
- **Trailing Underscore for Members**: Prevents naming collisions with constructor parameters and getters (e.g., `int size() const { return size_; }`).

## 3. Project Structure

### 3.1 Files
- **Header Files (`.h`)**: Place in `include/graphlib/`. Use standard include guards `#ifndef GRAPHLIB_FILENAME_H`.
- **Source Files (`.cpp`)**: Place in `src/`.
- **Tests**: Place in `tests/`.

### 3.2 Namespaces
- All public code must be inside the `graphlib` namespace.
- Helper functions/classes not meant for public use should be in `graphlib::internal` or an anonymous namespace within the `.cpp` file.

### 3.3 Exporting Symbols
- Use `GRAPHLIB_API` macro for classes and functions that need to be exported from the DLL.
- Headers should include `export.h`.

```cpp
#include "export.h"

namespace graphlib {
    class GRAPHLIB_API MyClass { ... };
}
```

## 4. Modern C++ Features

### 4.1 Auto
- Use `auto` when the type is obvious from the right-hand side (e.g., `auto it = map.begin();`) or when the specific type doesn't matter (e.g., generic lambdas).
- Avoid `auto` in function signatures (unless it's a template) to maintain API clarity.

### 4.2 Structured Bindings
Use structured bindings for returning multiple values (e.g., from `std::pair` or `std::tuple`).

```cpp
// Good
auto [flow, cost] = graph.min_cost_max_flow(s, t);
```

### 4.3 Nullptr
Always use `nullptr`, never `NULL` or `0`.

## 5. Error Handling

- **Exceptions**: Use exceptions (`std::runtime_error`, `std::invalid_argument`) for fatal errors or invariant violations (e.g., constructing a graph with negative vertices).
- **Return Values**: For algorithmic failures that are expected (e.g., "no path found"), use sentinel values (like `-1` or `INF`) or `std::optional`.

## 6. Comments and Documentation

- **Public API**: All public classes and methods should have comments explaining their purpose, parameters, return values, and complexity.
- **Implementation**: Comment complex logic blocks.
- **Style**: Use `//` for single-line comments.

```cpp
/**
 * @brief Computes the shortest path using Dijkstra's algorithm.
 * 
 * Time Complexity: O(E log V)
 * 
 * @param start The starting vertex.
 * @return std::vector<long long> Distances to all vertices.
 */
std::vector<long long> dijkstra(int start) const;
```

## 7. Formatting

- We use **clang-format** to maintain consistent style.
- Indentation: 4 spaces.
- Braces: Attach to the line (K&R / Stroustrup style).

```cpp
void function() {
    if (condition) {
        // do something
    } else {
        // do else
    }
}
```

## 8. Testing

- Every new feature must have accompanying unit tests in `tests/`.
- Use **Google Test** framework.
- Tests should cover:
  - Basic usage.
  - Edge cases (empty graph, single node, disconnected graph).
  - Large inputs (performance check).

## 9. Version Control

- **Commit Messages**: Clear and descriptive.
  - `Feat: Add Dominator Tree implementation`
  - `Fix: Resolve infinite loop in BFS`
  - `Docs: Update README with new examples`
- **Branches**: Use feature branches (`feature/name`) and pull requests.
