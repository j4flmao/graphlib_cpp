#include <graphlib/graphlib.h>
#include <graphlib/np_hard.h>
#include <iostream>
#include <vector>
#include <iomanip>

void demo_tsp() {
    std::cout << "--- Traveling Salesperson Problem (TSP) ---\n";
    // 0 --(10)--> 1
    // 1 --(15)--> 2
    // 2 --(20)--> 0
    // 0 --(35)--> 2 (direct)
    graphlib::Graph g(3);
    g.add_edge(0, 1, 10); g.add_edge(1, 0, 10);
    g.add_edge(1, 2, 15); g.add_edge(2, 1, 15);
    g.add_edge(2, 0, 20); g.add_edge(0, 2, 20);
    
    // TSP cost should be 10 + 15 + 20 = 45
    long long cost = graphlib::tsp_bitmask(g);
    std::cout << "Minimum TSP cost for 3-cycle (10, 15, 20): " << cost << "\n\n";
}

void demo_max_clique() {
    std::cout << "--- Maximum Clique ---\n";
    // K4 (complete graph of 4 vertices) plus one loose vertex
    graphlib::Graph g(5, false);
    
    // Make 0,1,2,3 a clique
    for(int i=0; i<4; ++i) {
        for(int j=i+1; j<4; ++j) {
            g.add_edge(i, j);
        }
    }
    // Connect 4 to 0 only
    g.add_edge(4, 0);
    
    int size = graphlib::max_clique_bron_kerbosch(g);
    std::cout << "Max clique size (expected 4): " << size << "\n\n";
}

void demo_coloring() {
    std::cout << "--- Graph Coloring ---\n";
    // A simple cycle 0-1-2-3-4-0 (5 vertices) needs 3 colors
    graphlib::Graph g(5, false);
    for(int i=0; i<5; ++i) {
        g.add_edge(i, (i+1)%5);
    }
    
    std::vector<int> colors;
    int num_colors = graphlib::greedy_coloring(g, colors);
    std::cout << "Number of colors used for C5: " << num_colors << "\n";
    for(int i=0; i<5; ++i) {
        std::cout << "Vertex " << i << ": Color " << colors[i] << "\n";
    }
    std::cout << "\n";
}

void demo_chinese_postman() {
    std::cout << "--- Chinese Postman Problem ---\n";
    // A single edge 0-1 with weight 10.
    // Degrees: deg(0)=1, deg(1)=1. Both odd.
    // Must traverse 0-1 twice.
    // Total weight = 10. Added path = 10. Result = 20.
    graphlib::Graph g(2, false);
    g.add_edge(0, 1, 10);
    
    long long cost = graphlib::chinese_postman(g);
    std::cout << "CPP cost for single edge weight 10 (expected 20): " << cost << "\n\n";
}

int main() {
    std::cout << "GraphLib NP-Hard Algorithms Demo\n";
    std::cout << "================================\n\n";
    
    demo_tsp();
    demo_max_clique();
    demo_coloring();
    demo_chinese_postman();
    
    return 0;
}
