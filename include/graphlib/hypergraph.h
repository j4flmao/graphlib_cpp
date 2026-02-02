#ifndef GRAPHLIB_HYPERGRAPH_H
#define GRAPHLIB_HYPERGRAPH_H

#include "export.h"
#include "graph_core.h"
#include <vector>
#include <iostream>
#include <set>

namespace graphlib {

struct HyperEdge {
    int id;
    double weight;
    std::vector<int> nodes;
};

/**
 * @brief Represents a Hypergraph where edges can connect more than two vertices.
 * 
 * Implemented using an incidence list structure.
 * Vertices are 0-indexed. Hyperedges are assigned IDs 0, 1, 2...
 */
class GRAPHLIB_API Hypergraph {
public:
    explicit Hypergraph(int n);

    /**
     * @brief Adds a hyperedge connecting a set of nodes.
     * @param nodes The vector of vertex IDs included in this hyperedge.
     * @param weight The weight of the hyperedge (default 1.0).
     * @return The ID of the newly created hyperedge.
     */
    int add_hyperedge(const std::vector<int>& nodes, double weight = 1.0);

    /**
     * @brief Returns the number of vertices.
     */
    int vertex_count() const;

    /**
     * @brief Returns the number of hyperedges.
     */
    int edge_count() const;

    /**
     * @brief Returns the degree of a vertex (number of incident hyperedges).
     */
    int vertex_degree(int v) const;

    /**
     * @brief Returns the size (rank) of a hyperedge (number of vertices in it).
     */
    int edge_size(int e_id) const;

    /**
     * @brief Returns the maximum rakn (max edge size) in the hypergraph.
     */
    int max_rank() const;

    /**
     * @brief Returns the incident hyperedges for a given vertex.
     */
    const std::vector<int>& get_incident_edges(int v) const;

    /**
     * @brief Returns the vertices belonging to a specific hyperedge.
     */
    const std::vector<int>& get_edge_nodes(int e_id) const;
    
    /**
     * @brief Returns the weight of a specific hyperedge.
     */
    double get_edge_weight(int e_id) const;

    /**
     * @brief Converts the hypergraph to its dual.
     * In the dual, vertices become hyperedges and hyperedges become vertices.
     */
    Hypergraph get_dual() const;

    /**
     * @brief Converts the hypergraph to its clique expansion (a standard graph).
     * Each hyperedge is replaced by a clique (complete subgraph) of its vertices.
     */
    Graph to_clique_expansion() const;

    /**
     * @brief Converts the hypergraph to a bipartite graph (incidence graph).
     * One set of nodes represents original vertices, the other represents hyperedges.
     * Edges exist between v and e if v is in e.
     */
    Graph to_bipartite_expansion() const; 

    /**
     * @brief Finds a minimal transversal (hitting set) of the hypergraph.
     * A transversal is a subset of vertices that intersects every hyperedge.
     * Guaranteed to be a valid transversal, but not necessarily minimum size (NP-Hard).
     * Uses a greedy approximation.
     */
    std::vector<int> greedy_transversal() const;

    /**
     * @brief Finds a maximum matching (set of disjoint hyperedges).
     * Two edges are disjoint if they share no vertices.
     * Uses a greedy approximation.
     * @return Vector of edge IDs.
     */
    std::vector<int> greedy_matching() const;

    /**
     * @brief Checks if the hypergraph is 2-colorable (Property B).
     * Attempts to color vertices with 2 colors such that no hyperedge is monochromatic.
     * 
     * @param colors Output vector to store colors (0 or 1) for each vertex.
     * @return true if 2-colorable, false otherwise.
     * Note: This uses a backtracking approach and may be slow for large graphs.
     */
    bool is_2_colorable(std::vector<int>& colors) const;

private:
    int n_;
    std::vector<HyperEdge> edges_;
    std::vector<std::vector<int>> incident_edges_; // vertex -> [edge_ids]
};

} // namespace graphlib

#endif // GRAPHLIB_HYPERGRAPH_H
