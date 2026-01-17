#include <graphlib/graphlib.h>
#include <iostream>

int main() {
    using graphlib::GeneralMatching;

    GeneralMatching g(4);

    g.add_edge(0, 1, 5);
    g.add_edge(0, 2, 2);
    g.add_edge(1, 2, 4);
    g.add_edge(1, 3, 3);
    g.add_edge(2, 3, 1);

    int size = g.maximum_matching();
    long long total_weight = g.maximum_weight_matching();

    std::cout << "Maximum matching size  = " << size << "\n";
    std::cout << "Maximum matching weight = " << total_weight << "\n";

    return 0;
}

