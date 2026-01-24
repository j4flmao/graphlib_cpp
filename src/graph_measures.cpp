#include "graphlib/graph_measures.h"
#include <queue>
#include <algorithm>
#include <limits>
#include <stack>
#include <vector>
#include <set>
#include <cmath>
#include <iterator>
#include <map>
#include <random>

namespace graphlib {

std::vector<int> eccentricity(const Graph& g) {
    int n = g.vertex_count();
    std::vector<int> ecc(n, 0);
    
    if (n == 0) return {};

    for (int start_node = 0; start_node < n; ++start_node) {
        std::vector<int> dist(n, -1);
        std::queue<int> q;
        
        dist[start_node] = 0;
        q.push(start_node);
        
        int max_dist = 0;
        int visited_count = 0;
        
        while(!q.empty()) {
            int u = q.front();
            q.pop();
            visited_count++;
            
            if (dist[u] > max_dist) {
                max_dist = dist[u];
            }
            
            Edge* e = g.get_edges(u);
            while(e) {
                int v = e->to;
                if (dist[v] == -1) {
                    dist[v] = dist[u] + 1;
                    q.push(v);
                }
                e = e->next;
            }
        }
        
        if (visited_count != n) {
            // For disconnected components, eccentricity is infinite.
            // But if we consider only reachable nodes?
            // Standard definition: infinity. We return -1.
            ecc[start_node] = -1;
        } else {
            ecc[start_node] = max_dist;
        }
    }
    return ecc;
}

int diameter(const Graph& g) {
    std::vector<int> ecc = eccentricity(g);
    if (ecc.empty()) return 0;
    
    int max_e = -1;
    for (int e : ecc) {
        if (e == -1) return -1;
        if (e > max_e) max_e = e;
    }
    return max_e;
}

int radius(const Graph& g) {
    std::vector<int> ecc = eccentricity(g);
    if (ecc.empty()) return 0;
    
    int min_e = std::numeric_limits<int>::max();
    bool any_connected = false;
    
    for (int e : ecc) {
        if (e != -1) {
            if (e < min_e) min_e = e;
            any_connected = true;
        }
    }
    
    if (!any_connected) return -1;
    return min_e;
}

std::vector<int> center(const Graph& g) {
    std::vector<int> ecc = eccentricity(g);
    int r = radius(g);
    if (r == -1) return {};
    
    std::vector<int> c;
    for (int i = 0; i < static_cast<int>(ecc.size()); ++i) {
        if (ecc[i] == r) {
            c.push_back(i);
        }
    }
    return c;
}

std::vector<int> periphery(const Graph& g) {
    std::vector<int> ecc = eccentricity(g);
    int d = diameter(g);
    if (d == -1) return {};
    
    std::vector<int> p;
    for (int i = 0; i < static_cast<int>(ecc.size()); ++i) {
        if (ecc[i] == d) {
            p.push_back(i);
        }
    }
    return p;
}

std::vector<double> closeness_centrality(const Graph& g) {
    int n = g.vertex_count();
    std::vector<double> closeness(n, 0.0);
    
    for (int u = 0; u < n; ++u) {
        std::vector<int> dist(n, -1);
        std::queue<int> q;
        
        dist[u] = 0;
        q.push(u);
        
        long long sum_dist = 0;
        int reachable_count = 0;
        
        while(!q.empty()) {
            int curr = q.front();
            q.pop();
            
            if (curr != u) {
                sum_dist += dist[curr];
                reachable_count++;
            }
            
            Edge* e = g.get_edges(curr);
            while(e) {
                int v = e->to;
                if (dist[v] == -1) {
                    dist[v] = dist[curr] + 1;
                    q.push(v);
                }
                e = e->next;
            }
        }
        
        if (reachable_count > 0) {
            // Standard definition for connected graph: (n-1) / sum_dist
            // For disconnected: (reachable_count) / sum_dist * (reachable_count / (n-1)) ?
            // Let's stick to (n-1)/sum_dist and 0 if disconnected (or isolated).
            // If sum_dist == 0 (isolated), result 0.
            
            // If we want to handle disconnected components strictly:
            // Wasserman and Faust formula: (N-1) / sum(d(u,v)) is only for connected.
            // We'll use (n-1)/sum_dist, but if not all reachable, maybe it should be 0?
            // Let's use simple reciprocal sum for now? No, the header said (n-1)/sum.
            // If disconnected, sum_dist only includes reachable.
            // If we strictly follow "closeness in component", we'd use reachable_count instead of n-1.
            // But let's use n-1 to penalize disconnection.
            if (n > 1) {
                closeness[u] = static_cast<double>(n - 1) / sum_dist;
            }
        }
    }
    return closeness;
}

std::vector<double> betweenness_centrality(const Graph& g) {
    int n = g.vertex_count();
    std::vector<double> cb(n, 0.0);
    
    for (int s = 0; s < n; ++s) {
        std::stack<int> S;
        std::vector<std::vector<int>> P(n);
        std::vector<int> sigma(n, 0);
        std::vector<int> d(n, -1);
        std::queue<int> Q;
        
        sigma[s] = 1;
        d[s] = 0;
        Q.push(s);
        
        while(!Q.empty()) {
            int v = Q.front();
            Q.pop();
            S.push(v);
            
            Edge* e = g.get_edges(v);
            while(e) {
                int w = e->to;
                // Path discovery
                if (d[w] < 0) {
                    d[w] = d[v] + 1;
                    Q.push(w);
                }
                // Path counting
                if (d[w] == d[v] + 1) {
                    sigma[w] += sigma[v];
                    P[w].push_back(v);
                }
                e = e->next;
            }
        }
        
        std::vector<double> delta(n, 0.0);
        while(!S.empty()) {
            int w = S.top();
            S.pop();
            for (int v : P[w]) {
                delta[v] += (static_cast<double>(sigma[v]) / sigma[w]) * (1.0 + delta[w]);
            }
            if (w != s) {
                cb[w] += delta[w];
            }
        }
    }
    
    // For undirected graphs, each shortest path is counted twice (s->t and t->s).
    // Divide by 2 to normalize.
    if (!g.is_directed()) {
        for (double& val : cb) {
            val /= 2.0;
        }
    }
    
    return cb;
}

std::vector<double> pagerank(const Graph& g, double d, int iterations) {
    int n = g.vertex_count();
    if (n == 0) return {};
    
    std::vector<double> pr(n, 1.0 / n);
    std::vector<double> new_pr(n);
    std::vector<int> out_degree(n, 0);
    
    // Calculate out-degrees
    for (int u = 0; u < n; ++u) {
        Edge* e = g.get_edges(u);
        while (e) {
            out_degree[u]++;
            e = e->next;
        }
    }
    
    for (int iter = 0; iter < iterations; ++iter) {
        double sink_sum = 0.0;
        for (int u = 0; u < n; ++u) {
            if (out_degree[u] == 0) {
                sink_sum += pr[u];
            }
        }
        
        double base_val = (1.0 - d) / n;
        double sink_share = d * (sink_sum / n);
        
        for (int u = 0; u < n; ++u) {
            new_pr[u] = base_val + sink_share;
        }
        
        for (int u = 0; u < n; ++u) {
            if (out_degree[u] > 0) {
                double share = pr[u] / out_degree[u];
                Edge* e = g.get_edges(u);
                while (e) {
                    new_pr[e->to] += d * share;
                    e = e->next;
                }
            }
        }
        
        pr = new_pr;
    }
    
    return pr;
}

std::vector<double> clustering_coefficient(const Graph& g) {
    int n = g.vertex_count();
    std::vector<double> cc(n, 0.0);
    
    // Convert to adjacency sets for fast lookup
    std::vector<std::set<int>> adj(n);
    for (int u = 0; u < n; ++u) {
        Edge* e = g.get_edges(u);
        while(e) {
            adj[u].insert(e->to);
            e = e->next;
        }
    }
    
    for (int u = 0; u < n; ++u) {
        // Collect neighbors
        // Since we treat graph as undirected, we should consider all neighbors.
        // The graph structure might be directed or undirected.
        // If directed=false was used, we have u->v and v->u.
        // So adj[u] contains all neighbors.
        
        size_t k = adj[u].size();
        if (k < 2) {
            cc[u] = 0.0;
            continue;
        }
        
        int edges_between_neighbors = 0;
        for (auto it1 = adj[u].begin(); it1 != adj[u].end(); ++it1) {
            for (auto it2 = std::next(it1); it2 != adj[u].end(); ++it2) {
                int v = *it1;
                int w = *it2;
                
                // Check if edge exists between v and w
                // In undirected graph, check v->w or w->v.
                // Since we built adj from all edges, if undirected, v->w exists implies w->v exists.
                if (adj[v].count(w)) {
                    edges_between_neighbors++;
                }
            }
        }
        
        // For undirected graph: possible edges = k * (k-1) / 2
        // We counted each edge once in the inner loop (since it1 < it2).
        double possible_edges = static_cast<double>(k) * (k - 1) / 2.0;
        cc[u] = static_cast<double>(edges_between_neighbors) / possible_edges;
    }
    
    return cc;
}

double average_clustering_coefficient(const Graph& g) {
    std::vector<double> cc = clustering_coefficient(g);
    if (cc.empty()) return 0.0;
    
    double sum = 0.0;
    for (double c : cc) sum += c;
    return sum / cc.size();
}

std::vector<int> core_number(const Graph& g) {
    int n = g.vertex_count();
    std::vector<int> degrees(n, 0);
    std::vector<int> core(n, 0);
    
    // Calculate initial degrees
    for (int u = 0; u < n; ++u) {
        Edge* e = g.get_edges(u);
        while(e) {
            degrees[u]++;
            e = e->next;
        }
    }
    
    // Bin sort / Bucket sort
    int max_deg = 0;
    for (int d : degrees) max_deg = std::max(max_deg, d);
    
    std::vector<std::vector<int>> bin(max_deg + 1);
    for (int u = 0; u < n; ++u) {
        bin[degrees[u]].push_back(u);
    }
    
    std::vector<int> pos(n); // Position of vertex in the bin array (not needed if we just iterate?)
    // We need to locate vertices to move them.
    // Actually, simpler O(V+E) implementation:
    // array of vertices sorted by degree.
    // pos[u] = index of u in the sorted array.
    
    std::vector<int> vert(n);
    std::vector<int> bin_start(max_deg + 2, 0);
    
    // Count frequencies
    for (int d : degrees) bin_start[d]++;
    
    // Cumulative sum
    int start = 0;
    for (int d = 0; d <= max_deg; ++d) {
        int count = bin_start[d];
        bin_start[d] = start;
        start += count;
    }
    
    // Fill vert array and pos array
    for (int u = 0; u < n; ++u) {
        pos[u] = bin_start[degrees[u]];
        vert[pos[u]] = u;
        bin_start[degrees[u]]++;
    }
    
    // Restore bin_start
    for (int d = max_deg; d >= 1; --d) {
        bin_start[d] = bin_start[d-1];
    }
    bin_start[0] = 0;
    
    // Process
    for (int i = 0; i < n; ++i) {
        int u = vert[i];
        core[u] = degrees[u];
        
        Edge* e = g.get_edges(u);
        while(e) {
            int v = e->to;
            if (degrees[v] > degrees[u]) {
                // Decrease degree of v and move it in sorted array
                int deg_v = degrees[v];
                int pos_v = pos[v];
                int pos_start = bin_start[deg_v];
                
                // Swap v with the vertex at the start of its bin
                if (pos_v != pos_start) {
                    int w = vert[pos_start];
                    vert[pos_v] = w;
                    vert[pos_start] = v;
                    pos[w] = pos_v;
                    pos[v] = pos_start;
                }
                
                bin_start[deg_v]++;
                degrees[v]--;
            }
            e = e->next;
        }
    }
    
    return core;
}

std::vector<double> eigenvector_centrality(const Graph& g, int iterations) {
    int n = g.vertex_count();
    if (n == 0) return {};
    
    std::vector<double> x(n, 1.0 / n);
    std::vector<double> new_x(n, 0.0);
    
    for (int iter = 0; iter < iterations; ++iter) {
        std::fill(new_x.begin(), new_x.end(), 0.0);
        
        // x_v = sum_{u -> v} x_u
        // We also add x_u to new_x[u] (effectively A + I) to ensure convergence
        // on bipartite graphs (like star graphs) where standard power iteration oscillates.
        // Eigenvectors of A and A+I are the same.
        for (int u = 0; u < n; ++u) {
            new_x[u] += x[u]; // Self-loop contribution
            
            Edge* e = g.get_edges(u);
            while(e) {
                int v = e->to;
                new_x[v] += x[u];
                e = e->next;
            }
        }
        
        // Normalize (Euclidean norm)
        double norm = 0.0;
        for (double val : new_x) norm += val * val;
        norm = std::sqrt(norm);
        
        if (norm > 1e-9) {
            for (int u = 0; u < n; ++u) {
                new_x[u] /= norm;
            }
        }
        
        x = new_x;
    }
    
    return x;
}

std::pair<std::vector<double>, std::vector<double>> hits(const Graph& g, int iterations) {
    int n = g.vertex_count();
    if (n == 0) return {{}, {}};
    
    std::vector<double> h(n, 1.0);
    std::vector<double> a(n, 1.0);
    
    for (int iter = 0; iter < iterations; ++iter) {
        // Update Authorities: a_v = sum_{u -> v} h_u
        std::vector<double> new_a(n, 0.0);
        for (int u = 0; u < n; ++u) {
            Edge* e = g.get_edges(u);
            while(e) {
                int v = e->to;
                new_a[v] += h[u];
                e = e->next;
            }
        }
        
        // Normalize Authorities
        double norm_a = 0.0;
        for (double val : new_a) norm_a += val * val;
        norm_a = std::sqrt(norm_a);
        if (norm_a > 1e-9) {
            for (int u = 0; u < n; ++u) new_a[u] /= norm_a;
        }
        a = new_a;
        
        // Update Hubs: h_u = sum_{u -> v} a_v
        std::vector<double> new_h(n, 0.0);
        for (int u = 0; u < n; ++u) {
            Edge* e = g.get_edges(u);
            while(e) {
                int v = e->to;
                new_h[u] += a[v];
                e = e->next;
            }
        }
        
        // Normalize Hubs
        double norm_h = 0.0;
        for (double val : new_h) norm_h += val * val;
        norm_h = std::sqrt(norm_h);
        if (norm_h > 1e-9) {
            for (int u = 0; u < n; ++u) new_h[u] /= norm_h;
        }
        h = new_h;
    }
    
    return {h, a};
}

std::vector<double> katz_centrality(const Graph& g, double alpha, double beta, int iterations) {
    int n = g.vertex_count();
    if (n == 0) return {};
    
    std::vector<double> x(n, 0.0); // Start with 0 or beta? 
    // Usually start with random or zero. Let's start with 0.
    
    std::vector<double> new_x(n, 0.0);
    
    for (int iter = 0; iter < iterations; ++iter) {
        std::fill(new_x.begin(), new_x.end(), beta);
        
        // x_v = alpha * sum_{u -> v} x_u + beta
        for (int u = 0; u < n; ++u) {
            if (std::abs(x[u]) < 1e-9) continue;
            
            Edge* e = g.get_edges(u);
            while(e) {
                int v = e->to;
                new_x[v] += alpha * x[u];
                e = e->next;
            }
        }
        
        // Normalize? Katz usually converges if alpha < 1/lambda_max.
        // But for safety against overflow, we can normalize.
        // However, the standard definition is absolute.
        // If we normalize, it becomes Eigenvector Centrality variant.
        // We will NOT normalize by default, assuming user provides valid alpha.
        // But if values explode, it's user's fault (alpha too large).
        x = new_x;
    }
    
    return x;
}

// Helper to get sorted neighbors
static std::vector<int> get_sorted_neighbors(const Graph& g, int u) {
    std::vector<int> neighbors;
    Edge* e = g.get_edges(u);
    while(e) {
        neighbors.push_back(e->to);
        e = e->next;
    }
    std::sort(neighbors.begin(), neighbors.end());
    // Remove duplicates if any (multi-graph support?)
    neighbors.erase(std::unique(neighbors.begin(), neighbors.end()), neighbors.end());
    return neighbors;
}

double jaccard_index(const Graph& g, int u, int v) {
    if (u < 0 || u >= g.vertex_count() || v < 0 || v >= g.vertex_count()) return 0.0;
    
    std::vector<int> nu = get_sorted_neighbors(g, u);
    std::vector<int> nv = get_sorted_neighbors(g, v);
    
    std::vector<int> intersection;
    std::set_intersection(nu.begin(), nu.end(), nv.begin(), nv.end(),
                          std::back_inserter(intersection));
                          
    size_t intersection_size = intersection.size();
    size_t union_size = nu.size() + nv.size() - intersection_size;
    
    if (union_size == 0) return 0.0;
    
    return static_cast<double>(intersection_size) / union_size;
}

double adamic_adar_index(const Graph& g, int u, int v) {
    if (u < 0 || u >= g.vertex_count() || v < 0 || v >= g.vertex_count()) return 0.0;

    std::vector<int> nu = get_sorted_neighbors(g, u);
    std::vector<int> nv = get_sorted_neighbors(g, v);
    
    std::vector<int> intersection;
    std::set_intersection(nu.begin(), nu.end(), nv.begin(), nv.end(),
                          std::back_inserter(intersection));
                          
    double score = 0.0;
    for (int w : intersection) {
        // Degree of w
        int deg = 0;
        Edge* e = g.get_edges(w);
        while(e) {
            deg++;
            e = e->next;
        }
        
        if (deg > 1) {
            score += 1.0 / std::log(static_cast<double>(deg));
        }
    }
    
    return score;
}

std::vector<int> label_propagation_communities(const Graph& g, int max_iterations, int seed) {
    int n = g.vertex_count();
    std::vector<int> labels(n);
    // Initialize labels
    for(int i=0; i<n; ++i) labels[i] = i;

    std::vector<int> nodes(n);
    for(int i=0; i<n; ++i) nodes[i] = i;

    std::mt19937 rng(seed == -1 ? std::random_device{}() : seed);

    bool changed = true;
    for (int iter = 0; iter < max_iterations && changed; ++iter) {
        changed = false;
        std::shuffle(nodes.begin(), nodes.end(), rng);

        for (int u : nodes) {
            std::map<int, int> label_counts;
            Edge* e = g.get_edges(u);
            while(e) {
                int v = e->to;
                label_counts[labels[v]]++;
                e = e->next;
            }

            if (label_counts.empty()) continue;

            // Find max frequency
            int max_freq = -1;
            for(auto const& [label, count] : label_counts) {
                if(count > max_freq) max_freq = count;
            }

            // Collect best labels
            std::vector<int> best_labels;
            for(auto const& [label, count] : label_counts) {
                if(count == max_freq) best_labels.push_back(label);
            }

            if (!best_labels.empty()) {
                // If current label is not among best, pick one
                // Or simply always pick random best to avoid stagnation?
                // Standard LPA picks random from best.
                
                // Optimization: check if current label is already one of the best
                bool current_is_best = false;
                for(int l : best_labels) {
                    if (l == labels[u]) {
                        current_is_best = true;
                        break;
                    }
                }
                
                // If current label is best, we might stay (but random choice is better for ties)
                // However, strict LPA says: if multiple max, pick random.
                
                int chosen_label = best_labels[0];
                if (best_labels.size() > 1) {
                    std::uniform_int_distribution<int> dist(0, (int)best_labels.size() - 1);
                    chosen_label = best_labels[dist(rng)];
                }

                if (labels[u] != chosen_label) {
                    labels[u] = chosen_label;
                    changed = true;
                }
            }
        }
    }
    return labels;
}

double modularity(const Graph& g, const std::vector<int>& communities) {
    int n = g.vertex_count();
    if (n == 0) return 0.0;
    if (communities.size() != (size_t)n) return 0.0;

    double m = 0; // Total weight of edges (or number of edges in unweighted)
    // Actually modularity formula usually uses 2m.
    // For directed/weighted, we sum weights.
    // Assuming undirected graph for standard modularity def, but this works for directed if we interpret m as total degree/2?
    // Standard modularity is for undirected.
    // Let's assume undirected graph logic (sum of degrees / 2 = m).
    // Or just iterate all edges.

    // Calculate degrees and m
    std::vector<double> k(n, 0.0);
    double two_m = 0.0;

    for (int u = 0; u < n; ++u) {
        Edge* e = g.get_edges(u);
        while(e) {
            double w = e->weight; // Assuming weight 1 if not weighted? Graphlib uses double weight.
            // If weight is 0 or uninitialized, might be issue.
            // Usually we assume positive weights.
            k[u] += w;
            two_m += w;
            e = e->next;
        }
    }

    if (two_m == 0) return 0.0;

    double Q = 0.0;

    // Iterate over all edges for A_ij part
    // Q = (1/2m) * sum_{ij} [ A_ij - (k_i * k_j) / (2m) ] * delta(c_i, c_j)
    // This is O(n^2) if we iterate all pairs.
    // Better: Q = (1/2m) * sum_{c} [ (Sigma_in) - (Sigma_tot)^2 / 2m ]
    // Sigma_in: sum of weights of edges INSIDE community c
    // Sigma_tot: sum of degrees of nodes in community c
    
    std::map<int, double> sigma_in;
    std::map<int, double> sigma_tot;

    for (int u = 0; u < n; ++u) {
        int c_u = communities[u];
        sigma_tot[c_u] += k[u];

        Edge* e = g.get_edges(u);
        while(e) {
            int v = e->to;
            int c_v = communities[v];
            if (c_u == c_v) {
                // For undirected graph stored as directed pairs (u->v, v->u),
                // this edge counts towards sigma_in of community c_u.
                // We will sum all such edges.
                sigma_in[c_u] += e->weight;
            }
            e = e->next;
        }
    }

    double sum_term = 0.0;
    // Iterate over all communities present
    // Collect all unique community IDs
    std::vector<int> comm_ids;
    for(auto const& [id, val] : sigma_tot) {
        comm_ids.push_back(id);
    }

    for (int c : comm_ids) {
        double in = sigma_in[c]; // This sums (u,v) and (v,u) if both in c. Correct for 2m formula.
        double tot = sigma_tot[c];
        sum_term += (in / two_m) - std::pow(tot / two_m, 2);
    }

    return sum_term;
}

std::string weisfeiler_lehman_hash(const Graph& g, int iterations) {
    int n = g.vertex_count();
    if (n == 0) return "0";

    // Initial labels: degrees
    std::vector<int> labels(n);
    for (int i = 0; i < n; ++i) {
        int deg = 0;
        Edge* e = g.get_edges(i);
        while(e) { deg++; e = e->next; }
        labels[i] = deg;
    }

    // Map signature to new label
    // To keep it deterministic, we need to sort signatures and assign sequential IDs.
    // Signature: pair<old_label, sorted_neighbor_labels>
    
    for (int iter = 0; iter < iterations; ++iter) {
        std::vector<std::pair<int, std::vector<int>>> signatures(n);
        
        for (int u = 0; u < n; ++u) {
            std::vector<int> neighbor_labels;
            Edge* e = g.get_edges(u);
            while(e) {
                neighbor_labels.push_back(labels[e->to]);
                e = e->next;
            }
            std::sort(neighbor_labels.begin(), neighbor_labels.end());
            signatures[u] = {labels[u], neighbor_labels};
        }

        // Compress signatures
        // Collect all unique signatures
        std::vector<std::pair<int, std::vector<int>>> unique_sigs = signatures;
        std::sort(unique_sigs.begin(), unique_sigs.end());
        unique_sigs.erase(std::unique(unique_sigs.begin(), unique_sigs.end()), unique_sigs.end());

        // Map signature to index
        // We can use a map or binary search since unique_sigs is sorted.
        // Map is easier.
        std::map<std::pair<int, std::vector<int>>, int> sig_map;
        for (size_t i = 0; i < unique_sigs.size(); ++i) {
            sig_map[unique_sigs[i]] = (int)i;
        }

        // Update labels
        for (int u = 0; u < n; ++u) {
            labels[u] = sig_map[signatures[u]];
        }
    }

    // Final hash: sorted list of labels
    std::sort(labels.begin(), labels.end());
    
    // Convert to string
    std::string hash = "";
    for (int l : labels) {
        hash += std::to_string(l) + ";";
    }
    
    // Hash the string to make it compact? Or just return the long string?
    // User wants "hash". Let's return the string representation of the multiset.
    // Or use std::hash to collapse it.
    // std::hash is not guaranteed to be consistent across runs/platforms (though usually is for same STL).
    // Let's return the full string for correctness and determinism.
    // Maybe hash it with a simple custom hash if too long.
    // For now, return string.
    
    // To make it shorter, let's hash it.
    size_t seed = 0;
    for (int l : labels) {
        // Boost's hash_combine logic
        seed ^= std::hash<int>{}(l) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return std::to_string(seed);
}

}
