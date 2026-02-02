#ifndef GRAPHLIB_RANDOM_WALK_H
#define GRAPHLIB_RANDOM_WALK_H

#include "export.h"
#include "graph_core.h"
#include <vector>
#include <random>

namespace graphlib {

/**
 * @brief Configuration for random walks.
 */
struct RandomWalkConfig {
    int walk_length = 80;           // Length of each random walk
    int num_walks = 10;             // Number of walks per node
    double restart_prob = 0.0;      // Probability of restart (for PageRank-style walks)
    int seed = -1;                  // Random seed (-1 for random device)
};

/**
 * @brief Configuration for Node2Vec.
 */
struct Node2VecConfig : RandomWalkConfig {
    double p = 1.0;  // Return parameter (controls likelihood of revisiting)
    double q = 1.0;  // In-out parameter (controls DFS vs BFS behavior)
    // p < 1: more likely to backtrack (BFS-like)
    // q < 1: more likely to explore outward (DFS-like)
};

/**
 * @brief Generates random walks from a graph.
 * 
 * @param g The graph.
 * @param config Walk configuration.
 * @return std::vector<std::vector<int>> List of walks, each walk is a sequence of vertex IDs.
 */
GRAPHLIB_API std::vector<std::vector<int>> random_walks(const Graph& g, RandomWalkConfig config = RandomWalkConfig());

/**
 * @brief Generates random walks starting from specific nodes.
 * 
 * @param g The graph.
 * @param start_nodes List of starting nodes.
 * @param config Walk configuration.
 * @return std::vector<std::vector<int>> List of walks.
 */
GRAPHLIB_API std::vector<std::vector<int>> random_walks_from(const Graph& g, 
                                                              const std::vector<int>& start_nodes,
                                                              RandomWalkConfig config = RandomWalkConfig());

/**
 * @brief Generates Node2Vec biased random walks.
 * 
 * Node2Vec uses biased random walks that can explore different graph structures:
 * - Low p, high q: BFS-like (local neighborhood)
 * - High p, low q: DFS-like (explore further)
 * 
 * @param g The graph.
 * @param config Node2Vec configuration.
 * @return std::vector<std::vector<int>> List of biased walks.
 */
GRAPHLIB_API std::vector<std::vector<int>> node2vec_walks(const Graph& g, Node2VecConfig config = Node2VecConfig());

/**
 * @brief Computes random walk hitting time from source to target.
 * 
 * Hitting time H(s,t) = expected number of steps for random walk starting at s to reach t.
 * Uses Monte Carlo simulation.
 * 
 * @param g The graph.
 * @param source Source vertex.
 * @param target Target vertex.
 * @param num_simulations Number of Monte Carlo simulations.
 * @param max_steps Maximum steps per simulation.
 * @param seed Random seed.
 * @return double Estimated hitting time, or -1 if target is unreachable.
 */
GRAPHLIB_API double hitting_time(const Graph& g, int source, int target,
                                  int num_simulations = 1000, int max_steps = 10000, int seed = -1);

/**
 * @brief Computes commute time between two vertices.
 * 
 * Commute time C(u,v) = H(u,v) + H(v,u)
 * 
 * @param g The graph.
 * @param u First vertex.
 * @param v Second vertex.
 * @param num_simulations Number of simulations.
 * @param seed Random seed.
 * @return double Estimated commute time.
 */
GRAPHLIB_API double commute_time(const Graph& g, int u, int v,
                                  int num_simulations = 1000, int seed = -1);

/**
 * @brief Computes random walk cover time.
 * 
 * Cover time = expected number of steps for a random walk to visit all vertices.
 * 
 * @param g The graph.
 * @param start_vertex Starting vertex (-1 for random start).
 * @param num_simulations Number of simulations.
 * @param seed Random seed.
 * @return double Estimated cover time, or -1 if graph is disconnected.
 */
GRAPHLIB_API double cover_time(const Graph& g, int start_vertex = -1,
                                int num_simulations = 100, int seed = -1);

/**
 * @brief Computes stationary distribution of random walk.
 * 
 * For undirected graphs: pi_v = degree(v) / (2 * |E|)
 * For directed graphs: uses power iteration.
 * 
 * @param g The graph.
 * @param iterations Number of iterations for power method (directed graphs).
 * @return std::vector<double> Stationary distribution.
 */
GRAPHLIB_API std::vector<double> stationary_distribution(const Graph& g, int iterations = 100);

/**
 * @brief Performs random walk with restart (Personalized PageRank).
 * 
 * @param g The graph.
 * @param source Source vertex (restart point).
 * @param restart_prob Probability of restart (typically 0.15).
 * @param iterations Number of iterations.
 * @return std::vector<double> Personalized PageRank scores.
 */
GRAPHLIB_API std::vector<double> random_walk_with_restart(const Graph& g, int source,
                                                           double restart_prob = 0.15, 
                                                           int iterations = 100);

/**
 * @brief Samples a subgraph using random walks.
 * 
 * @param g The graph.
 * @param sample_size Target number of vertices in sample.
 * @param seed Random seed.
 * @return std::vector<int> Sampled vertex IDs.
 */
GRAPHLIB_API std::vector<int> random_walk_sampling(const Graph& g, int sample_size, int seed = -1);

}

#endif
