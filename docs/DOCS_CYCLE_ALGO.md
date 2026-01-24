# Minimum Mean Cycle

Finds the cycle with the minimum mean weight (total weight / number of edges) in a directed graph.

Header:
```cpp
#include <graphlib/cycle_algo.h>
```

## 1. Karp's Algorithm

The library implements **Karp's Algorithm**.
- **Complexity**: $O(VE)$.

### Usage

```cpp
#include <graphlib/cycle_algo.h>

double min_mean = 0.0;
std::vector<int> cycle = graphlib::find_min_mean_cycle(g, min_mean);

if (!cycle.empty()) {
    std::cout << "Minimum Mean Weight: " << min_mean << std::endl;
    std::cout << "Cycle: ";
    for (int u : cycle) std::cout << u << " ";
    std::cout << std::endl;
} else {
    std::cout << "No cycle found (DAG)." << std::endl;
}
```

This is particularly useful in discrete event systems and performance analysis of cyclic processes.
