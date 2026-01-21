#include <graphlib/shortest_path.h>
#include <iostream>
#include <vector>
#include <limits>
#include <iomanip>

using namespace graphlib;

void print_distances(const std::vector<long long>& dist, long long inf) {
    for (size_t i = 0; i < dist.size(); ++i) {
        std::cout << "Node " << i << ": ";
        if (dist[i] >= inf / 2) {
            std::cout << "INF";
        } else {
            std::cout << dist[i];
        }
        std::cout << "\n";
    }
}

void example_dijkstra() {
    std::cout << "\n=== Dijkstra's Algorithm Example ===\n";
    int n = 5;
    ShortestPath sp(n);
    
    // Graph: 0 -> 1 (4), 0 -> 2 (1)
    //        1 -> 3 (1)
    //        2 -> 1 (2), 2 -> 3 (5)
    //        3 -> 4 (3)
    
    sp.add_edge(0, 1, 4);
    sp.add_edge(0, 2, 1);
    sp.add_edge(1, 3, 1);
    sp.add_edge(2, 1, 2);
    sp.add_edge(2, 3, 5);
    sp.add_edge(3, 4, 3);
    
    long long inf = 1e18;
    std::vector<long long> dist = sp.dijkstra(0, inf);
    
    std::cout << "Shortest distances from Node 0:\n";
    print_distances(dist, inf);
}

void example_bellman_ford() {
    std::cout << "\n=== Bellman-Ford Algorithm Example ===\n";
    int n = 5;
    ShortestPath sp(n);
    
    // Graph with negative edge:
    // 0 -> 1 (5)
    // 1 -> 2 (-2)
    // 1 -> 3 (2)
    // 2 -> 3 (3)
    // 3 -> 4 (1)
    
    sp.add_edge(0, 1, 5);
    sp.add_edge(1, 2, -2);
    sp.add_edge(1, 3, 2);
    sp.add_edge(2, 3, 3);
    sp.add_edge(3, 4, 1);
    
    long long inf = 1e18;
    bool has_neg_cycle = false;
    std::vector<long long> dist = sp.bellman_ford(0, inf, has_neg_cycle);
    
    if (has_neg_cycle) {
        std::cout << "Negative cycle detected!\n";
    } else {
        std::cout << "Shortest distances from Node 0:\n";
        print_distances(dist, inf);
    }
}

void example_floyd_warshall() {
    std::cout << "\n=== Floyd-Warshall Algorithm Example ===\n";
    int n = 4;
    ShortestPath sp(n);
    
    // Graph:
    // 0 -> 1 (5), 0 -> 3 (10)
    // 1 -> 2 (3)
    // 2 -> 3 (1)
    // 3 -> 0 (2) -- makes it cyclic
    
    sp.add_edge(0, 1, 5);
    sp.add_edge(0, 3, 10);
    sp.add_edge(1, 2, 3);
    sp.add_edge(2, 3, 1);
    // sp.add_edge(3, 0, 2);
    
    long long inf = 1e18;
    std::vector<std::vector<long long>> dist_matrix = sp.floyd_warshall(inf);
    
    std::cout << "All-Pairs Shortest Path Matrix:\n";
    std::cout << "      ";
    for(int i=0; i<n; ++i) std::cout << std::setw(6) << i;
    std::cout << "\n";
    
    for (int i = 0; i < n; i++) {
        std::cout << "Node " << i << ":";
        for (int j = 0; j < n; j++) {
            if (dist_matrix[i][j] >= inf / 2) {
                std::cout << std::setw(6) << "INF";
            } else {
                std::cout << std::setw(6) << dist_matrix[i][j];
            }
        }
        std::cout << "\n";
    }
}

int main() {
    std::cout << "GraphLib Shortest Path Examples\n";
    std::cout << "===============================\n";
    
    example_dijkstra();
    example_bellman_ford();
    example_floyd_warshall();
    
    return 0;
}
