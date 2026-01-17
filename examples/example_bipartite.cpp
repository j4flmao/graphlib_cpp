#include <graphlib/graphlib.h>
#include <iostream>

int main() {
    int n_left = 3;
    int n_right = 3;
    graphlib::BipartiteGraph bg(n_left, n_right);

    bg.add_edge(0, 0);
    bg.add_edge(0, 1);
    bg.add_edge(1, 1);
    bg.add_edge(2, 2);

    bool is_bip = bg.is_bipartite();
    int max_match = bg.maximum_matching();

    long long INF = 1000000000000LL;
    long long min_cost = bg.hungarian_min_cost(INF);

    std::cout << "Is bipartite: " << (is_bip ? "true" : "false") << "\n";
    std::cout << "Maximum matching size: " << max_match << "\n";
    std::cout << "Hungarian minimum cost: " << min_cost << "\n";

    return 0;
}

