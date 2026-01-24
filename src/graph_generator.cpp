#include "graphlib/graph_generator.h"
#include <random>
#include <algorithm>
#include <set>
#include <vector>
#include <cmath>

namespace graphlib {
namespace generator {

namespace {
    std::mt19937 get_rng(int seed) {
        if (seed == -1) {
            std::random_device rd;
            return std::mt19937(rd());
        }
        return std::mt19937(seed);
    }
}

Graph random_graph(int n, double p, bool directed, int seed) {
    Graph g(n, directed);
    auto rng = get_rng(seed);
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    if (directed) {
        for (int u = 0; u < n; ++u) {
            for (int v = 0; v < n; ++v) {
                if (u != v && dist(rng) < p) {
                    g.add_edge(u, v);
                }
            }
        }
    } else {
        for (int u = 0; u < n; ++u) {
            for (int v = u + 1; v < n; ++v) {
                if (dist(rng) < p) {
                    g.add_edge(u, v);
                }
            }
        }
    }
    return g;
}

Graph random_tree(int n, int seed) {
    Graph g(n, false); // Trees are usually undirected
    if (n <= 1) return g;
    
    // Random attachment: for i from 1 to n-1, attach i to random node in 0..i-1
    auto rng = get_rng(seed);
    
    for (int i = 1; i < n; ++i) {
        std::uniform_int_distribution<int> dist(0, i - 1);
        int parent = dist(rng);
        g.add_edge(parent, i);
    }
    return g;
}

Graph complete_graph(int n, bool directed) {
    Graph g(n, directed);
    if (directed) {
        for (int u = 0; u < n; ++u) {
            for (int v = 0; v < n; ++v) {
                if (u != v) g.add_edge(u, v);
            }
        }
    } else {
        for (int u = 0; u < n; ++u) {
            for (int v = u + 1; v < n; ++v) {
                g.add_edge(u, v);
            }
        }
    }
    return g;
}

Graph cycle_graph(int n, bool directed) {
    Graph g(n, directed);
    if (n < 2) return g;
    for (int i = 0; i < n; ++i) {
        g.add_edge(i, (i + 1) % n);
    }
    return g;
}

Graph path_graph(int n, bool directed) {
    Graph g(n, directed);
    if (n < 2) return g;
    for (int i = 0; i < n - 1; ++i) {
        g.add_edge(i, i + 1);
    }
    return g;
}

Graph grid_2d_graph(int rows, int cols, bool directed) {
    int n = rows * cols;
    Graph g(n, directed);
    if (n == 0) return g;
    
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int u = r * cols + c;
            
            // Right neighbor
            if (c + 1 < cols) {
                int v = r * cols + (c + 1);
                g.add_edge(u, v);
            }
            
            // Down neighbor
            if (r + 1 < rows) {
                int v = (r + 1) * cols + c;
                g.add_edge(u, v);
            }
        }
    }
    return g;
}

Graph watts_strogatz(int n, int k, double p, int seed) {
    // k must be even for standard ring lattice construction
    if (k % 2 != 0) k--; 
    if (k < 2) k = 2; // Minimum k
    
    Graph g(n, false);
    if (n <= k) {
        return complete_graph(n, false);
    }
    
    auto rng = get_rng(seed);
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    std::uniform_int_distribution<int> node_dist(0, n - 1);
    
    // 1. Construct ring lattice
    // Connect each node to k/2 neighbors on each side
    int half_k = k / 2;
    for (int u = 0; u < n; ++u) {
        for (int i = 1; i <= half_k; ++i) {
            int v = (u + i) % n;
            g.add_edge(u, v);
        }
    }
    
    // 2. Rewire edges
    // For each edge (u, v) with i < k/2, replace with probability p
    // We need to be careful not to duplicate edges or self-loops.
    // Iterating edges in adjacency list is tricky while modifying.
    // So we'll build a list of edges first? No, we can just iterate.
    // Standard WS algorithm iterates over each node u and each edge (u, v) to its k/2 right neighbors.
    
    // Since Graph stores adjacency list and undirected adds both ways, we need to be careful.
    // Let's rebuild the graph.
    
    std::vector<std::set<int>> adj(n);
    
    // Initial ring
    for (int u = 0; u < n; ++u) {
        for (int i = 1; i <= half_k; ++i) {
            int v = (u + i) % n;
            adj[u].insert(v);
            adj[v].insert(u);
        }
    }
    
    for (int u = 0; u < n; ++u) {
        for (int i = 1; i <= half_k; ++i) {
            int v = (u + i) % n;
            
            if (dist(rng) < p) {
                // Rewire (u, v) to (u, w)
                // Remove (u, v)
                adj[u].erase(v);
                adj[v].erase(u);
                
                // Find new w
                int w = node_dist(rng);
                while (w == u || adj[u].count(w)) {
                    w = node_dist(rng);
                    // Avoid infinite loop if graph is dense
                    if (adj[u].size() >= (size_t)n - 1) break; 
                }
                
                adj[u].insert(w);
                adj[w].insert(u);
            }
        }
    }
    
    // Rebuild Graph object
    Graph res(n, false);
    for (int u = 0; u < n; ++u) {
        for (int v : adj[u]) {
            if (u < v) { // Add each edge once
                res.add_edge(u, v);
            }
        }
    }
    return res;
}

Graph barabasi_albert(int n, int m, int seed) {
    if (m < 1) m = 1;
    if (n <= m) return complete_graph(n, false);
    
    Graph g(n, false);
    auto rng = get_rng(seed);
    
    // Start with a complete graph of m nodes (or m+1)
    // Standard BA starts with m0 nodes. Let's start with m nodes connected.
    for (int i = 0; i < m; ++i) {
        for (int j = i + 1; j < m; ++j) {
            g.add_edge(i, j);
        }
    }
    
    // List of nodes to sample from, proportional to degree
    // "Roulette wheel" selection can be simulated by picking a random element from a list
    // where each node appears degree times.
    std::vector<int> repeated_nodes;
    repeated_nodes.reserve(2 * n * m);
    
    // Initialize with initial clique
    for (int i = 0; i < m; ++i) {
        for (int k = 0; k < m - 1; ++k) {
            repeated_nodes.push_back(i);
        }
    }
    
    // Add remaining nodes
    for (int i = m; i < n; ++i) {
        std::vector<int> targets;
        targets.reserve(m);
        
        std::set<int> distinct_targets;
        
        // Preferential attachment
        while (distinct_targets.size() < (size_t)m) {
            if (repeated_nodes.empty()) break; // Should not happen
            
            std::uniform_int_distribution<size_t> dist(0, repeated_nodes.size() - 1);
            int target = repeated_nodes[dist(rng)];
            
            if (distinct_targets.find(target) == distinct_targets.end()) {
                distinct_targets.insert(target);
                targets.push_back(target);
            }
        }
        
        for (int target : targets) {
            g.add_edge(i, target);
            repeated_nodes.push_back(i);
            repeated_nodes.push_back(target);
        }
    }
    
    return g;
}

Graph random_tree_prufer(int n, int seed) {
    Graph g(n, false);
    if (n <= 1) return g;
    if (n == 2) {
        g.add_edge(0, 1);
        return g;
    }

    auto rng = get_rng(seed);
    std::uniform_int_distribution<int> dist(0, n - 1);

    // Generate Prufer sequence of length n-2
    std::vector<int> prufer(n - 2);
    std::vector<int> degree(n, 1);
    for (int i = 0; i < n - 2; ++i) {
        prufer[i] = dist(rng);
        degree[prufer[i]]++;
    }

    // Leaves are nodes with degree 1
    // We need smallest leaf to match standard algorithm, but for random tree 
    // any leaf order is fine? No, Prufer bijection requires specific order (usually smallest).
    // Using set for O(N log N)
    std::set<int> leaves;
    for (int i = 0; i < n; ++i) {
        if (degree[i] == 1) {
            leaves.insert(i);
        }
    }

    for (int i = 0; i < n - 2; ++i) {
        int u = *leaves.begin();
        leaves.erase(leaves.begin());
        int v = prufer[i];

        g.add_edge(u, v);

        degree[v]--;
        if (degree[v] == 1) {
            leaves.insert(v);
        }
    }

    // Connect last two nodes
    int u = *leaves.begin();
    leaves.erase(leaves.begin());
    int v = *leaves.begin(); // There must be exactly two left
    g.add_edge(u, v);

    return g;
}

Graph rmat_graph(int n, int m, double a, double b, double c, double d, int seed) {
    // Round n up to power of 2
    int N = 1;
    while (N < n) N *= 2;
    
    Graph g(n, true); // Directed by default for R-MAT
    auto rng = get_rng(seed);
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    // Normalize probabilities just in case
    double sum = a + b + c + d;
    a /= sum; b /= sum; c /= sum; d /= sum;

    double ab = a + b;
    double abc = a + b + c;

    for (int i = 0; i < m; ++i) {
        int u = 0;
        int v = 0;
        int step = N / 2;
        
        int curr_n = N;

        while (step >= 1) {
            double r = dist(rng);
            if (r < a) {
                // Top-Left: u in [0, N/2), v in [0, N/2)
            } else if (r < ab) {
                // Top-Right: u in [0, N/2), v in [N/2, N)
                v += step;
            } else if (r < abc) {
                // Bottom-Left: u in [N/2, N), v in [0, N/2)
                u += step;
            } else {
                // Bottom-Right: u in [N/2, N), v in [N/2, N)
                u += step;
                v += step;
            }
            step /= 2;
        }

        // Map back to [0, n-1] range if needed?
        // R-MAT usually generates edges in [0, N-1].
        // If generated node >= n, we can discard or map?
        // Standard practice: discard self-loops or out-of-bounds?
        // Or wrap around? Discarding reduces m.
        // Let's discard if >= n.
        if (u < n && v < n && u != v) {
            g.add_edge(u, v);
        }
    }
    return g;
}

Graph random_geometric_graph(int n, double radius, int dim, int seed) {
    Graph g(n, false);
    if (n <= 1) return g;

    auto rng = get_rng(seed);
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    struct Point {
        std::vector<double> coords;
    };

    std::vector<Point> points(n);
    for (int i = 0; i < n; ++i) {
        points[i].coords.resize(dim);
        for (int d = 0; d < dim; ++d) {
            points[i].coords[d] = dist(rng);
        }
    }

    double r2 = radius * radius;

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            double d2 = 0;
            for (int d = 0; d < dim; ++d) {
                double diff = points[i].coords[d] - points[j].coords[d];
                d2 += diff * diff;
            }
            if (d2 <= r2) {
                g.add_edge(i, j);
            }
        }
    }
    return g;
}

} // namespace generator
} // namespace graphlib
