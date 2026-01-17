#ifndef GRAPHLIB_CONNECTIVITY_H
#define GRAPHLIB_CONNECTIVITY_H

#include "export.h"
#include "graph_core.h"
#include <vector>
#include <utility>

namespace graphlib {

class GRAPHLIB_API Connectivity : public Graph {
public:
    explicit Connectivity(int n);

    int connected_components(std::vector<int>& component);
    void bridges(std::vector<std::pair<int, int>>& result);
    void articulation_points(std::vector<int>& result);
    void biconnected_components(std::vector<std::vector<int>>& components);
};

}

#endif
