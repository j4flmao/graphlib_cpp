#include <graphlib/graphlib.h>
#include <iostream>
#include <utility>

int main() {
    using graphlib::MaxFlow;

    MaxFlow flow(4);

    flow.add_edge(0, 1, 2, 1);
    flow.add_edge(0, 2, 1, 2);
    flow.add_edge(1, 3, 2, 1);
    flow.add_edge(2, 3, 1, 1);

    std::pair<long long, long long> result = flow.min_cost_max_flow(0, 3);

    std::cout << "Total flow: " << result.first << "\n";
    std::cout << "Total cost: " << result.second << "\n";

    return 0;
}

