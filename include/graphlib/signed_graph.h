#ifndef GRAPHLIB_SIGNED_GRAPH_H
#define GRAPHLIB_SIGNED_GRAPH_H

#include "export.h"
#include "graph_core.h"
#include <vector>
#include <utility>

namespace graphlib {

/**
 * @brief Signed Graph where edges can be positive (+) or negative (-).
 * 
 * Used in social network analysis (friend/enemy relationships),
 * correlation networks, and balance theory applications.
 */
class GRAPHLIB_API SignedGraph {
private:
    int n_;
    struct SignedEdge {
        int to;
        int sign;  // +1 or -1
        long long weight;  // Absolute weight
        SignedEdge* next;
        
        SignedEdge(int to, int sign, long long weight);
    };
    SignedEdge** adj_;
    bool directed_;
    
    void clear_adj();

public:
    explicit SignedGraph(int n, bool directed = false);
    ~SignedGraph();
    
    SignedGraph(const SignedGraph&) = delete;
    SignedGraph& operator=(const SignedGraph&) = delete;
    SignedGraph(SignedGraph&& other) noexcept;
    SignedGraph& operator=(SignedGraph&& other) noexcept;
    
    /**
     * @brief Adds a signed edge.
     * 
     * @param from Source vertex.
     * @param to Target vertex.
     * @param sign +1 for positive edge, -1 for negative edge.
     * @param weight Absolute weight of the edge.
     */
    void add_edge(int from, int to, int sign, long long weight = 1);
    
    /**
     * @brief Adds a positive edge.
     */
    void add_positive_edge(int from, int to, long long weight = 1) {
        add_edge(from, to, +1, weight);
    }
    
    /**
     * @brief Adds a negative edge.
     */
    void add_negative_edge(int from, int to, long long weight = 1) {
        add_edge(from, to, -1, weight);
    }
    
    int vertex_count() const { return n_; }
    bool is_directed() const { return directed_; }
    
    /**
     * @brief Gets the sign of edge (u, v).
     * 
     * @return 1 for positive, -1 for negative, 0 if edge doesn't exist.
     */
    int get_sign(int u, int v) const;
    
    /**
     * @brief Checks if the graph is structurally balanced.
     * 
     * A signed graph is balanced if vertices can be partitioned into two sets
     * such that positive edges are within sets and negative edges are between sets.
     * 
     * Equivalent to: no cycle with odd number of negative edges.
     * 
     * @return true if balanced.
     */
    bool is_balanced() const;
    
    /**
     * @brief Checks if the graph is weakly balanced.
     * 
     * A signed graph is weakly balanced if it can be partitioned into k sets
     * (for any k) with the balance property.
     * 
     * Equivalent to: no cycle of length 3 with exactly 1 or 2 negative edges.
     * 
     * @return true if weakly balanced.
     */
    bool is_weakly_balanced() const;
    
    /**
     * @brief Finds the balance partition (if balanced).
     * 
     * @param partition Output: partition[v] = 0 or 1 indicating which set v belongs to.
     * @return true if graph is balanced and partition was found.
     */
    bool find_balance_partition(std::vector<int>& partition) const;
    
    /**
     * @brief Computes the frustration index.
     * 
     * The minimum number of edges that need to be removed (or sign-flipped)
     * to make the graph balanced.
     * 
     * This is NP-hard; uses heuristics for large graphs.
     * 
     * @return int Frustration index.
     */
    int frustration_index() const;
    
    /**
     * @brief Computes signed clustering coefficient.
     * 
     * Considers both positive and negative triangles.
     * A balanced triangle has 0 or 2 negative edges.
     * 
     * @param v Vertex.
     * @return double Signed clustering coefficient.
     */
    double signed_clustering_coefficient(int v) const;
    
    /**
     * @brief Computes status scores based on signed edges.
     * 
     * In social networks, positive in-edges increase status,
     * negative in-edges decrease it.
     * 
     * @return std::vector<double> Status score for each vertex.
     */
    std::vector<double> status_scores() const;
    
    /**
     * @brief Gets counts of positive and negative triangles.
     * 
     * @return Pair of {positive_triangles, negative_triangles}.
     */
    std::pair<long long, long long> count_signed_triangles() const;
    
    /**
     * @brief Computes signed PageRank.
     * 
     * Positive edges contribute positively to rank, negative edges negatively.
     * 
     * @param d Damping factor.
     * @param iterations Number of iterations.
     * @return std::vector<double> Signed PageRank scores.
     */
    std::vector<double> signed_pagerank(double d = 0.85, int iterations = 100) const;
    
    /**
     * @brief Detects polarized communities using signed spectral clustering.
     * 
     * Tries to find communities that minimize negative edges within communities
     * and positive edges between communities.
     * 
     * @return std::vector<int> Community assignment for each vertex.
     */
    std::vector<int> polarized_communities() const;
    
    /**
     * @brief Converts to unsigned Graph (ignoring signs).
     */
    Graph to_unsigned_graph() const;
    
    /**
     * @brief Gets the positive subgraph only.
     */
    Graph positive_subgraph() const;
    
    /**
     * @brief Gets the negative subgraph only.
     */
    Graph negative_subgraph() const;
    
    /**
     * @brief Predicts sign of a potential edge using balance theory.
     * 
     * @param u First vertex.
     * @param v Second vertex.
     * @return Predicted sign: +1, -1, or 0 if cannot predict.
     */
    int predict_sign(int u, int v) const;
    
    /**
     * @brief Computes the number of balanced and unbalanced triangles.
     */
    void triangle_analysis(long long& balanced_ppp, long long& balanced_pnn,
                          long long& unbalanced_ppn, long long& unbalanced_nnn) const;
};

/**
 * @brief Creates a signed graph from trust/distrust data.
 * 
 * @param n Number of vertices.
 * @param trust_edges Positive edges (trust).
 * @param distrust_edges Negative edges (distrust).
 * @return SignedGraph The signed graph.
 */
GRAPHLIB_API SignedGraph make_signed_graph(int n,
    const std::vector<std::pair<int, int>>& trust_edges,
    const std::vector<std::pair<int, int>>& distrust_edges);

}

#endif
