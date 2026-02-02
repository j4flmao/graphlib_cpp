#include "graphlib/graph_io.h"
#include <fstream>
#include <iostream>

namespace graphlib {

GRAPHLIB_API bool save_to_dot(const Graph& g, const std::string& filename) {
    std::vector<std::string> labels;
    int n = g.vertex_count();
    for(int i=0; i<n; ++i) {
        labels.push_back(std::to_string(i));
    }
    return save_to_dot_labeled(g, filename, labels);
}

GRAPHLIB_API bool save_to_dot_labeled(const Graph& g, const std::string& filename, const std::vector<std::string>& labels) {
    std::ofstream out(filename);
    if (!out.is_open()) return false;
    
    bool directed = g.is_directed();
    
    if (directed) {
        out << "digraph G {\n";
    } else {
        out << "graph G {\n";
    }
    
    int n = g.vertex_count();
    for(int i=0; i<n; ++i) {
        std::string label = (i < (int)labels.size()) ? labels[i] : std::to_string(i);
        out << "    " << i << " [label=\"" << label << "\"];\n";
    }
    
    // For undirected graphs stored as directed adj list, we want to avoid duplicate edges.
    // E.g. 0-1 and 1-0. Print only if u < v.
    
    for(int u=0; u<n; ++u) {
        for(Edge* e = g.get_edges(u); e; e = e->next) {
            int v = e->to;
            if (!directed) {
                if (u < v) {
                    out << "    " << u << " -- " << v;
                    if (e->weight != 1) {
                         out << " [label=\"" << e->weight << "\"]";
                    }
                    out << ";\n";
                }
            } else {
                out << "    " << u << " -> " << v;
                if (e->weight != 1) {
                     out << " [label=\"" << e->weight << "\"]";
                }
                out << ";\n";
            }
        }
    }
    
    out << "}\n";
    out.close();
    return true;
}

}
