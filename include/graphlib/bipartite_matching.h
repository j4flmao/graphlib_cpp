#ifndef GRAPHLIB_BIPARTITE_MATCHING_H
#define GRAPHLIB_BIPARTITE_MATCHING_H

#include "export.h"
#include <vector>
#include <utility>

namespace graphlib {

/**
 * @brief Class for solving Maximum Bipartite Matching using Hopcroft-Karp algorithm.
 * Time Complexity: O(E * sqrt(V))
 */
class GRAPHLIB_API BipartiteMatching {
public:
    /**
     * @brief Constructs a Bipartite Matching solver.
     * @param n_left Number of vertices in the left partition (0 to n_left-1).
     * @param n_right Number of vertices in the right partition (0 to n_right-1).
     */
    BipartiteMatching(int n_left, int n_right);

    /**
     * @brief Adds an edge between u (left) and v (right).
     * @param u Vertex in left partition.
     * @param v Vertex in right partition.
     */
    void add_edge(int u, int v);

    /**
     * @brief Computes the maximum matching.
     * @return The size of the maximum matching.
     */
    int max_matching();

    /**
     * @brief Returns the matching pairs.
     * @return A vector of pairs (u, v) where u is from left and v is from right.
     */
    std::vector<std::pair<int, int>> get_matching() const;

private:
    int n_left_;
    int n_right_;
    std::vector<std::vector<int>> adj_; // Adjacency list for left vertices
    
    std::vector<int> match_left_;  // match_left_[u] = v (u in left, v in right)
    std::vector<int> match_right_; // match_right_[v] = u (v in right, u in left)
    std::vector<int> dist_;        // Distance array for BFS
    int dist_limit_;               // Max distance for current phase (length of shortest augmenting path)

    bool bfs();
    bool dfs(int u);
};

/**
 * @brief Class for solving Maximum Weight Bipartite Matching (Assignment Problem).
 * Uses Kuhn-Munkres (Hungarian) algorithm.
 * Time Complexity: O(N^3) where N = max(n_left, n_right).
 * 
 * Assumes non-negative weights for "Maximum Weight Matching".
 * Effectively solves the Assignment Problem on a complete bipartite graph,
 * treating missing edges as having weight 0.
 */
class GRAPHLIB_API WeightedBipartiteMatching {
public:
    WeightedBipartiteMatching(int n_left, int n_right);

    /**
     * @brief Adds a weighted edge between u (left) and v (right).
     * @param u Vertex in left partition.
     * @param v Vertex in right partition.
     * @param w Weight of the edge (should be non-negative).
     */
    void add_edge(int u, int v, long long w);

    /**
     * @brief Computes the maximum weight matching.
     * @return The total weight of the matching.
     */
    long long max_weight_matching();

    /**
     * @brief Returns the matching pairs.
     * @return A vector of pairs (u, v) included in the optimal matching.
     */
    std::vector<std::pair<int, int>> get_matching() const;

private:
    int n_left_;
    int n_right_;
    int n_; // max(n_left_, n_right_)
    
    // Use flat vector for matrix to improve cache locality? Or vector<vector>?
    // N <= 1000 usually for O(N^3). 1000x1000 is 1MB long long. Fine.
    std::vector<std::vector<long long>> weights_;
    
    std::vector<int> match_right_; // match_right_[v] = u
    std::vector<long long> lx_, ly_;
    std::vector<long long> slack_;
    std::vector<int> way_; // for tracking path
};

} // namespace graphlib

#endif // GRAPHLIB_BIPARTITE_MATCHING_H
