# Offline Dynamic Connectivity

Header: `<graphlib/dynamic_connectivity.h>`

Solves the dynamic connectivity problem (incremental and decremental updates) offline using a Segment Tree over time + DSU with Rollback.
This allows handling edge additions and removals and connectivity queries.

## Class `DynamicConnectivity`

### Constructor
`DynamicConnectivity(int n)`: Initializes with `n` vertices.

### Methods
- `void add_edge(int u, int v)`: Adds an undirected edge between `u` and `v` at the current time step.
- `void remove_edge(int u, int v)`: Removes the undirected edge between `u` and `v` at the current time step.
- `void query(int u, int v)`: Queues a connectivity query between `u` and `v` at the current time step.
- `std::vector<bool> solve()`: Processes all operations and returns a vector containing the boolean result for each query in the order they were added.

### Notes
- The problem is solved "offline", meaning you must provide the sequence of operations (add/remove/query) first, and then call `solve()` to get the answers.
- The implementation uses a Segment Tree where each node represents a time interval. Edges are added to the intervals where they are active. DSU with Rollback is used during the traversal.

## Complexity
O(Q log Q log N) where Q is the number of operations and N is the number of vertices.

## Example

```cpp
#include <graphlib/dynamic_connectivity.h>
#include <iostream>

int main() {
    graphlib::DynamicConnectivity dc(5);
    
    dc.add_edge(0, 1);
    dc.query(0, 1); // True
    
    dc.add_edge(1, 2);
    dc.query(0, 2); // True
    
    dc.remove_edge(0, 1);
    dc.query(0, 2); // False
    
    std::vector<bool> results = dc.solve();
    
    for (bool res : results) {
        std::cout << (res ? "Connected" : "Disconnected") << std::endl;
    }
}
```
