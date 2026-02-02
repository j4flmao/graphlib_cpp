#include "graphlib/heuristics.h"
#include <numeric>
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include <limits>
#include <iostream>
#include <map>

namespace graphlib {

namespace {
    // Helper to get edge weight safely
    long long get_weight(const Graph& g, int u, int v) {
        Edge* e = g.get_edges(u);
        long long min_w = std::numeric_limits<long long>::max();
        bool found = false;
        while(e) {
            if(e->to == v) {
                if (e->weight < min_w) min_w = e->weight;
                found = true;
            }
            e = e->next;
        }
        return found ? min_w : std::numeric_limits<long long>::max();
    }

    long long calculate_tour_cost(const Graph& g, const std::vector<int>& tour, const std::vector<std::vector<long long>>& dist_matrix) {
        (void)g;
        long long cost = 0;
        for (size_t i = 0; i < tour.size(); ++i) {
            int u = tour[i];
            int v = tour[(i + 1) % tour.size()];
            long long w = dist_matrix[u][v];
            if (w == std::numeric_limits<long long>::max()) return std::numeric_limits<long long>::max();
            cost += w;
        }
        return cost;
    }
}

GRAPHLIB_API std::pair<long long, std::vector<int>> tsp_simulated_annealing(const Graph& g, TSP_SA_Config config) {
    int n = g.vertex_count();
    if (n < 2) return {0, {0}};
    
    // Precompute distances
    std::vector<std::vector<long long>> dist(n, std::vector<long long>(n));
    for(int i=0; i<n; ++i) {
        for(int j=0; j<n; ++j) {
            if(i==j) dist[i][j] = 0;
            else dist[i][j] = get_weight(g, i, j);
        }
    }

    std::mt19937 rng;
    if (config.seed == -1) {
        rng.seed(static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count()));
    } else {
        rng.seed(config.seed);
    }

    // Initial solution: 0, 1, 2, ..., n-1
    std::vector<int> current_tour(n);
    std::iota(current_tour.begin(), current_tour.end(), 0);
    // Shuffle for random start? Or Nearest Neighbor?
    // Let's shuffle
    std::shuffle(current_tour.begin() + 1, current_tour.end(), rng); // Keep 0 fixed as start

    long long current_cost = calculate_tour_cost(g, current_tour, dist);
    if (current_cost == std::numeric_limits<long long>::max()) return {-1, {}}; // Disconnected

    std::vector<int> best_tour = current_tour;
    long long best_cost = current_cost;

    double temp = config.initial_temperature;

    while (temp > config.min_temperature) {
        for (int i = 0; i < config.max_iterations_per_temp; ++i) {
            // Generate neighbor: Swap two cities (excluding start 0)
            int u = std::uniform_int_distribution<int>(1, n - 1)(rng);
            int v = std::uniform_int_distribution<int>(1, n - 1)(rng);
            while (u == v) {
                v = std::uniform_int_distribution<int>(1, n - 1)(rng);
            }

            // Or 2-OPT reversal? Reversal is often better for TSP.
            // Let's use 2-OPT: reverse segment [u, v]
            if (u > v) std::swap(u, v);
            
            // Calculate delta cost efficiently
            // Old edges: (u-1)->u and v->(v+1)
            // New edges: (u-1)->v and u->(v+1)
            // Need to handle wrap-around indices carefully if we were reversing fully, 
            // but here we just reverse a segment in the linear array.
            
            // For simplicity, just reverse and recalculate full cost for now (safe but slower)
            // Or use delta.
            
            int prev_u = current_tour[u-1];
            int curr_u = current_tour[u];
            int curr_v = current_tour[v];
            int next_v = current_tour[(v + 1) % n];

            long long old_seg_cost = dist[prev_u][curr_u] + dist[curr_v][next_v];
            long long new_seg_cost = dist[prev_u][curr_v] + dist[curr_u][next_v];
            
            // Inside the segment, costs don't change for symmetric TSP (undirected).
            // Assuming undirected graph here.
            
            // Try the move
            std::reverse(current_tour.begin() + u, current_tour.begin() + v + 1);
            long long new_cost = calculate_tour_cost(g, current_tour, dist);
            
            if (new_cost < current_cost) {
                current_cost = new_cost;
                if (current_cost < best_cost) {
                    best_cost = current_cost;
                    best_tour = current_tour;
                }
            } else {
                // Accept with probability
                double diff = static_cast<double>(new_cost - current_cost);
                double p = std::exp(-diff / temp);
                if (std::uniform_real_distribution<double>(0.0, 1.0)(rng) < p) {
                    current_cost = new_cost;
                } else {
                    // Revert
                    std::reverse(current_tour.begin() + u, current_tour.begin() + v + 1);
                }
            }
        }
        temp *= config.cooling_rate;
    }

    return {best_cost, best_tour};
}

GRAPHLIB_API std::pair<long long, std::vector<int>> tsp_ant_colony(const Graph& g, TSP_ACO_Config config) {
    int n = g.vertex_count();
    if (n < 2) return {0, {0}};

    std::mt19937 rng;
    if (config.seed == -1) rng.seed(static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count()));
    else rng.seed(config.seed);

    std::vector<std::vector<long long>> dist(n, std::vector<long long>(n));
    for(int i=0; i<n; ++i) {
        for(int j=0; j<n; ++j) {
            if(i==j) dist[i][j] = 0;
            else dist[i][j] = get_weight(g, i, j);
            if (dist[i][j] == std::numeric_limits<long long>::max()) dist[i][j] = static_cast<long long>(1e9); // Avoid inf
        }
    }

    // Pheromone matrix
    std::vector<std::vector<double>> pheromone(n, std::vector<double>(n, 1.0));

    std::vector<int> best_tour;
    long long best_cost = std::numeric_limits<long long>::max();

    for (int iter = 0; iter < config.max_iterations; ++iter) {
        std::vector<std::vector<int>> ant_tours(config.num_ants);
        std::vector<long long> ant_costs(config.num_ants, 0);

        for (int k = 0; k < config.num_ants; ++k) {
            std::vector<int>& tour = ant_tours[k];
            std::vector<bool> visited(n, false);
            
            int start_node = std::uniform_int_distribution<int>(0, n - 1)(rng);
            tour.push_back(start_node);
            visited[start_node] = true;

            int curr = start_node;
            for (int step = 1; step < n; ++step) {
                // Calculate probabilities
                std::vector<double> probs;
                std::vector<int> candidates;
                double sum_probs = 0.0;

                for (int next = 0; next < n; ++next) {
                    if (!visited[next]) {
                        double tau = pheromone[curr][next];
                        double eta = 1.0 / (double)(dist[curr][next] + 1e-9); // Heuristic
                        double p = std::pow(tau, config.alpha) * std::pow(eta, config.beta);
                        probs.push_back(p);
                        candidates.push_back(next);
                        sum_probs += p;
                    }
                }

                if (candidates.empty()) break; // Should not happen if connected

                // Wheel selection
                double r = std::uniform_real_distribution<double>(0.0, sum_probs)(rng);
                double acc = 0.0;
                int chosen = candidates.back();
                for (size_t i = 0; i < candidates.size(); ++i) {
                    acc += probs[i];
                    if (r <= acc) {
                        chosen = candidates[i];
                        break;
                    }
                }

                tour.push_back(chosen);
                visited[chosen] = true;
                ant_costs[k] += dist[curr][chosen];
                curr = chosen;
            }
            ant_costs[k] += dist[curr][start_node]; // Return to start
        }

        // Evaporate
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                pheromone[i][j] *= (1.0 - config.evaporation_rate);
            }
        }

        // Deposit
        for (int k = 0; k < config.num_ants; ++k) {
            if (ant_tours[k].size() != n) continue;
            
            double delta = config.Q / (double)ant_costs[k];
            for (size_t i = 0; i < ant_tours[k].size(); ++i) {
                int u = ant_tours[k][i];
                int v = ant_tours[k][(i + 1) % n];
                pheromone[u][v] += delta;
                pheromone[v][u] += delta;
            }

            if (ant_costs[k] < best_cost) {
                best_cost = ant_costs[k];
                best_tour = ant_tours[k];
            }
        }
    }

    return {best_cost, best_tour};
}

}
