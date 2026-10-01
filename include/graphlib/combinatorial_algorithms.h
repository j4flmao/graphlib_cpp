#ifndef GRAPHLIB_COMBINATORIAL_ALGORITHMS_H
#define GRAPHLIB_COMBINATORIAL_ALGORITHMS_H
#include "export.h"
#include <utility>
#include <vector>
namespace graphlib {
struct GRAPHLIB_API FunctionalGraphInfo {
    std::vector<int> cycle_id;
    std::vector<int> distance_to_cycle;
    std::vector<int> cycle_lengths;
};
GRAPHLIB_API FunctionalGraphInfo analyze_functional_graph(const std::vector<int>& successor);
GRAPHLIB_API bool is_graphical_degree_sequence(const std::vector<int>& degrees);
GRAPHLIB_API std::vector<std::pair<int,int>> realize_degree_sequence(const std::vector<int>& degrees);
}
#endif
