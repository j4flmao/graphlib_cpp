#ifndef GRAPHLIB_HEURISTICS_H
#define GRAPHLIB_HEURISTICS_H

#include "export.h"
#include "graph_core.h"
#include <vector>
#include <utility>

namespace graphlib {

/**
 * @brief Configuration for Simulated Annealing TSP.
 */
struct TSP_SA_Config {
    double initial_temperature = 1000.0;
    double cooling_rate = 0.995;
    double min_temperature = 0.001;
    int max_iterations_per_temp = 100;
    int seed = -1; // -1 for random
};

/**
 * @brief Solves TSP using Simulated Annealing.
 * Suitable for larger graphs where exact bitmask DP is too slow.
 * 
 * @param g The graph (assumed complete/connected).
 * @param config Configuration parameters.
 * @return Pair of {min_cost, path}.
 */
GRAPHLIB_API std::pair<long long, std::vector<int>> tsp_simulated_annealing(const Graph& g, TSP_SA_Config config = TSP_SA_Config());

/**
 * @brief Configuration for Ant Colony Optimization TSP.
 */
struct TSP_ACO_Config {
    int num_ants = 20;
    int max_iterations = 100;
    double alpha = 1.0; // Pheromone importance
    double beta = 2.0;  // Heuristic (distance) importance
    double evaporation_rate = 0.1;
    double Q = 100.0;   // Pheromone deposit amount
    int seed = -1;
};

/**
 * @brief Solves TSP using Ant Colony Optimization (ACO).
 * 
 * @param g The graph.
 * @param config Configuration parameters.
 * @return Pair of {min_cost, path}.
 */
GRAPHLIB_API std::pair<long long, std::vector<int>> tsp_ant_colony(const Graph& g, TSP_ACO_Config config = TSP_ACO_Config());

}

#endif
