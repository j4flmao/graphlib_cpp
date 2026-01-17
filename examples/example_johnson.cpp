#include <graphlib/graphlib.h>
#include <iostream>
#include <limits>
#include <vector>

int main() {
    int n = 4;
    graphlib::ShortestPath sp(n);

    sp.add_edge(0, 1, 1);
    sp.add_edge(0, 2, 5);
    sp.add_edge(1, 2, -2);
    sp.add_edge(1, 3, 4);
    sp.add_edge(2, 3, 1);

    long long INF = std::numeric_limits<long long>::max() / 4;
    bool has_negative_cycle = false;

    std::vector<std::vector<long long>> dist = sp.johnson(INF, has_negative_cycle);

    if (has_negative_cycle) {
        std::cout << "Negative cycle detected\n";
        return 0;
    }

    std::cout << "All-pairs shortest paths (Johnson):\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (dist[i][j] >= INF / 2) {
                std::cout << "INF ";
            } else {
                std::cout << dist[i][j] << " ";
            }
        }
        std::cout << "\n";
    }

    return 0;
}

