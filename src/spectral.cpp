#include "graphlib/spectral.h"
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <random>
#include <chrono>

namespace graphlib {

namespace {
    // Helper: Dot product
    double dot_product(const std::vector<double>& a, const std::vector<double>& b) {
        double sum = 0.0;
        for (size_t i = 0; i < a.size(); ++i) {
            sum += a[i] * b[i];
        }
        return sum;
    }

    // Helper: Normalize
    void normalize(std::vector<double>& v) {
        double norm = std::sqrt(dot_product(v, v));
        if (norm > 1e-12) {
            for (double& val : v) val /= norm;
        }
    }
}

GRAPHLIB_API std::vector<double> fiedler_vector(const Graph& g, int iterations, double tolerance) {
    int n = g.vertex_count();
    if (n < 2) return std::vector<double>(n, 0.0);

    // 1. Construct Adjacency and Degrees
    // We don't store the full matrix L = D - A explicitly to save memory (sparse).
    // We implement Matrix-Vector multiplication implicitly.
    
    std::vector<int> degrees(n, 0);
    std::vector<std::vector<int>> adj(n);
    
    for (int u = 0; u < n; ++u) {
        Edge* e = g.get_edges(u);
        while(e) {
            if (e->to != u) { // Ignore self-loops for Laplacian usually
                adj[u].push_back(e->to);
                degrees[u]++;
            }
            e = e->next;
        }
    }
    
    // 2. Estimate Max Eigenvalue of L
    // Gershgorin: lambda_max <= max(degree(u) + sum(|A_uv|)) = 2 * max_degree
    int max_deg = 0;
    for(int d : degrees) max_deg = std::max(max_deg, d);
    
    // 3. Setup Power Iteration for M = alpha * I - L
    // Eigenvalues of L are 0 = lambda_1 <= lambda_2 <= ... <= lambda_n
    // Eigenvalues of M are alpha - lambda_i.
    // Largest eigenvalue of M is alpha - 0 = alpha (corresponding to eigenvector 1).
    // Second largest is alpha - lambda_2.
    // We want eigenvector of alpha - lambda_2.
    // We must orthogonalize against 1 to remove the alpha component.
    
    double alpha = 2.0 * max_deg + 1.0; // Shift to make M positive definite
    
    std::vector<double> v(n);
    std::mt19937 rng(static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::uniform_real_distribution<double> dist(-1.0, 1.0);
    for(int i=0; i<n; ++i) v[i] = dist(rng);
    
    // Orthogonalize against 1 vector: v = v - proj(v, 1)
    // proj(v, 1) = (v . 1) / (1 . 1) * 1
    // 1 . 1 = n
    // v . 1 = sum(v)
    {
        double sum = std::accumulate(v.begin(), v.end(), 0.0);
        double avg = sum / n;
        for(double& val : v) val -= avg;
    }
    normalize(v);
    
    std::vector<double> next_v(n);
    
    for (int iter = 0; iter < iterations; ++iter) {
        // Compute M * v = (alpha * I - (D - A)) * v = alpha * v - D * v + A * v
        // (A * v)[i] = sum_{j in neighbors(i)} v[j]
        
        for (int i = 0; i < n; ++i) {
            double Av_i = 0.0;
            for (int neighbor : adj[i]) {
                Av_i += v[neighbor];
            }
            
            // M v = alpha * v - D * v + A * v
            //     = (alpha - degree[i]) * v[i] + Av_i
            next_v[i] = (alpha - degrees[i]) * v[i] + Av_i;
        }
        
        // Orthogonalize against 1
        double sum = std::accumulate(next_v.begin(), next_v.end(), 0.0);
        double avg = sum / n;
        for(double& val : next_v) val -= avg;
        
        // Normalize
        normalize(next_v);
        
        // Check convergence
        double diff = 0.0;
        for(int i=0; i<n; ++i) diff += (next_v[i] - v[i]) * (next_v[i] - v[i]);
        if (std::sqrt(diff) < tolerance) {
            v = next_v;
            break;
        }
        
        v = next_v;
    }
    
    return v;
}

GRAPHLIB_API std::pair<std::vector<int>, std::vector<int>> spectral_bisection(const Graph& g) {
    int n = g.vertex_count();
    if (n == 0) return {{}, {}};
    
    // Check connectivity first? If disconnected, just return components.
    // But Fiedler vector handles it (lambda_2 = 0 implies disconnected).
    // The vector will naturally separate components.
    
    std::vector<double> fv = fiedler_vector(g);
    
    // Sort vertices by Fiedler value
    std::vector<int> p(n);
    std::iota(p.begin(), p.end(), 0);
    std::sort(p.begin(), p.end(), [&](int a, int b) {
        return fv[a] < fv[b];
    });
    
    // Cut at median (balance partition) or at 0 (sign cut)?
    // "Spectral Bisection" usually means median cut for balancing sizes.
    // Or sign cut for minimizing cut ratio.
    // Let's use median to ensure non-empty partitions.
    
    int mid = n / 2;
    std::vector<int> part1, part2;
    for(int i=0; i<mid; ++i) part1.push_back(p[i]);
    for(int i=mid; i<n; ++i) part2.push_back(p[i]);
    
    return {part1, part2};
}

}
