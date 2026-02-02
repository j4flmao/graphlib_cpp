#ifndef GRAPHLIB_MULTIGRAPH_H
#define GRAPHLIB_MULTIGRAPH_H

#include "export.h"
#include "graph_core.h"
#include <vector>
#include <map>
#include <tuple>
#include <string>

namespace graphlib {

/**
 * @brief Multigraph supporting multiple edges between the same pair of vertices.
 * 
 * Each edge has a unique ID for tracking and manipulation.
 * Supports self-loops.
 */
class GRAPHLIB_API Multigraph {
public:
    struct MultiEdge {
        int id;
        int from;
        int to;
        long long weight;
        
        MultiEdge() : id(-1), from(-1), to(-1), weight(0) {}
        MultiEdge(int id, int from, int to, long long weight);
    };

private:
    int n_;
    int next_edge_id_;
    bool directed_;
    std::vector<std::vector<MultiEdge>> adj_;
    std::map<int, MultiEdge> edges_by_id_;
    std::vector<int> self_loops_;  // Edge IDs of self-loops

public:
    explicit Multigraph(int n, bool directed = false);
    
    /**
     * @brief Adds an edge and returns its unique ID.
     * 
     * @param from Source vertex.
     * @param to Target vertex.
     * @param weight Edge weight.
     * @return int Unique edge ID.
     */
    int add_edge(int from, int to, long long weight = 1);
    
    /**
     * @brief Removes an edge by its ID.
     * 
     * @param edge_id The edge ID to remove.
     * @return true if edge was found and removed.
     */
    bool remove_edge(int edge_id);
    
    /**
     * @brief Removes all edges between u and v.
     * 
     * @return int Number of edges removed.
     */
    int remove_all_edges(int u, int v);
    
    // Accessors
    int vertex_count() const { return n_; }
    int edge_count() const { return static_cast<int>(edges_by_id_.size()); }
    bool is_directed() const { return directed_; }
    
    /**
     * @brief Gets all edges from vertex v.
     */
    const std::vector<MultiEdge>& edges_from(int v) const { return adj_[v]; }
    
    /**
     * @brief Gets edge by ID.
     * 
     * @return Pointer to edge, or nullptr if not found.
     */
    const MultiEdge* get_edge(int edge_id) const;
    
    /**
     * @brief Gets number of edges between u and v.
     */
    int edge_multiplicity(int u, int v) const;
    
    /**
     * @brief Gets all edges between u and v.
     */
    std::vector<MultiEdge> edges_between(int u, int v) const;
    
    /**
     * @brief Gets degree of vertex (counting multiplicities).
     */
    int degree(int v) const;
    
    /**
     * @brief Gets simple degree (number of distinct neighbors).
     */
    int simple_degree(int v) const;
    
    /**
     * @brief Checks if graph has any parallel edges.
     */
    bool has_parallel_edges() const;
    
    /**
     * @brief Checks if graph has self-loops.
     */
    bool has_self_loops() const { return !self_loops_.empty(); }
    
    /**
     * @brief Gets all self-loop edge IDs.
     */
    const std::vector<int>& get_self_loops() const { return self_loops_; }
    
    /**
     * @brief Removes all self-loops.
     * 
     * @return int Number of self-loops removed.
     */
    int remove_self_loops();
    
    /**
     * @brief Simplifies to at most one edge per vertex pair.
     * 
     * @param keep "min", "max", "sum", or "first"
     * @return int Number of edges removed.
     */
    int simplify(const std::string& keep = "max");
    
    /**
     * @brief Converts to a simple graph (one edge per pair, no self-loops).
     * 
     * Uses maximum weight when multiple edges exist.
     */
    Graph to_simple_graph() const;
    
    /**
     * @brief Checks if the multigraph is Eulerian.
     * 
     * Has Eulerian circuit if all vertices have even degree.
     */
    bool is_eulerian() const;
    
    /**
     * @brief Finds Eulerian path/circuit if exists.
     * 
     * @return std::vector<int> Sequence of edge IDs forming the path, or empty if none.
     */
    std::vector<int> eulerian_path() const;
    
    /**
     * @brief Gets total weight of all edges.
     */
    long long total_weight() const;
    
    /**
     * @brief Iterator support for all edges.
     */
    std::vector<MultiEdge> all_edges() const;
};

} // namespace graphlib

#endif
