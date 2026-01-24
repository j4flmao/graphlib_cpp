#include "graphlib/bipartite_matching.h"
#include <queue>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>

namespace graphlib {

static const int INF = std::numeric_limits<int>::max();
static const long long LL_INF = std::numeric_limits<long long>::max();

BipartiteMatching::BipartiteMatching(int n_left, int n_right)
    : n_left_(n_left), n_right_(n_right), adj_(n_left), dist_limit_(INF) {
    if (n_left < 0 || n_right < 0) {
        throw std::invalid_argument("Number of vertices must be non-negative");
    }
}

void BipartiteMatching::add_edge(int u, int v) {
    if (u < 0 || u >= n_left_) {
        throw std::out_of_range("Left vertex index out of range");
    }
    if (v < 0 || v >= n_right_) {
        throw std::out_of_range("Right vertex index out of range");
    }
    adj_[u].push_back(v);
}

int BipartiteMatching::max_matching() {
    match_left_.assign(n_left_, -1);
    match_right_.assign(n_right_, -1);
    dist_.resize(n_left_);
    
    int matching_size = 0;
    
    while (bfs()) {
        for (int u = 0; u < n_left_; ++u) {
            if (match_left_[u] == -1) {
                if (dfs(u)) {
                    matching_size++;
                }
            }
        }
    }
    
    return matching_size;
}

std::vector<std::pair<int, int>> BipartiteMatching::get_matching() const {
    std::vector<std::pair<int, int>> result;
    for (int u = 0; u < n_left_; ++u) {
        if (match_left_[u] != -1) {
            result.emplace_back(u, match_left_[u]);
        }
    }
    return result;
}

bool BipartiteMatching::bfs() {
    std::queue<int> q;
    
    for (int u = 0; u < n_left_; ++u) {
        if (match_left_[u] == -1) {
            dist_[u] = 0;
            q.push(u);
        } else {
            dist_[u] = INF;
        }
    }
    
    dist_limit_ = INF;
    
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        
        if (dist_[u] < dist_limit_) {
            for (int v : adj_[u]) {
                if (match_right_[v] == -1) {
                    if (dist_limit_ == INF) {
                        dist_limit_ = dist_[u] + 1;
                    }
                } else {
                    int next_u = match_right_[v];
                    if (dist_[next_u] == INF) {
                        dist_[next_u] = dist_[u] + 1;
                        q.push(next_u);
                    }
                }
            }
        }
    }
    
    return dist_limit_ != INF;
}

bool BipartiteMatching::dfs(int u) {
    for (int v : adj_[u]) {
        int next_u = match_right_[v];
        
        if (next_u == -1) {
             if (dist_limit_ == dist_[u] + 1) {
                 match_right_[v] = u;
                 match_left_[u] = v;
                 return true;
             }
        } else {
            if (dist_[next_u] == dist_[u] + 1) {
                if (dfs(next_u)) {
                    match_right_[v] = u;
                    match_left_[u] = v;
                    return true;
                }
            }
        }
    }
    return false;
}

// ==========================================
// WeightedBipartiteMatching Implementation
// ==========================================

WeightedBipartiteMatching::WeightedBipartiteMatching(int n_left, int n_right)
    : n_left_(n_left), n_right_(n_right) {
    if (n_left < 0 || n_right < 0) {
        throw std::invalid_argument("Number of vertices must be non-negative");
    }
    n_ = std::max(n_left, n_right);
    weights_.assign(n_ + 1, std::vector<long long>(n_ + 1, 0));
    match_right_.assign(n_ + 1, 0);
    lx_.assign(n_ + 1, 0);
    ly_.assign(n_ + 1, 0);
    slack_.assign(n_ + 1, 0);
    way_.assign(n_ + 1, 0);
}

void WeightedBipartiteMatching::add_edge(int u, int v, long long w) {
    if (u < 0 || u >= n_left_) {
        throw std::out_of_range("Left vertex index out of range");
    }
    if (v < 0 || v >= n_right_) {
        throw std::out_of_range("Right vertex index out of range");
    }
    // Kuhn-Munkres usually uses 1-based indexing internally or 0-based.
    // We'll use 1-based indexing for the algorithm logic to match common implementations (e.g. e-maxx),
    // mapping input 0..n-1 to 1..n.
    
    // We will use 1-based indexing for internal vectors: weights_[u+1][v+1]
    weights_[u + 1][v + 1] = std::max(weights_[u + 1][v + 1], w);
}

long long WeightedBipartiteMatching::max_weight_matching() {
    // 1-based indexing for the algorithm
    // match_right_[v] = u means column v is matched with row u
    
    std::vector<long long> minv(n_ + 1);
    std::vector<bool> used(n_ + 1);
    
    // Initialize match_right_ to 0 (unmatched)
    std::fill(match_right_.begin(), match_right_.end(), 0);
    std::fill(lx_.begin(), lx_.end(), 0);
    std::fill(ly_.begin(), ly_.end(), 0);
    std::fill(way_.begin(), way_.end(), 0);

    // Initialize lx_ to max weight of each row to ensure non-negative slack
    for (int i = 1; i <= n_; ++i) {
        long long max_w = 0;
        for (int j = 1; j <= n_; ++j) {
            max_w = std::max(max_w, weights_[i][j]);
        }
        lx_[i] = max_w;
    }

    for (int i = 1; i <= n_; ++i) {
        match_right_[0] = i; // Dummy column 0 stores current row being matched
        int j0 = 0;
        std::fill(minv.begin(), minv.end(), LL_INF);
        std::fill(used.begin(), used.end(), false);
        
        do {
            used[j0] = true;
            int i0 = match_right_[j0];
            long long delta = LL_INF;
            int j1 = 0;
            
            for (int j = 1; j <= n_; ++j) {
                if (!used[j]) {
                    // Max Weight Logic:
                    // Slack = lx[i0] + ly[j] - weights_[i0][j]
                    // Since we init lx to max row weight, slack >= 0.
                    
                    long long cur = lx_[i0] + ly_[j] - weights_[i0][j];
                    if (cur < minv[j]) {
                        minv[j] = cur;
                        way_[j] = j0;
                    }
                    if (minv[j] < delta) {
                        delta = minv[j];
                        j1 = j;
                    }
                }
            }
            
            for (int j = 0; j <= n_; ++j) {
                if (used[j]) {
                    lx_[match_right_[j]] += delta;
                    ly_[j] -= delta;
                } else {
                    minv[j] -= delta;
                }
            }
            j0 = j1;
        } while (match_right_[j0] != 0);
        
        do {
            int j1 = way_[j0];
            match_right_[j0] = match_right_[j1];
            j0 = j1;
        } while (j0 != 0);
    }
    
    // Calculate total weight
    long long result = 0;
    for (int j = 1; j <= n_; ++j) {
        if (match_right_[j] > 0) {
             result += weights_[match_right_[j]][j];
        }
    }
    return result;
}

std::vector<std::pair<int, int>> WeightedBipartiteMatching::get_matching() const {
    std::vector<std::pair<int, int>> result;
    // match_right_[v] = u, where v is column (right), u is row (left).
    // Indices are 1-based.
    // Valid original indices: u in [1, n_left_], v in [1, n_right_]
    
    for (int v = 1; v <= n_; ++v) {
        int u = match_right_[v];
        if (u > 0) { // If matched
            if (u <= n_left_ && v <= n_right_) {
                // Check if edge has positive weight? 
                if (weights_[u][v] > 0) {
                     result.emplace_back(u - 1, v - 1);
                }
            }
        }
    }
    return result;
}

} // namespace graphlib
