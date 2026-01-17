#include <graphlib/graphlib.h>
#include <iostream>

int main() {
    graphlib::MaxFlow flow(6);

    flow.add_edge(0, 1, 16);
    flow.add_edge(0, 2, 13);
    flow.add_edge(1, 2, 10);
    flow.add_edge(1, 3, 12);
    flow.add_edge(2, 1, 4);
    flow.add_edge(2, 4, 14);
    flow.add_edge(3, 2, 9);
    flow.add_edge(3, 5, 20);
    flow.add_edge(4, 3, 7);
    flow.add_edge(4, 5, 4);

    long long ek = flow.edmonds_karp(0, 5);
    long long dinic = flow.dinic(0, 5);

    std::cout << "Edmonds-Karp max flow: " << ek << std::endl;
    std::cout << "Dinic max flow: " << dinic << std::endl;

    return 0;
}

