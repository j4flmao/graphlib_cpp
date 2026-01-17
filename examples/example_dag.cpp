#include <graphlib/graphlib.h>
#include <iostream>
#include <limits>
#include <vector>

int main() {
    int n = 5;
    graphlib::DAG dag(n);

    dag.add_edge(0, 1);
    dag.add_edge(0, 2);
    dag.add_edge(1, 3);
    dag.add_edge(2, 3);
    dag.add_edge(3, 4);

    bool has_cycle = false;
    auto order = dag.topological_sort_kahn(has_cycle);

    std::cout << "Has cycle: " << (has_cycle ? "true" : "false") << "\n";
    std::cout << "Topological order:";
    for (int v : order) {
        std::cout << " " << v;
    }
    std::cout << "\n";

    long long MINUS_INF = std::numeric_limits<long long>::min() / 4;
    long long INF = std::numeric_limits<long long>::max() / 4;

    auto longest = dag.longest_path(0, MINUS_INF);
    auto shortest = dag.shortest_path(0, INF);
    long long count_paths = dag.count_paths(0, 4);

    std::cout << "Longest distances from 0:\n";
    for (int i = 0; i < n; ++i) {
        std::cout << "longest[ " << i << " ] = " << longest[i] << "\n";
    }

    std::cout << "Shortest distances from 0:\n";
    for (int i = 0; i < n; ++i) {
        std::cout << "shortest[ " << i << " ] = " << shortest[i] << "\n";
    }

    std::cout << "Number of paths from 0 to 4: " << count_paths << "\n";

    return 0;
}

