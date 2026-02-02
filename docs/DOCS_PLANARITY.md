# Planarity Testing & Embedding

## Overview
This module determines if a graph is **planar** (can be drawn on a plane without edge crossings) and, if so, computes a **planar embedding** (the set of faces).

## Header
```cpp
#include <graphlib/planarity.h>
```

## Functions

### `is_planar`
```cpp
/**
 * @brief Checks if the graph is planar.
 * @param g The input graph.
 * @return true if planar, false otherwise.
 */
bool is_planar(const Graph& g);
```
- **Complexity**: O(N) (linear time).
- Uses a path addition algorithm (simplified Demoucron/Boyer-Myrvold hybrid).

### `get_planar_faces`
```cpp
/**
 * @brief Computes the faces of a planar embedding.
 * @param g The input graph.
 * @return A vector of faces. Each face is a vector of vertex IDs ordered cyclically.
 *         Returns an empty vector if the graph is not planar.
 */
std::vector<std::vector<int>> get_planar_faces(const Graph& g);
```
- Returns the list of faces.
- For a connected planar graph, `|Faces| = |Edges| - |Vertices| + 2` (Euler's formula).

## Example
```cpp
#include <graphlib/graphlib.h>
#include <graphlib/planarity.h>
#include <iostream>

int main() {
    graphlib::Graph g(4);
    // K4 is planar
    g.add_edge(0, 1); g.add_edge(1, 2); g.add_edge(2, 0); // Face 1
    g.add_edge(0, 3); g.add_edge(1, 3); g.add_edge(2, 3); // More faces
    
    if (graphlib::is_planar(g)) {
        std::cout << "Planar!\n";
        auto faces = graphlib::get_planar_faces(g);
        std::cout << "Number of faces: " << faces.size() << "\n";
        for(const auto& face : faces) {
            std::cout << "Face: ";
            for(int v : face) std::cout << v << " ";
            std::cout << "\n";
        }
    } else {
        std::cout << "Not planar.\n";
    }
}
```
