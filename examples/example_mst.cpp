#include <graphlib/mst.h>
#include <iostream>
#include <vector>
#include <iomanip>

using namespace graphlib;

void example_kruskal() {
    std::cout << "\n=== Kruskal's Algorithm Example ===\n";
    
    int n = 4;
    std::vector<MstEdge> edges = {
        {0, 1, 10},
        {0, 2, 6},
        {0, 3, 5},
        {1, 3, 15},
        {2, 3, 4}
    };

    std::cout << "Graph Edges:\n";
    for (const auto& edge : edges) {
        std::cout << edge.u << " - " << edge.v << " : " << edge.weight << "\n";
    }

    // Note: MST::kruskal sorts the edges in-place
    long long cost = MST::kruskal(n, edges);
    
    std::cout << "Minimum Spanning Tree Cost: " << cost << "\n";
    std::cout << "Edges in MST (sorted by weight):\n";
    
    // To identify which edges are in MST, we can run UnionFind again or modify the API.
    // However, the current API only returns the cost and sorts edges.
    // For visualization, we can show the sorted edges that form the MST manually using UnionFind.
    
    UnionFind uf(n);
    for (const auto& edge : edges) {
        if (uf.unite(edge.u, edge.v)) {
            std::cout << edge.u << " - " << edge.v << " : " << edge.weight << "\n";
        }
    }
}

void example_prim() {
    std::cout << "\n=== Prim's Algorithm Example ===\n";
    
    int n = 5;
    // Adjacency list: u -> {v, weight}
    std::vector<std::vector<std::pair<int, long long>>> adj(n);
    
    auto add_edge = [&](int u, int v, long long w) {
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    };

    add_edge(0, 1, 2);
    add_edge(0, 3, 6);
    add_edge(1, 2, 3);
    add_edge(1, 3, 8);
    add_edge(1, 4, 5);
    add_edge(2, 4, 7);
    add_edge(3, 4, 9);

    std::cout << "Graph with " << n << " nodes.\n";
    long long inf = 1e18;
    long long cost = MST::prim(n, 0, adj, inf);

    std::cout << "Minimum Spanning Tree Cost: " << cost << "\n";
}

int main() {
    std::cout << "GraphLib Minimum Spanning Tree (MST) Examples\n";
    std::cout << "=============================================\n";
    
    example_kruskal();
    example_prim();
    
    return 0;
}
