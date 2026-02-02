#ifndef GRAPHLIB_GENERAL_MATCHING_H
#define GRAPHLIB_GENERAL_MATCHING_H

#include "export.h"
#include "graph_core.h"

namespace graphlib {

class GRAPHLIB_API GeneralMatching : public Graph {
public:
    explicit GeneralMatching(int n);
    int maximum_matching();
    long long maximum_weight_matching();
    std::vector<int> get_mate() const;

private:
    std::vector<int> match_;
};

}

#endif
