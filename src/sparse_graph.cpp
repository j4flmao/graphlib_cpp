#include "graphlib/sparse_graph.h"
#include <algorithm>
#include <numeric>
#include <unordered_map>

namespace graphlib {

// ============== CSRGraph Implementation ==============

CSRGraph::CSRGraph() : n_(0), m_(0), directed_(true) {}

CSRGraph::CSRGraph(int n, const std::vector<std::tuple<int, int, long long>>& edges, bool directed)
    : n_(n), directed_(directed) {
    
    if (directed) {
        m_ = static_cast<int>(edges.size());
    } else {
        m_ = static_cast<int>(edges.size()) * 2;
    }
    
    row_ptr_.resize(n + 1, 0);
    col_idx_.reserve(m_);
    values_.reserve(m_);
    
    // Count edges per vertex
    std::vector<int> counts(n, 0);
    for (const auto& [u, v, w] : edges) {
        if (u >= 0 && u < n) counts[u]++;
        if (!directed && v >= 0 && v < n && u != v) counts[v]++;
    }
    
    // Build row_ptr
    row_ptr_[0] = 0;
    for (int i = 0; i < n; ++i) {
        row_ptr_[i + 1] = row_ptr_[i] + counts[i];
    }
    
    // Reset counts for insertion
    std::fill(counts.begin(), counts.end(), 0);
    
    col_idx_.resize(row_ptr_[n]);
    values_.resize(row_ptr_[n]);
    
    // Insert edges
    for (const auto& [u, v, w] : edges) {
        if (u >= 0 && u < n && v >= 0 && v < n) {
            int idx = row_ptr_[u] + counts[u]++;
            col_idx_[idx] = v;
            values_[idx] = w;
            
            if (!directed && u != v) {
                idx = row_ptr_[v] + counts[v]++;
                col_idx_[idx] = u;
                values_[idx] = w;
            }
        }
    }
    
    // Sort adjacency lists
    for (int i = 0; i < n; ++i) {
        int start = row_ptr_[i];
        int end = row_ptr_[i + 1];
        
        std::vector<std::pair<int, long long>> neighbors;
        for (int j = start; j < end; ++j) {
            neighbors.emplace_back(col_idx_[j], values_[j]);
        }
        std::sort(neighbors.begin(), neighbors.end());
        
        for (int j = start; j < end; ++j) {
            col_idx_[j] = neighbors[j - start].first;
            values_[j] = neighbors[j - start].second;
        }
    }
}

CSRGraph::CSRGraph(const Graph& g) {
    n_ = g.vertex_count();
    directed_ = g.is_directed();
    
    std::vector<std::tuple<int, int, long long>> edges;
    for (int u = 0; u < n_; ++u) {
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            if (directed_ || u <= e->to) {
                edges.emplace_back(u, e->to, e->weight);
            }
        }
    }
    
    *this = CSRGraph(n_, edges, directed_);
}

Graph CSRGraph::to_graph() const {
    Graph g(n_, directed_);
    
    for (int u = 0; u < n_; ++u) {
        for (int j = row_ptr_[u]; j < row_ptr_[u + 1]; ++j) {
            if (directed_ || u <= col_idx_[j]) {
                g.add_edge(u, col_idx_[j], values_[j]);
            }
        }
    }
    
    return g;
}

CSRGraph CSRGraph::transpose() const {
    std::vector<std::tuple<int, int, long long>> edges;
    
    for (int u = 0; u < n_; ++u) {
        for (int j = row_ptr_[u]; j < row_ptr_[u + 1]; ++j) {
            edges.emplace_back(col_idx_[j], u, values_[j]);
        }
    }
    
    return CSRGraph(n_, edges, directed_);
}

bool CSRGraph::has_edge(int u, int v) const {
    if (u < 0 || u >= n_) return false;
    
    int start = row_ptr_[u];
    int end = row_ptr_[u + 1];
    
    // Binary search since adjacency is sorted
    return std::binary_search(col_idx_.begin() + start, col_idx_.begin() + end, v);
}

long long CSRGraph::get_weight(int u, int v) const {
    if (u < 0 || u >= n_) return 0;
    
    int start = row_ptr_[u];
    int end = row_ptr_[u + 1];
    
    auto it = std::lower_bound(col_idx_.begin() + start, col_idx_.begin() + end, v);
    if (it != col_idx_.begin() + end && *it == v) {
        return values_[it - col_idx_.begin()];
    }
    
    return 0;
}

// ============== CSCGraph Implementation ==============

CSCGraph::CSCGraph() : n_(0), m_(0), directed_(true) {}

CSCGraph::CSCGraph(int n, const std::vector<std::tuple<int, int, long long>>& edges, bool directed)
    : n_(n), directed_(directed) {
    
    if (directed) {
        m_ = static_cast<int>(edges.size());
    } else {
        m_ = static_cast<int>(edges.size()) * 2;
    }
    
    col_ptr_.resize(n + 1, 0);
    
    // Count edges per column (destination vertex)
    std::vector<int> counts(n, 0);
    for (const auto& [u, v, w] : edges) {
        if (v >= 0 && v < n) counts[v]++;
        if (!directed && u >= 0 && u < n && u != v) counts[u]++;
    }
    
    // Build col_ptr
    col_ptr_[0] = 0;
    for (int i = 0; i < n; ++i) {
        col_ptr_[i + 1] = col_ptr_[i] + counts[i];
    }
    
    std::fill(counts.begin(), counts.end(), 0);
    
    row_idx_.resize(col_ptr_[n]);
    values_.resize(col_ptr_[n]);
    
    // Insert edges
    for (const auto& [u, v, w] : edges) {
        if (u >= 0 && u < n && v >= 0 && v < n) {
            int idx = col_ptr_[v] + counts[v]++;
            row_idx_[idx] = u;
            values_[idx] = w;
            
            if (!directed && u != v) {
                idx = col_ptr_[u] + counts[u]++;
                row_idx_[idx] = v;
                values_[idx] = w;
            }
        }
    }
    
    // Sort each column
    for (int i = 0; i < n; ++i) {
        int start = col_ptr_[i];
        int end = col_ptr_[i + 1];
        
        std::vector<std::pair<int, long long>> rows;
        for (int j = start; j < end; ++j) {
            rows.emplace_back(row_idx_[j], values_[j]);
        }
        std::sort(rows.begin(), rows.end());
        
        for (int j = start; j < end; ++j) {
            row_idx_[j] = rows[j - start].first;
            values_[j] = rows[j - start].second;
        }
    }
}

CSCGraph::CSCGraph(const Graph& g) {
    n_ = g.vertex_count();
    directed_ = g.is_directed();
    
    std::vector<std::tuple<int, int, long long>> edges;
    for (int u = 0; u < n_; ++u) {
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            if (directed_ || u <= e->to) {
                edges.emplace_back(u, e->to, e->weight);
            }
        }
    }
    
    *this = CSCGraph(n_, edges, directed_);
}

CSCGraph::CSCGraph(const CSRGraph& csr) {
    n_ = csr.vertex_count();
    m_ = csr.edge_count();
    directed_ = csr.is_directed();
    
    std::vector<std::tuple<int, int, long long>> edges;
    const auto& row_ptr = csr.row_ptr();
    const auto& col_idx = csr.col_idx();
    const auto& values = csr.values();
    
    for (int u = 0; u < n_; ++u) {
        for (int j = row_ptr[u]; j < row_ptr[u + 1]; ++j) {
            edges.emplace_back(u, col_idx[j], values[j]);
        }
    }
    
    *this = CSCGraph(n_, edges, true);  // Already expanded if undirected
}

Graph CSCGraph::to_graph() const {
    Graph g(n_, directed_);
    
    for (int v = 0; v < n_; ++v) {
        for (int j = col_ptr_[v]; j < col_ptr_[v + 1]; ++j) {
            int u = row_idx_[j];
            if (directed_ || u <= v) {
                g.add_edge(u, v, values_[j]);
            }
        }
    }
    
    return g;
}

CSRGraph CSCGraph::to_csr() const {
    std::vector<std::tuple<int, int, long long>> edges;
    
    for (int v = 0; v < n_; ++v) {
        for (int j = col_ptr_[v]; j < col_ptr_[v + 1]; ++j) {
            edges.emplace_back(row_idx_[j], v, values_[j]);
        }
    }
    
    return CSRGraph(n_, edges, true);
}

// ============== COOGraph Implementation ==============

COOGraph::COOGraph() : n_(0), m_(0), directed_(true) {}

COOGraph::COOGraph(int n, bool directed) : n_(n), m_(0), directed_(directed) {}

COOGraph::COOGraph(const Graph& g) {
    n_ = g.vertex_count();
    directed_ = g.is_directed();
    m_ = 0;
    
    for (int u = 0; u < n_; ++u) {
        for (Edge* e = g.get_edges(u); e; e = e->next) {
            if (directed_ || u <= e->to) {
                row_.push_back(u);
                col_.push_back(e->to);
                data_.push_back(e->weight);
                m_++;
            }
        }
    }
}

COOGraph::COOGraph(const CSRGraph& csr) {
    n_ = csr.vertex_count();
    directed_ = csr.is_directed();
    m_ = 0;
    
    const auto& row_ptr = csr.row_ptr();
    const auto& col_idx = csr.col_idx();
    const auto& values = csr.values();
    
    for (int u = 0; u < n_; ++u) {
        for (int j = row_ptr[u]; j < row_ptr[u + 1]; ++j) {
            row_.push_back(u);
            col_.push_back(col_idx[j]);
            data_.push_back(values[j]);
            m_++;
        }
    }
}

void COOGraph::add_edge(int from, int to, long long weight) {
    row_.push_back(from);
    col_.push_back(to);
    data_.push_back(weight);
    m_++;
    
    n_ = std::max(n_, std::max(from, to) + 1);
}

void COOGraph::reserve(int expected_edges) {
    row_.reserve(expected_edges);
    col_.reserve(expected_edges);
    data_.reserve(expected_edges);
}

void COOGraph::sort() {
    std::vector<size_t> indices(m_);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::sort(indices.begin(), indices.end(), [this](size_t a, size_t b) {
        if (row_[a] != row_[b]) return row_[a] < row_[b];
        return col_[a] < col_[b];
    });
    
    std::vector<int> new_row(m_), new_col(m_);
    std::vector<long long> new_data(m_);
    
    for (size_t i = 0; i < m_; ++i) {
        new_row[i] = row_[indices[i]];
        new_col[i] = col_[indices[i]];
        new_data[i] = data_[indices[i]];
    }
    
    row_ = std::move(new_row);
    col_ = std::move(new_col);
    data_ = std::move(new_data);
}

void COOGraph::remove_duplicates() {
    sort();
    
    std::vector<int> new_row, new_col;
    std::vector<long long> new_data;
    
    for (size_t i = 0; i < m_; ++i) {
        if (new_row.empty() || new_row.back() != row_[i] || new_col.back() != col_[i]) {
            new_row.push_back(row_[i]);
            new_col.push_back(col_[i]);
            new_data.push_back(data_[i]);
        } else {
            // Keep max weight
            new_data.back() = std::max(new_data.back(), data_[i]);
        }
    }
    
    row_ = std::move(new_row);
    col_ = std::move(new_col);
    data_ = std::move(new_data);
    m_ = static_cast<int>(row_.size());
}

CSRGraph COOGraph::to_csr() const {
    std::vector<std::tuple<int, int, long long>> edges;
    edges.reserve(m_);
    
    for (size_t i = 0; i < m_; ++i) {
        edges.emplace_back(row_[i], col_[i], data_[i]);
    }
    
    return CSRGraph(n_, edges, directed_);
}

CSCGraph COOGraph::to_csc() const {
    std::vector<std::tuple<int, int, long long>> edges;
    edges.reserve(m_);
    
    for (size_t i = 0; i < m_; ++i) {
        edges.emplace_back(row_[i], col_[i], data_[i]);
    }
    
    return CSCGraph(n_, edges, directed_);
}

Graph COOGraph::to_graph() const {
    Graph g(n_, directed_);
    
    for (size_t i = 0; i < m_; ++i) {
        g.add_edge(row_[i], col_[i], data_[i]);
    }
    
    return g;
}

// ============== Conversion Functions ==============

CSRGraph graph_to_csr(const Graph& g) {
    return CSRGraph(g);
}

CSCGraph graph_to_csc(const Graph& g) {
    return CSCGraph(g);
}

COOGraph graph_to_coo(const Graph& g) {
    return COOGraph(g);
}

} // namespace graphlib
