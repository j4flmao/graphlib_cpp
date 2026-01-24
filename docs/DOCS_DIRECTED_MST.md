# Directed Minimum Spanning Tree (Minimum Spanning Arborescence)

Header: `<graphlib/directed_mst.h>`

Computes the Minimum Spanning Arborescence (MST for directed graphs) rooted at a specific vertex using the Chu-Liu/Edmonds algorithm.

## Function `directed_mst`

```cpp
long long directed_mst(int n, int root, const std::vector<DirectedEdge>& edges, std::vector<int>& result_edges);
```

### Parameters
- `n`: Number of vertices.
- `root`: The root of the arborescence (0-based index).
- `edges`: List of directed edges.
- `result_edges`: Output vector to store indices of edges in the MST. **Note**: Edge reconstruction is currently experimental; this vector may be empty or incomplete in the current version. The return value (cost) is fully supported.

### Return Value
- Returns the total weight of the minimum spanning arborescence.
- Returns `-1` if no such arborescence exists (i.e., not all nodes are reachable from the root).

### Complexity
O(VE) in the worst case, or O(E log V) with efficient implementation (current implementation is O(VE)).

## Example

```cpp
#include <graphlib/directed_mst.h>
#include <iostream>
#include <vector>

int main() {
    int n = 4;
    int root = 0;
    std::vector<graphlib::DirectedEdge> edges = {
        {0, 1, 10, 0},
        {0, 2, 10, 1},
        {1, 3, 20, 2},
        {2, 3, 30, 3}
    };
    std::vector<int> res_edges;
    long long cost = graphlib::directed_mst(n, root, edges, res_edges);
    
    if (cost != -1) {
        std::cout << "MST Cost: " << cost << std::endl;
    } else {
        std::cout << "No MST exists" << std::endl;
    }
}
```
