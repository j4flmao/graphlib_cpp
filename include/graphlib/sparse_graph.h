#ifndef GRAPHLIB_SPARSE_GRAPH_H
#define GRAPHLIB_SPARSE_GRAPH_H

#include "export.h"
#include "graph_core.h"
#include <vector>
#include <tuple>

namespace graphlib {

/**
 * @brief Compressed Sparse Row (CSR) representation of a graph.
 * 
 * Optimal for read-heavy operations, parallel algorithms, and ML pipelines.
 * Memory: O(n + m) where n = vertices, m = edges
 * 
 * Structure:
 * - row_ptr[i] = start index in col_idx for vertex i's neighbors
 * - row_ptr[i+1] - row_ptr[i] = out-degree of vertex i
 * - col_idx[row_ptr[i]..row_ptr[i+1]) = neighbors of vertex i
 * - values[j] = weight of edge j
 */
class GRAPHLIB_API CSRGraph {
private:
    int n_;                         // Number of vertices
    int m_;                         // Number of edges
    bool directed_;
    std::vector<int> row_ptr_;      // Size: n + 1
    std::vector<int> col_idx_;      // Size: m (or 2m for undirected)
    std::vector<long long> values_; // Size: m (or 2m for undirected)

public:
    CSRGraph();
    
    /**
     * @brief Constructs CSR graph from edge list.
     * 
     * @param n Number of vertices.
     * @param edges List of (from, to, weight) tuples.
     * @param directed Whether the graph is directed.
     */
    CSRGraph(int n, const std::vector<std::tuple<int, int, long long>>& edges, bool directed = true);
    
    /**
     * @brief Constructs CSR graph from a Graph object.
     */
    explicit CSRGraph(const Graph& g);
    
    // Accessors
    int vertex_count() const { return n_; }
    int edge_count() const { return m_; }
    bool is_directed() const { return directed_; }
    
    /**
     * @brief Gets the degree (out-degree for directed) of a vertex.
     */
    int degree(int v) const { return row_ptr_[v + 1] - row_ptr_[v]; }
    
    /**
     * @brief Gets the neighbors of a vertex.
     * 
     * @param v Vertex index.
     * @return Pair of pointers to begin and end of neighbor array.
     */
    std::pair<const int*, const int*> neighbors(int v) const {
        return {col_idx_.data() + row_ptr_[v], col_idx_.data() + row_ptr_[v + 1]};
    }
    
    /**
     * @brief Gets neighbor and weight arrays for a vertex.
     */
    void neighbors_weighted(int v, const int*& neighbors, const long long*& weights, int& count) const {
        neighbors = col_idx_.data() + row_ptr_[v];
        weights = values_.data() + row_ptr_[v];
        count = row_ptr_[v + 1] - row_ptr_[v];
    }
    
    // Raw access for algorithms
    const std::vector<int>& row_ptr() const { return row_ptr_; }
    const std::vector<int>& col_idx() const { return col_idx_; }
    const std::vector<long long>& values() const { return values_; }
    
    /**
     * @brief Converts back to Graph object.
     */
    Graph to_graph() const;
    
    /**
     * @brief Gets the transpose (reverse edges) of the graph.
     */
    CSRGraph transpose() const;
    
    /**
     * @brief Checks if edge (u, v) exists.
     */
    bool has_edge(int u, int v) const;
    
    /**
     * @brief Gets weight of edge (u, v), or 0 if not exists.
     */
    long long get_weight(int u, int v) const;
};

/**
 * @brief Compressed Sparse Column (CSC) representation.
 * 
 * Optimal for computing in-degrees and reverse traversals.
 */
class GRAPHLIB_API CSCGraph {
private:
    int n_;
    int m_;
    bool directed_;
    std::vector<int> col_ptr_;      // Size: n + 1
    std::vector<int> row_idx_;      // Size: m
    std::vector<long long> values_; // Size: m

public:
    CSCGraph();
    CSCGraph(int n, const std::vector<std::tuple<int, int, long long>>& edges, bool directed = true);
    explicit CSCGraph(const Graph& g);
    explicit CSCGraph(const CSRGraph& csr);
    
    int vertex_count() const { return n_; }
    int edge_count() const { return m_; }
    bool is_directed() const { return directed_; }
    
    /**
     * @brief Gets in-degree of a vertex.
     */
    int in_degree(int v) const { return col_ptr_[v + 1] - col_ptr_[v]; }
    
    /**
     * @brief Gets predecessors of a vertex (vertices with edges to v).
     */
    std::pair<const int*, const int*> predecessors(int v) const {
        return {row_idx_.data() + col_ptr_[v], row_idx_.data() + col_ptr_[v + 1]};
    }
    
    const std::vector<int>& col_ptr() const { return col_ptr_; }
    const std::vector<int>& row_idx() const { return row_idx_; }
    const std::vector<long long>& values() const { return values_; }
    
    Graph to_graph() const;
    CSRGraph to_csr() const;
};

/**
 * @brief COO (Coordinate) format - simple edge list format.
 * 
 * Useful for building graphs and exporting to ML frameworks.
 */
class GRAPHLIB_API COOGraph {
private:
    int n_;
    int m_;
    bool directed_;
    std::vector<int> row_;          // Source vertices
    std::vector<int> col_;          // Target vertices
    std::vector<long long> data_;   // Edge weights

public:
    COOGraph();
    COOGraph(int n, bool directed = true);
    explicit COOGraph(const Graph& g);
    explicit COOGraph(const CSRGraph& csr);
    
    int vertex_count() const { return n_; }
    int edge_count() const { return m_; }
    bool is_directed() const { return directed_; }
    
    void add_edge(int from, int to, long long weight = 1);
    void reserve(int expected_edges);
    
    const std::vector<int>& row() const { return row_; }
    const std::vector<int>& col() const { return col_; }
    const std::vector<long long>& data() const { return data_; }
    
    /**
     * @brief Sorts edges by (row, col) for efficient conversion.
     */
    void sort();
    
    /**
     * @brief Removes duplicate edges (keeps the one with higher weight).
     */
    void remove_duplicates();
    
    CSRGraph to_csr() const;
    CSCGraph to_csc() const;
    Graph to_graph() const;
};

// Conversion functions
GRAPHLIB_API CSRGraph graph_to_csr(const Graph& g);
GRAPHLIB_API CSCGraph graph_to_csc(const Graph& g);
GRAPHLIB_API COOGraph graph_to_coo(const Graph& g);

}

#endif
