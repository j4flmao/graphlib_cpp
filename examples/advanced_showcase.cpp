#include <graphlib/graphlib.h>
#include <graphlib/np_hard.h>
#include <graphlib/clique.h>
#include <graphlib/coloring.h>
#include <graphlib/planarity.h>
#include <graphlib/max_flow.h>
#include <iostream>
#include <vector>

// Showcase of Advanced Graph Algorithms

int main() {
    std::cout << "--- Advanced GraphLib Showcase ---\n\n";

    // 1. TSP on a grid
    std::cout << "[1] Traveling Salesperson (Christofides)\n";
    graphlib::Graph grid(4);
    // 2x2 Square with diagonals (Metric)
    grid.add_edge(0, 1, 10); grid.add_edge(1, 0, 10);
    grid.add_edge(1, 2, 10); grid.add_edge(2, 1, 10);
    grid.add_edge(2, 3, 10); grid.add_edge(3, 2, 10);
    grid.add_edge(3, 0, 10); grid.add_edge(0, 3, 10);
    grid.add_edge(0, 2, 14); grid.add_edge(2, 0, 14); // sqrt(200) ~ 14
    grid.add_edge(1, 3, 14); grid.add_edge(3, 1, 14);
    
    auto tsp_res = graphlib::tsp_christofides(grid);
    std::cout << "TSP Approx Cost: " << tsp_res.first << "\n";
    std::cout << "Path: ";
    for(int u : tsp_res.second) std::cout << u << " ";
    std::cout << "\n\n";
    
    // 2. Max Clique
    std::cout << "[2] Maximum Clique\n";
    graphlib::Graph clique_g(5);
    // Triangle 0-1-2
    clique_g.add_edge(0, 1); clique_g.add_edge(1, 0);
    clique_g.add_edge(1, 2); clique_g.add_edge(2, 1);
    clique_g.add_edge(2, 0); clique_g.add_edge(0, 2);
    // Edge 2-3
    clique_g.add_edge(2, 3); clique_g.add_edge(3, 2);
    
    auto clique = graphlib::maximum_clique(clique_g);
    std::cout << "Max Clique Size: " << clique.size() << " (Vertices: ";
    for(int u : clique) std::cout << u << " ";
    std::cout << ")\n\n";
    
    // 3. Graph Coloring
    std::cout << "[3] Graph Coloring (DSATUR)\n";
    std::vector<int> colors = graphlib::greedy_coloring(clique_g);
    std::cout << "Colors: ";
    for(int c : colors) std::cout << c << " ";
    std::cout << "\n\n";
    
    // 4. Planarity
    std::cout << "[4] Planarity Testing\n";
    // K5 is non-planar
    graphlib::Graph k5(5);
    for(int i=0; i<5; ++i) 
        for(int j=i+1; j<5; ++j) 
            { k5.add_edge(i, j); k5.add_edge(j, i); }
            
    bool is_planar = graphlib::is_planar(k5);
    std::cout << "Is K5 planar? " << (is_planar ? "Yes" : "No") << "\n\n";
    
    // 5. Max Flow
    std::cout << "[5] Max Flow (Dinic)\n";
    graphlib::MaxFlow mf(4);
    mf.add_edge(0, 1, 10);
    mf.add_edge(0, 2, 10);
    mf.add_edge(1, 3, 10);
    mf.add_edge(2, 3, 10);
    mf.add_edge(1, 2, 5); // Cross edge
    
    long long flow = mf.dinic(0, 3);
    std::cout << "Max Flow (0->3): " << flow << "\n";
    
    return 0;
}
